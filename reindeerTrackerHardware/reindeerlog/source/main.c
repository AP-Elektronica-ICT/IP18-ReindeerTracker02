/**
 * This is template for main module created by New Kinetis SDK 2.x Project Wizard. Enjoy!
 **/

#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"

#include "fsl_gpio.h"
#include "fsl_uart.h"
#include "fsl_port.h"
#include "fsl_common.h"
#include "fsl_i2c.h"
#include "fsl_smc.h"
#include "fsl_lptmr.h"
#include "fsl_dspi.h"

#include <stdio.h>
#include <stdlib.h>

#include "acc_func.h"
#include "i2c_func.h"
#include "sdcard_io.h"
#include "ff.h"
#include "adc_func.h"
#include "fsl_rtc.h"

#define X_AXIS 	0
#define Y_AXIS 	1
#define Z_AXIS 	2
#define ACC_TEMP 3

#define USE_SD 1

#define SINGLE_ENTRY_SIZE 65
#define ENTRY_HOWMANY 200

uint32_t uartClkSrcFreq;

void delay(uint32_t del) {
	for (; del > 1; del--) {
		__asm("nop");
	}
}


/*
 * InitPcUart
 *
 * Initialize UART0 that is connected through the bootloader chip to USB serial
 * so we can have communication with computer terminal
 *
 * Will use UART interrupt for receiving
 *
 */

void InitPcUart() {
	CLOCK_EnableClock(kCLOCK_PortB);
	CLOCK_EnableClock(kCLOCK_Uart0);

	PORT_SetPinMux(PORTB, 16u, kPORT_MuxAlt3); /* PORTB16 (pin 62) is configured as UART0_RX */
	PORT_SetPinMux(PORTB, 17u, kPORT_MuxAlt3); /* PORTB17 (pin 63) is configured as UART0_TX */

uartClkSrcFreq= BOARD_DEBUG_UART_CLK_FREQ;

	uart_config_t user_config;
	UART_GetDefaultConfig(&user_config);
	user_config.baudRate_Bps = 57600U;
	user_config.enableTx = true;
	user_config.enableRx = true;
	UART_Init(UART0, &user_config, uartClkSrcFreq); //initialize with default clock speed 20,971520 Mhz

}
void initRtc(){

	rtc_config_t rtc_config;
	RTC_GetDefaultConfig(&rtc_config);
	rtc_config.supervisorAccess = true;
	RTC_Init(RTC0, &rtc_config);
	RTC0->CR |= RTC_CR_OSCE_MASK;
	RTC0->SR |= 0x00000010;

}

void initTimer() {

	lptmr_config_t lptmr_config;
	LPTMR_GetDefaultConfig(&lptmr_config);
	lptmr_config.bypassPrescaler = false;
	lptmr_config.value = kLPTMR_Prescale_Glitch_5;
	lptmr_config.prescalerClockSource = kLPTMR_PrescalerClock_0;
	EnableIRQ(LPTMR0_IRQn);
	LPTMR_Init(LPTMR0, &lptmr_config);
	LPTMR_SetTimerPeriod(LPTMR0, 3000);  // 3000 for 20hz data rat
}

void LPTMR0_IRQHandler() {

	LPTMR0 -> CSR |= LPTMR_CSR_TCF_MASK;		// Clear the interrupt flag
	while ( LPTMR0 -> CSR & LPTMR_CSR_TCF_MASK  ) {

	}

}

int main(void) {

	CLOCK_BootToBlpiMode(MCG_SC_FCRDIV(0), kMCG_IrcFast, 0x3);
	BOARD_InitPins();
	BOARD_InitDebugConsole();
	initI2C();
	initAdc();
	initRtc();
	initTimer();

	LPTMR_StartTimer(LPTMR0);
	uint32_t seconds = 0, buffer_pointer = 0;


	static const gpio_pin_config_t LED_configOutput = { kGPIO_DigitalOutput, /* use as output pin */
	1, /* initial value */
	};

	GPIO_PinInit(GPIOB, 22u, &LED_configOutput);
	GPIO_PinInit(GPIOB, 21u, &LED_configOutput);

	 /*
	  * allocate space from RAM to store logging data block
	  *
	  * one log line is SINGLE_ENTRY_SIZE bytes long:
	  *
	  * X(frdm);Y(frdm);Z(frdm);X(GY61);Y(GY61);Z(GY61)\r\n
	  *
	  * one value takes 7 bytes: 6 digit value + ; separator
	  * \r\n takes 2 bytes (\ is combined with the letter in compilation)
	  * so one entry line is 43 bytes
	  *
	  * ENTRY_HOWMANY is how many lines we want to store at once
	  *
	  */

	static char logresult_buffer[SINGLE_ENTRY_SIZE * ENTRY_HOWMANY];

	//flush space with zeros just to be sure
	memset(logresult_buffer,0,sizeof(logresult_buffer));

	acc_init(); //init accelerometer
	InitPcUart();
	initSpiConfig();

#if USE_SD

	SD_startComm();
	/*
	 * Declare FatFs objects
	 */
	FRESULT res; //FatFs operation result code object
	FIL fil; //working file object
	static FATFS fss; //declare a fatfs object

    res = f_mount(&fss, "0:", 0); //Mount sd card
    SD_error(res,"mount"); //check for operation error

    res = f_mkdir("testi");

    /*
     * First open log count file to update number of logs.
     * This is to catch if the logging has failed and restarted...
     */
    res = f_open(&fil, "testi/logcount.txt", FA_WRITE | FA_READ | FA_OPEN_ALWAYS);
    SD_error(res,"conf file open");

    //Make a small read buffer for the log count..
    char readbuf[10];
    //Read the string from the file. After this, file pointer will point to the end of string
    f_gets(readbuf, 10, &fil);

    //Convert the log counter number to integer using AsciiToInteger(atoi). Log counter number is the 5th char of the string
    uint8_t log_count = atoi(&readbuf[4]);

    printf("log count %s %d\r\n",readbuf,log_count);
    //delay(7000000);

    //Increment log counter
    log_count++;

    //Now we need to reset the file pointer so we can overwrite new counter value..
    fil.fptr = 0;
    //Overwrite new
    f_printf(&fil,"LOG:%d\r\n",log_count);

    res = f_close(&fil); //Close count file..
    SD_error(res, "close");

    static char filename[25];

    sprintf(filename,"testi/testilog%d.csv",log_count);

    res = f_open(&fil, filename, FA_WRITE | FA_READ | FA_CREATE_ALWAYS ); //open log file, overwrite old
    SD_error(res,"file open"); //check for operation error

    char text[] = "Start logging\r\n Time since start(s);Int acc X;Int acc Y;Int acc Z;Ext acc X;Ext acc Y;Ext acc Z;Ext temp;Int temp\r\n";

    f_printf(&fil, text); //print text to file

    res = f_close(&fil); //close file for now
    SD_error(res, "close");

    DSPI_Deinit(SPI1);
	SPI_deactivate_pins();

	printf("clk %ld\r\n",uartClkSrcFreq);

#endif

	while (1) {

			for(uint16_t k = 0; k < ENTRY_HOWMANY; k++) //Read all 6 values from accelerometers and save to logging buffer for ENTRY_HOWMANY times
			{

				int16_t adc_acc_x = ADC_read16b(1) - 32900;
				int16_t adc_acc_y = ADC_read16b(2) - 32900;		//Accelerometer GY-61
				int16_t adc_acc_z = ADC_read16b(3) - 32900;

				int32_t temp = (65535 - ADC_read16b(4)) / 541 -36;

				/* 22c temperature, Vout = 1,6V, Vcc = 3,3V,
				R = 10,69kohm @23.5c
				R = 3893ohm @50c Vout = 0.925V @50c ADC = 18369
				R = 135.2K @- 40c Vout = 3.073V     ADC = 61027

				measured ADC @3c = 45600
				measured ADC @64c = 12700
				541 units per C seems to give good accuracy, and -34 constant
				at 0C temp shows 3c too low
				at 40c temp shows 3c too high

				22.2. changed constant to -36
				now room temperature matches with FXOS
				at -17c external sensor showed -15 (with -34 constant) so they should match now too
				 */

				int16_t acc_val_x = read_acc_axis(X_AXIS); //read accelerometer X axis
				int16_t acc_val_y = read_acc_axis(Y_AXIS); //read accelerometer y axis	//FRDM integrated accelerometer
				int16_t acc_val_z = read_acc_axis(Z_AXIS); //read accelerometer z axis
				int16_t acc_temp = read_acc_axis(ACC_TEMP); //read accelerometer temperature.

				seconds = RTC0->TSR;

				uint16_t printlen = sprintf(logresult_buffer+buffer_pointer,"%ld;%d;%d;%d;%d;%d;%d;%d;%d\r\n",seconds,acc_val_x, acc_val_y, acc_val_z,
						adc_acc_x, adc_acc_y, adc_acc_z, (int)temp, acc_temp); //write new log value line


				//printf("%s\r\n",logresult_buffer+buffer_pointer);
				buffer_pointer += printlen;
				// prescaler 64, 62,5KHz clock

				LPTMR0 -> CNR = 0;
				NVIC_ClearPendingIRQ(LPTMR0_IRQn);

				LPTMR_EnableInterrupts(LPTMR0, LPTMR_CSR_TIE_MASK);		//Sets Timer Interrupt Enable bit to 1
				SMC ->PMPROT |= 0x20;
				SMC ->PMCTRL |= 0x02;

				uint8_t dummy = SMC ->PMCTRL;

				SCB ->SCR |= 0x04;

				__DSB();
				__WFI();
				__ISB();

				LPTMR_DisableInterrupts(LPTMR0, LPTMR_CSR_TIE_MASK);

			}

			/*
			 * Save log
			 */

#if USE_SD

			SD_startComm();
			res = f_open(&fil, filename, FA_WRITE | FA_READ | FA_OPEN_APPEND );
			SD_error(res,"file open"); //check for operation error

			f_printf(&fil, logresult_buffer);
			res = f_close(&fil);
			SD_error(res, "close");

			DSPI_Deinit(SPI1);
			SPI_deactivate_pins();

#else
			delay(15000);

#endif

			GPIO_ClearPinsOutput(GPIOB, 1<<22u); //light red LED
			delay(500);
			GPIO_SetPinsOutput(GPIOB, 1<<22u); //turn off red LED
			buffer_pointer=0;

	}

}


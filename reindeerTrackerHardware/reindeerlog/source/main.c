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

#define USE_SD 0

#define SINGLE_ENTRY_SIZE 62
#define ENTRY_HOWMANY 30

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

	uart_config_t user_config;
	UART_GetDefaultConfig(&user_config);
	user_config.baudRate_Bps = 57600U;
	user_config.enableTx = true;
	user_config.enableRx = true;
	UART_Init(UART0, &user_config, 20971520U); //initialize with default clock speed 20,971520 Mhz

}
void initRtc(){

	rtc_config_t rtc_config;
	RTC_GetDefaultConfig(&rtc_config);
	rtc_config.supervisorAccess = true;
	RTC_Init(RTC0, &rtc_config);
	RTC0->CR |= RTC_CR_OSCE_MASK;
	RTC0->SR |= 0x00000010;

}

int main(void) {

	BOARD_InitPins();
	BOARD_InitDebugConsole();
	initI2C();
	initAdc();
	initRtc();
	uint32_t seconds = 0;
	//rtc_datetime_t date_config;


	static const gpio_pin_config_t LED_configOutput = { kGPIO_DigitalOutput, /* use as output pin */
	1, /* initial value */
	};

	GPIO_PinInit(GPIOB, 22u, &LED_configOutput);
	GPIO_PinInit(GPIOB, 21u, &LED_configOutput);
	//GPIO_ClearPinsOutput(GPIOB, 1 << 21u);

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

	char logresult_buffer[SINGLE_ENTRY_SIZE * ENTRY_HOWMANY];

	//flush space with zeros just to be sure
	memset(logresult_buffer,0,sizeof(logresult_buffer));

	acc_init(); //init accelerometer




	InitPcUart();
	initSPI();

#if USE_SD
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
    delay(7000000);

    //Increment log counter
    log_count++;

    //Now we need to reset the file pointer so we can overwrite new counter value..
    fil.fptr = 0;
    //Overwrite new
    f_printf(&fil,"LOG:%d\r\n",log_count);

    res = f_close(&fil); //Close count file..
    SD_error(res, "close");

    res = f_open(&fil, "testi/testilog.csv", FA_WRITE | FA_READ | FA_CREATE_ALWAYS ); //open log file, overwrite old
    SD_error(res,"file open"); //check for operation error

    //FSIZE_t size = f_size(&fil);
    //f_expand(&fil, 20000,1);
   // res = f_lseek(&fil, size);
    //SD_error(res, "seek");

    char text[] = "Start logging\r\n Int acc X;Int acc Y;Int acc Z;Ext acc X;Ext acc Y;Ext acc Z;Ext temp\r\n";

    f_printf(&fil, text); //print text to file

    res = f_close(&fil); //close file for now
    SD_error(res, "close");

#endif
    delay(7000000);
	while (1) {

		for(;;) //This was a 200 cycle loop but changed to infinite for now
		{

			for(uint8_t k = 0; k < ENTRY_HOWMANY; k++) //Read all 6 values from accelerometers and save to logging buffer for ENTRY_HOWMANY times
			{

				int16_t adc_acc_x = ADC_read16b(1) - 32900;
				int16_t adc_acc_y = ADC_read16b(2) - 32900;		//Accelerometer GY-61
				int16_t adc_acc_z = ADC_read16b(3) - 32900;


				int32_t temp = (65535 - ADC_read16b(4)) / 541 -34;
				//int32_t temp = ADC_read16b(4);
				/* 22c temperature, Vout = 1,6V, Vcc = 3,3V,
				R = 10,69kohm @23.5c
				R = 3893ohm @50c Vout = 0.925V @50c ADC = 18369
				R = 135.2K @- 40c Vout = 3.073V     ADC = 61027

				measured ADC @3c = 45600
				measured ADC @64c = 12700
				541 units per C seems to give good accuracy
				at 0C temp shows 3c too low
				at 40c temp shows 3c too high
				 */

				int16_t acc_val_x = read_acc_axis(X_AXIS); //read accelerometer X axis
				int16_t acc_val_y = read_acc_axis(Y_AXIS); //read accelerometer y axis	//FRDM integrated accelerometer
				int16_t acc_val_z = read_acc_axis(Z_AXIS); //read accelerometer z axis
				int16_t acc_temp = read_acc_axis(ACC_TEMP); //read accelerometer temperature.

				//printf("ADC_X: %d\t ADC_Y: %d\t ADC_Z: %d\t", adc_acc_x, adc_acc_y, adc_acc_z );  //Accelerometer GY-61
				//printf("ACC_X: %d\t ACC_Y: %d\t ACC_Z: %d Temp: %d\r\n", acc_val_x, acc_val_y, acc_val_z, (int)temp);  //FRDM integrated accelerometer
				seconds = RTC0->TSR;
				//printf("Seconds: %ld\r\n", seconds);

				//printf("Int Temp: %d\r\n", acc_temp);

					GPIO_SetPinsOutput(GPIOB, 1<<22u); //turn off red LED
				//uint32_t buffer_pointer = strlen(logresult_buffer); //get pointer to last value in RAM buffer

				printf("%ld;%d;%d;%d;%d;%d;%d;%d;%d\r\n",seconds,acc_val_x, acc_val_y, acc_val_z,
									adc_acc_x, adc_acc_y, adc_acc_z, (int)temp, acc_temp); //write new log value line

				//sprintf(logresult_buffer+buffer_pointer,"%ld;%d;%d;%d;%d;%d;%d;%d\r\n",seconds,acc_val_x, acc_val_y, acc_val_z,
					//	adc_acc_x, adc_acc_y, adc_acc_z, (int)temp); //write new log value line

				delay(150000);
			}

			GPIO_ClearPinsOutput(GPIOB, 1<<22u); //light red LED

			/*
			 * Save log
			 */
#if USE_SD
			res = f_open(&fil, "testi/testilog.csv", FA_WRITE | FA_READ | FA_OPEN_APPEND );
			SD_error(res,"file open"); //check for operation error

			f_printf(&fil, logresult_buffer);
			res = f_close(&fil);
			SD_error(res, "close");

			GPIO_SetPinsOutput(GPIOB, 1<<22u); //turn off red LED
#endif
			memset(logresult_buffer,0,sizeof(logresult_buffer)); //flush logging buffer

		}

	  //UART_print(buffer);


	printf("ready\r\n");

	while(1){ //stop here after logging run
		;
	}
	}


}




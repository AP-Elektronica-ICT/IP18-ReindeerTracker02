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
#include "acc_func.h"
#include "fsl_dspi.h"
#include "i2c_func.h"

#include "sdcard_io.h"
#include "ff.h"
#include "stdlib.h"

#include "adc_func.h"

#define RING_BUFFER_SIZE 64
#define RX_DATA_SIZE     64
#define X_AXIS 	0
#define Y_AXIS 	1
#define Z_AXIS 	2

#define SINGLE_ENTRY_SIZE 20
#define ENTRY_HOWMANY 20

char receiveData[RX_DATA_SIZE];
char rxBuffer[RING_BUFFER_SIZE];

char SimcomRecBuf[RING_BUFFER_SIZE];

volatile uint8_t buf_ptr = 0, simcom_buf_ptr = 0;
volatile uint8_t pc_str_rdy = 0, simcom_str_rdy = 0, carriages = 0;

void delay(uint32_t del) {
	for (; del > 1; del--) {
		__asm("nop");
	}
}

/*
 * UART_receive
 *
 * Check if a string has been received and copy it to receiveData buffer. Return 1, return 0 if string has not been received.
 *
 */

uint8_t UART_receive() {
	if (pc_str_rdy) {
		strcpy(receiveData, rxBuffer); //copy rxBuffer content to receiveData
		pc_str_rdy = 0;
		memset(rxBuffer, 0x00, sizeof(rxBuffer)); //flush rxBuffer
		return 1;
	}

	return 0;
}

/*
 * UART_print
 *
 * Send a string pointed by *data to UART one character at a time
 *
 */

void UART_print(char *data) {
	char c;
	while ((c = *data)) {
		while (!((UART0->S1) & 0x40)) {
		}
		UART0->D = c;
		data++; //increment pointer *data to send next character
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

	//UART_Deinit(UART0);

	uart_config_t user_config;
	UART_GetDefaultConfig(&user_config);
	user_config.baudRate_Bps = 57600U;
	user_config.enableTx = true;
	user_config.enableRx = true;
	UART_Init(UART0, &user_config, 20971520U); //initialize with default clock speed 20,971520 Mhz

	UART_EnableInterrupts(UART0, kUART_RxDataRegFullInterruptEnable);
}

int main(void) {

	BOARD_InitPins();
	BOARD_InitDebugConsole();
	initI2C();
	//initAdc();

	static const gpio_pin_config_t LED_configOutput = { kGPIO_DigitalOutput, /* use as output pin */
	1, /* initial value */
	};

	GPIO_PinInit(GPIOB, 22u, &LED_configOutput);
	GPIO_PinInit(GPIOB, 21u, &LED_configOutput);
	GPIO_ClearPinsOutput(GPIOB, 1 << 21u);

	__enable_irq();

	char logresult_buffer[SINGLE_ENTRY_SIZE * ENTRY_HOWMANY];

	memset(logresult_buffer,0,sizeof(logresult_buffer));

	acc_init(); //init accelerometer

	InitPcUart();
	EnableIRQ(UART0_RX_TX_IRQn);
	initSPI();

	FRESULT res;
	FIL fil;
	UINT bw,br;
	br = 0;

	static FATFS fss; //declare a fatfs object

    res = f_mount(&fss, "0:", 0); //Mount sd card
    SD_error(res,"mount"); //check for operation error

    res = f_mkdir("testi");
    res = f_open(&fil, "testi/testilog.txt", FA_WRITE | FA_READ | FA_OPEN_APPEND );
    SD_error(res,"file open"); //check for operation error

    char text[] = "Restart logging\r\n";

    FSIZE_t size = f_size(&fil); //get size of opened file

    //res = f_lseek(&fil, size); //move file pointer to end of file, so we can append to file without overwriting old
    //SD_error(res, "seek");
    f_printf(&fil, text);


    res = f_close(&fil);
    SD_error(res, "close");

	uint8_t buffer2[25];

	while (1) {

		for(uint8_t k = 0; k < ENTRY_HOWMANY; k++) //log some values to RAM
		{

			int16_t acc_val_x = read_acc_axis(X_AXIS); //read accelerometer X axis
			int16_t acc_val_y = read_acc_axis(Y_AXIS);
			int16_t acc_val_z = read_acc_axis(Z_AXIS);

			printf("X axis %d Y axis %d Z axis %d\r\n",acc_val_x, acc_val_y, acc_val_z); //print to console

			uint32_t buffer_pointer = strlen(logresult_buffer); //get pointer to last value in RAM buffer
			sprintf(logresult_buffer+buffer_pointer,"%d;%d;%d\n",acc_val_x, acc_val_y, acc_val_z); //write new log value line

			delay(100000);

		}

		/*
		 * Save log
		 */

		res = f_open(&fil, "testi/testilog.txt", FA_WRITE | FA_READ | FA_OPEN_APPEND );
		SD_error(res,"file open"); //check for operation error

		f_printf(&fil, logresult_buffer);

		res = f_close(&fil);
		SD_error(res, "close");

		memset(logresult_buffer,0,sizeof(logresult_buffer)); //flush logging buffer
		//delay(200000);

	}
}

void UART0_RX_TX_IRQHandler() {
	//GPIO_TogglePinsOutput(GPIOB,1<<21u);

	UART_ClearStatusFlags(UART0, kUART_RxDataRegFullFlag);
	UART_ClearStatusFlags(UART0, kUART_RxActiveEdgeFlag);
	char ch = UART0->D;
	rxBuffer[buf_ptr] = ch;
	buf_ptr++;

	if (ch == 0x0d) {
		pc_str_rdy = 1;
		buf_ptr = 0;

	}

}


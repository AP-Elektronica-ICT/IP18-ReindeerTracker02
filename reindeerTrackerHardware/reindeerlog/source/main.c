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
#include "i2c_func.h"
#include "sdcard_io.h"


#define RING_BUFFER_SIZE 64
#define RX_DATA_SIZE     64
#define X_AXIS 	0
#define Y_AXIS 	1
#define Z_AXIS 	2


volatile bool txFinished;
volatile bool rxFinished;
char receiveData[RX_DATA_SIZE];
char rxBuffer[RING_BUFFER_SIZE];

char SimcomRecBuf[RING_BUFFER_SIZE];

volatile uint8_t buf_ptr = 0, simcom_buf_ptr = 0;
volatile uint8_t pc_str_rdy = 0, simcom_str_rdy = 0, carriages=0;



void delay(uint32_t del)
{
	for(;del>1;del--)
	{
		__asm("nop");
	}
}

/*
 * UART_receive
 *
 * Check if a string has been received and copy it to receiveData buffer. Return 1, return 0 if string has not been received.
 *
 */

uint8_t UART_receive()
{
	if(pc_str_rdy)
	{
		strcpy(receiveData,rxBuffer); //copy rxBuffer content to receiveData
		pc_str_rdy =0;
		memset(rxBuffer,0x00,sizeof(rxBuffer));//flush rxBuffer
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

void UART_print(char *data)
{
	char c;
	while ((c=*data))
	{
		while(!((UART0 ->S1) & 0x40)){}
		UART0 ->D = c;
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

void InitPcUart()
{
	CLOCK_EnableClock(kCLOCK_PortB);
	CLOCK_EnableClock(kCLOCK_Uart0);

	PORT_SetPinMux(PORTB, 16u, kPORT_MuxAlt3);   /* PORTB16 (pin 62) is configured as UART0_RX */
	PORT_SetPinMux(PORTB, 17u, kPORT_MuxAlt3);   /* PORTB17 (pin 63) is configured as UART0_TX */

	//UART_Deinit(UART0);

	uart_config_t user_config;
	UART_GetDefaultConfig(&user_config);
	user_config.baudRate_Bps = 57600U;
	user_config.enableTx = true;
	user_config.enableRx = true;
	UART_Init(UART0,&user_config,20971520U);  //initialize with default clock speed 20,971520 Mhz

	UART_EnableInterrupts(UART0,kUART_RxDataRegFullInterruptEnable);
}

int main(void) {
  /* Init board hardware. */
  BOARD_InitPins();

  BOARD_InitDebugConsole();
  initI2C();

  static const gpio_pin_config_t LED_configOutput = {
  kGPIO_DigitalOutput,  /* use as output pin */
  1,  /* initial value */
  };

  GPIO_PinInit(GPIOB, 22u, &LED_configOutput);
  GPIO_PinInit(GPIOB, 21u, &LED_configOutput);

  GPIO_ClearPinsOutput(GPIOB, 1<<21u);

  //UART_EnableInterrupts(UART3,kUART_RxDataRegFullInterruptEnable);
  //EnableIRQ(UART3_RX_TX_IRQn);*/

  __enable_irq();

  char buffer[50];

  acc_init(); //init accelerometer

  initSPI();


InitPcUart();
EnableIRQ(UART0_RX_TX_IRQn);

cardInit();

uint8_t tmp = 0;

/*while(tmp != 0xff)
{
	//SPIsend(0xff,0,0);
	tmp = SPIread();
}*/


	//SPIsend(0xff,0,0);


	while(tmp != 0x01)
	{
	SPIsend_command2(0x00,0,0x95,0);


	tmp = SPIread();

	sprintf(buffer,"read CMD0: %x\r\n",tmp);
	UART_print(buffer);
	delay(300000);
	}

	UART_print("SD korti jyllii\r\n");

	SPIsend_command2(0x08,0x1aa,0x87,0);




	tmp = SPIread();

	sprintf(buffer,"read CMD8 1: %x\r\n",tmp);
	UART_print(buffer);

	tmp = SPIread();

	sprintf(buffer,"read CMD8 2: %x\r\n",tmp);
	UART_print(buffer);

	tmp = SPIread();

	sprintf(buffer,"read CMD8 3: %x\r\n",tmp);
	UART_print(buffer);

	tmp = SPIread();

	sprintf(buffer,"read CMD8 4: %x\r\n",tmp);
	UART_print(buffer);
	delay(300000);




  while(1)

  {
	  /*int16_t acc_val_x = read_acc_axis(X_AXIS); //read accelerometer X axis
	  int16_t acc_val_y = read_acc_axis(Y_AXIS);
	  int16_t acc_val_z = read_acc_axis(Z_AXIS);

	  float acc_float = acc_val_x * 0.000244 * 9.81;

	  char float_charbuf[10];

	  //acc_val_x = (int)acc_float;

	  itoa(acc_val_x,float_charbuf+ptr,10);

	  acc_float -= acc_val_x;

	  for(uint8_t ptr = 0;ptr<3;ptr++)
	  {
		  uint32_t buf = (int)(acc_float*10);

	  }

	  sprintf(buffer,"X axis %f Y axis %d Z axis %d\r\n",acc_float, acc_val_y, acc_val_z);*/

	  	 // UART_print(buffer);

	  /*cardInit();
	  SPIsend_command(0x00,0x95,0);
	  SPIread();*/
	  delay(200000);



  }
}

void UART0_RX_TX_IRQHandler()
{
	//GPIO_TogglePinsOutput(GPIOB,1<<21u);

	UART_ClearStatusFlags(UART0,kUART_RxDataRegFullFlag);
	UART_ClearStatusFlags(UART0,kUART_RxActiveEdgeFlag);
	char ch = UART0 ->D;
	rxBuffer[buf_ptr] = ch;
	buf_ptr++;

	if(ch == 0x0d)
	{
		pc_str_rdy = 1;
		buf_ptr = 0;

	}

}



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
	for(del;del>1;del--)
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
		while(!((UART0 ->S1) & 0x80)){}
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

  InitPcUart();

  initI2C();

  static const gpio_pin_config_t LED_configOutput = {
  kGPIO_DigitalOutput,  /* use as output pin */
  1,  /* initial value */
  };

  GPIO_PinInit(GPIOB, 22u, &LED_configOutput);
  GPIO_PinInit(GPIOB, 21u, &LED_configOutput);
  GPIO_PinInit(GPIOB, 9u, &LED_configOutput);

  GPIO_ClearPinsOutput(GPIOB, 1<<21u);

  //UART_EnableInterrupts(UART3,kUART_RxDataRegFullInterruptEnable);

  EnableIRQ(UART0_RX_TX_IRQn);
  //EnableIRQ(UART3_RX_TX_IRQn);*/

  char buffer[50];

  acc_init();

  while(1)

  {

	  if( UART_receive() )
	  {
		  UART_print(receiveData); //loopback data


		  if(strstr(receiveData,"ac") != NULL)

		  {
		  uint8_t accdata = accReadReg(0x2a);

		  sprintf(buffer,"Accelerometer respond 0x%2x",accdata);
		  UART_print(buffer);

		  }
		  memset(receiveData,0x00,64);
	  }
	  int16_t acc_val = read_acc_axis(Z_AXIS);


	  sprintf(buffer,"X axis %d\r\n",acc_val);
	  UART_print(buffer);

	  delay(100000);

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

void UART3_RX_TX_IRQHandler()
{
	//GPIO_TogglePinsOutput(GPIOB,1<<21u);

	UART_ClearStatusFlags(UART3,kUART_RxDataRegFullFlag);
	char ch = UART3 ->D;
	SimcomRecBuf[simcom_buf_ptr] = ch;
	simcom_buf_ptr++;

	if(ch == 0x0a && SimcomRecBuf[simcom_buf_ptr-2] == 0x0d)
	{

		carriages++;

	}


}

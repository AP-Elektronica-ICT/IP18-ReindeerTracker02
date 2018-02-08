/**
 * This is template for main module created by New Kinetis SDK 2.x Project Wizard. Enjoy!
 **/

#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_gpio.h"
#include "fsl_uart.h"
#include "fsl_common.h"
#include <stdio.h>

#define RING_BUFFER_SIZE 64
#define RX_DATA_SIZE     64


volatile bool txFinished;
volatile bool rxFinished;
char receiveData[RX_DATA_SIZE];
char ringBuffer[RING_BUFFER_SIZE];

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

void UART_receive()
{

	if(pc_str_rdy)

	{
		strcpy(receiveData,ringBuffer);

		pc_str_rdy =2;
		memset(ringBuffer,0x00,sizeof(ringBuffer));
	}
}

void UART_print(char *data)
{
	char c;
	while ((c=*data++))
	{
		while(!((UART0 ->S1) & 0x80)){}
		UART0 ->D = c;
	}

}


void Simcom_send(char *data)
{
	char c;
	while ((c=*data++))
	{
		while(!((UART3 ->S1) & 0x80)){}
		UART3 ->D = c;
	}
}

void stop_mode()
{

	//__disable_irq();
	SMC ->PMPROT |= 0x20;
	SMC ->PMCTRL |= 0x02;
	uint8_t dummy = SMC ->PMCTRL;

	SCB ->SCR |= 0x04;

	asm("nop");
	asm("nop");
	asm("nop");

	asm("wfi");
}
int main(void) {
  /* Init board hardware. */
  BOARD_InitPins();
  //BOARD_BootClockRUN();
  //BOARD_InitDebugConsole();



  static const gpio_pin_config_t LED_configOutput = {
  kGPIO_DigitalOutput,  /* use as output pin */
  1,  /* initial value */
  };

  GPIO_PinInit(GPIOB, 21u, &LED_configOutput);
  GPIO_PinInit(GPIOB, 9u, &LED_configOutput);

  GPIO_ClearPinsOutput(GPIOB, 1<<21u);


  uart_config_t user_config;

  uint8_t bytesRead;

  UART_GetDefaultConfig(&user_config);
  user_config.baudRate_Bps = 57600U;
  user_config.enableTx = true;
  user_config.enableRx = true;
  UART_Init(UART0,&user_config,20971520U);

  //UART_Init(UART3,&user_config,20971520U);


  UART_EnableInterrupts(UART0,kUART_RxDataRegFullInterruptEnable);
  //UART_EnableInterrupts(UART3,kUART_RxDataRegFullInterruptEnable);

  EnableIRQ(UART0_RX_TX_IRQn);
  //EnableIRQ(UART3_RX_TX_IRQn);*/

  //char buffer[10];

  //uint8_t tmp = SMC ->PMCTRL;

  //sprintf(buffer,"pmctrl: %x \r\n",tmp);
  //UART_print(buffer);

  while(1) { /* Infinite loop to avoid leaving the main function */

	  UART_receive();


	  if(pc_str_rdy == 2)
	  {

		  UART_print(receiveData);


		  memset(receiveData,0x00,64);
		  pc_str_rdy = 0;


	  }


  }
}

void UART0_RX_TX_IRQHandler()
{
	//GPIO_TogglePinsOutput(GPIOB,1<<21u);

	UART_ClearStatusFlags(UART0,kUART_RxDataRegFullFlag);
	UART_ClearStatusFlags(UART0,kUART_RxActiveEdgeFlag);
	char ch = UART0 ->D;
	ringBuffer[buf_ptr] = ch;
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

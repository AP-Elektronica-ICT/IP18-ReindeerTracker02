/*
 * sdcard_io.c
 *
 *  Created on: Feb 9, 2018
 *      Author: pauli
 */


#include "sdcard_io.h"
#include "fsl_port.h"
#include <stdio.h>
#include "fsl_gpio.h"
#include "fsl_dspi.h"

extern void UART_print(char *data);

char buffer[20];

void initSPI()
{

 //use SPI1 module
 /*
  * Pins:
  * PTE 1: MISO
  * PTE 2: SCLK
  * PTE 3: MOSI
  * PTE 4: CS
  * PTE 6: card detect
  *
  * SPI1 use Alt1 mux
  *
  */

	PORT_SetPinMux(PORTE, 1u, kPORT_MuxAlt7);
	PORT_SetPinMux(PORTE, 2u, kPORT_MuxAlt2);
	PORT_SetPinMux(PORTE, 3u, kPORT_MuxAlt7);
	PORT_SetPinMux(PORTE, 4u, kPORT_MuxAlt2);
	PORT_SetPinMux(PORTE, 6u, kPORT_MuxAlt2);

	CLOCK_EnableClock(kCLOCK_Spi1);

	 // Clear all registers

  SPI1->SR = (SPI_SR_TCF_MASK | SPI_SR_EOQF_MASK | SPI_SR_TFUF_MASK | SPI_SR_TFFF_MASK | SPI_SR_RFOF_MASK | SPI_SR_RFDF_MASK); //clear the status bits (write-1-to-clear)

  SPI1->TCR = 0;

  SPI1->RSER = 0;

  SPI1->PUSHR = 0; //Clear out PUSHR register. Since DSPI is halted, nothing should be transmitted

  SPI1->CTAR[0] = 0;

	SPI1 ->MCR |= 0x80000000; //enable master mode
	SPI1 ->MCR &= ~0x00004000; //enable clocks

	SPI1 ->CTAR[0] = 0;
	SPI1 ->CTAR[0] |= 0x38004010; //FMSZ 8 bits
	SPI1 ->CTAR[0] |= 0x7; //Baud rate scaler 128, 250khz

   // configure registers



	/*
	 * Baud rate calculation: FCPU / PBR * ( (1+PBR) / BR )
	 */

}

void SPIsend_command(uint8_t cmdidx, uint8_t crc, uint8_t cspol)
{
	cmdidx |= 0x40;
	SPI1 ->MCR |=  SPI_MCR_HALT_MASK; //Halt SPI

	if(cspol)
	{
		SPI1 ->MCR &= ~0x00010000;
	}
	else
	{
		SPI1 ->MCR |= 0x00010000;
	}


	SPI1 ->MCR |= (SPI_MCR_CLR_RXF_MASK | SPI_MCR_CLR_TXF_MASK); //Flush the fifos

	SPI1 ->SR |= (SPI_SR_TCF_MASK | SPI_SR_EOQF_MASK | SPI_SR_TFUF_MASK | SPI_SR_TFFF_MASK | SPI_SR_RFOF_MASK | SPI_SR_RFDF_MASK); //clear the status bits (write-1-to-clear)

	SPI1 ->TCR |= SPI_TCR_SPI_TCNT_MASK;

	SPI1 ->MCR &=  ~SPI_MCR_HALT_MASK; //Start SPI

	SPI1 ->PUSHR = (0x88010000 | cmdidx); //Assert chip select and push data to transmit register

	while(!(SPI1 ->SR & SPI_SR_TCF_MASK));
	SPI1 ->SR |= (SPI_SR_TCF_MASK | SPI_SR_EOQF_MASK); //clear the status bits (write-1-to-clear)



	SPI1 ->PUSHR = 0x88010000; //Assert chip select and push data to transmit register

	while(!(SPI1 ->SR & SPI_SR_TCF_MASK));
	SPI1 ->SR |= (SPI_SR_TCF_MASK | SPI_SR_EOQF_MASK); //clear the status bits (write-1-to-clear)

	SPI1 ->PUSHR = 0x88010000; //Assert chip select and push data to transmit register

	while(!(SPI1 ->SR & SPI_SR_TCF_MASK));
	SPI1 ->SR |= (SPI_SR_TCF_MASK | SPI_SR_EOQF_MASK); //clear the status bits (write-1-to-clear)

	SPI1 ->PUSHR = 0x88010000; //Assert chip select and push data to transmit register

	while(!(SPI1 ->SR & SPI_SR_TCF_MASK));
	SPI1 ->SR |= (SPI_SR_TCF_MASK | SPI_SR_EOQF_MASK); //clear the status bits (write-1-to-clear)

	SPI1 ->PUSHR = 0x88010000; //Assert chip select and push data to transmit register

	while(!(SPI1 ->SR & SPI_SR_TCF_MASK));
	SPI1 ->SR |= SPI_SR_TCF_MASK;

	SPI1 ->PUSHR = 0x88010000 | crc; //Assert chip select and push data to transmit register

	while(!(SPI1 ->SR & SPI_SR_TCF_MASK));
	SPI1 ->SR |= SPI_SR_TCF_MASK;

}

void SPIsend_command2(uint8_t cmdidx, uint32_t arg, uint8_t crc, uint8_t cspol)
{
	cmdidx |= 0x40;
	//SPI1 ->MCR |=  SPI_MCR_HALT_MASK; //Halt SPI

	dspi_command_data_config_t config;

	//DSPI_GetDefaultDataCommandConfig(&config);

	config.isPcsContinuous = true;
	config.clearTransferCount = true;
	config.isEndOfQueue = false;
	config.whichCtar = 0;
	config.whichPcs = 1;

	DSPI_MasterWriteDataBlocking(SPI1, &config, cmdidx);


	DSPI_MasterWriteDataBlocking(SPI1, &config, (uint16_t)((arg & 0xff000000) >> 24));
	DSPI_MasterWriteDataBlocking(SPI1, &config, (uint16_t)((arg & 0x00ff0000) >> 16));
	DSPI_MasterWriteDataBlocking(SPI1, &config, (uint16_t)((arg & 0x0000ff00) >> 8));
	DSPI_MasterWriteDataBlocking(SPI1, &config, (uint16_t)(arg & 0x000000ff));


	DSPI_MasterWriteDataBlocking(SPI1, &config, crc);

}


uint8_t SPIread()
{
	uint8_t data = 0;

  SPI1->MCR |=  SPI_MCR_HALT_MASK;

  SPI1->MCR |= (SPI_MCR_CLR_RXF_MASK | SPI_MCR_CLR_TXF_MASK); //flush the fifos

  SPI1 ->MCR |= 0x00010000;

  SPI1->SR |= (SPI_SR_TCF_MASK | SPI_SR_EOQF_MASK | SPI_SR_TFUF_MASK | SPI_SR_TFFF_MASK | SPI_SR_RFOF_MASK | SPI_SR_RFDF_MASK); //clear the status bits (write-1-to-clear)

  SPI1->TCR |= SPI_TCR_SPI_TCNT_MASK;

  SPI1->MCR &=  ~SPI_MCR_HALT_MASK;


  SPI1->PUSHR = (SPI_PUSHR_CONT_MASK | SPI_PUSHR_PCS(1) | 0xFF); //dummy byte to read

  while (!(SPI1->SR & SPI_SR_RFDF_MASK)  );

  data = SPI1->POPR; //read

  SPI1->SR = SPI_SR_RFDF_MASK;   // clear the reception flag (not self-clearing)

  return data;
}

void SPIsend(uint8_t data, uint8_t cspol, uint8_t cs)
{

	SPI1 ->MCR |=  SPI_MCR_HALT_MASK; //Halt SPI

	if(cspol)
	{
		SPI1 ->MCR &= ~0x00010000;
	}
	else
	{
		SPI1 ->MCR |= 0x00010000;
	}


	SPI1 ->MCR |= (SPI_MCR_CLR_RXF_MASK | SPI_MCR_CLR_TXF_MASK); //Flush the fifos

	SPI1 ->SR |= (SPI_SR_TCF_MASK | SPI_SR_EOQF_MASK | SPI_SR_TFUF_MASK | SPI_SR_TFFF_MASK | SPI_SR_RFOF_MASK | SPI_SR_RFDF_MASK); //clear the status bits (write-1-to-clear)

	SPI1 ->TCR |= SPI_TCR_SPI_TCNT_MASK;

	SPI1 ->MCR &=  ~SPI_MCR_HALT_MASK; //Start SPI

	SPI1 ->PUSHR = (0x80010000 | ((uint32_t)cs << 31) | (uint32_t)data); //Assert chip select and push data to transmit register

	while(!(SPI1 ->SR & SPI_SR_TCF_MASK));

	SPI1 ->SR |= SPI_SR_TCF_MASK;
}



void cardInit()
{

	/*
	 * To initialize SD card first set CS high and send MOSI '1' for 72 cycles
	 */

	//GPIO_SetPinsOutput(GPIOE,1 << 4u);

	for(uint8_t i = 0;i<10;i++)
	{
		SPIsend(0xff,1,1);
	}




}

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
extern void delay(uint32_t del);

char buffer[80];

void initSPI() {

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

	SPI1->SR = (SPI_SR_TCF_MASK | SPI_SR_EOQF_MASK | SPI_SR_TFUF_MASK
			| SPI_SR_TFFF_MASK | SPI_SR_RFOF_MASK | SPI_SR_RFDF_MASK); //clear the status bits (write-1-to-clear)

	SPI1->TCR = 0;

	SPI1->RSER = 0;

	SPI1->PUSHR = 0; //Clear out PUSHR register. Since DSPI is halted, nothing should be transmitted

	SPI1->CTAR[0] = 0;

	SPI1->MCR |= 0x80000000; //enable master mode
	SPI1->MCR &= ~0x00004000; //enable clocks

	SPI1->CTAR[0] = 0;
	SPI1->CTAR[0] |= 0x38000000; //FMSZ 8 bits
	SPI1->CTAR[0] |= 0x5; //Baud rate scaler 128, 250khz

	SPI1 ->MCR &= ~0x1;

	// configure registers
	/*
	 * Baud rate calculation: FCPU / PBR * ( (1+PBR) / BR )
	 */
}

uint8_t SD_sendCommand(uint8_t cmdidx, uint32_t arg, uint8_t crc) {

	printf("Sending card command: CMD%d, arg: %lx, crc: %x\r\n",cmdidx,arg,crc);
	//UART_print(buffer);

	SPIread();SPIread();

	cmdidx |= 0x40;
dspi_command_data_config_t config;
	config.isPcsContinuous = true;
	config.clearTransferCount = true;
	config.isEndOfQueue = false;
	config.whichCtar = 0;
	config.whichPcs = 1;

	uint8_t addrbuf[4];
	addrbuf[0] = (uint8_t)(arg>>24);
	addrbuf[1] = (uint8_t)(arg>>16);
	addrbuf[2] = (uint8_t)(arg>>8);
	addrbuf[3] = (uint8_t)(arg);

	printf("read addr: %x %x %x %x",addrbuf[0],addrbuf[1],addrbuf[2],addrbuf[3]);

	DSPI_MasterWriteDataBlocking(SPI1, &config, cmdidx);

	DSPI_MasterWriteDataBlocking(SPI1, &config,
			(uint8_t) (arg  >> 24));
	DSPI_MasterWriteDataBlocking(SPI1, &config,
			(uint8_t) (arg >> 16));
	DSPI_MasterWriteDataBlocking(SPI1, &config,
			(uint8_t) (arg >> 8));
	DSPI_MasterWriteDataBlocking(SPI1, &config, (uint8_t) (arg & 0x000000ff));

	DSPI_MasterWriteDataBlocking(SPI1, &config, crc);

	uint8_t resp = 0x80;
	do
	{
		resp = SPIread();
	}

	while(resp & 0x80);


	printf("Got response: %x\r\n",resp);


	return resp;
}

uint8_t SPIread() {

	uint8_t data = 0;
	//printf("SPIread\r\n");

	SPI1->MCR |= 0x00010000;
dspi_command_data_config_t config;
	config.isPcsContinuous = true;
	config.clearTransferCount = true;
	config.isEndOfQueue = false;
	config.whichCtar = 0;
	config.whichPcs = 1;

	DSPI_MasterWriteDataBlocking(SPI1, &config, 0xff);

	while (!(SPI1->SR & SPI_SR_RFDF_MASK))
		;

	data = SPI1->POPR; //read

	SPI1->SR = SPI_SR_RFDF_MASK; // clear the reception flag (not self-clearing)

	return data;
}

void SPIsend(uint8_t data) {

	//printf("SPIsend\r\n");
	SPI1->MCR |= 0x00010000;
dspi_command_data_config_t config;
	config.isPcsContinuous = true;
	config.clearTransferCount = true;
	config.isEndOfQueue = false;
	config.whichCtar = 0;
	config.whichPcs = 1;

	DSPI_MasterWriteDataBlocking(SPI1, &config, data);

}

/*
 * cardInit
 *
 * Do SD card initialization sequence
 *
 */

uint8_t cardInit() {

	/*
	 * To initialize SD card first set CS high and send MOSI '1' for 72 cycles
	 */
dspi_command_data_config_t config;
	config.isPcsContinuous = true;
	config.clearTransferCount = true;
	config.isEndOfQueue = false;
	config.whichCtar = 0;
	config.whichPcs = 1;

	for (uint8_t i = 0; i < 10; i++) {

		SPI1->MCR &= ~0x00010000; //put CS high

		UART_print("debug\r\n");
		DSPI_MasterWriteDataBlocking(SPI1, &config, 0xff);

	}

	uint8_t tmp = 0;

	while (tmp != 0x01) //Send CMD0 (card software reset) until card responds with R1 response, with idle state bit(0x01)
	{
		tmp = SD_sendCommand(0x00, 0, 0x95);
		sprintf(buffer, "read CMD0: %x\r\n", tmp);
		UART_print(buffer);
		delay(300000);
	}

	tmp = 0;

	tmp = SD_sendCommand(0x08, 0x1aa, 0x87); //Next send CMD8
	sprintf(buffer, "read CMD8 1: %x\r\n", tmp);
	UART_print(buffer);

	tmp = SPIread();
	sprintf(buffer, "read CMD8 1: %x\r\n", tmp);
	UART_print(buffer);

	tmp = SPIread();
	sprintf(buffer, "read CMD8 1: %x\r\n", tmp);
	UART_print(buffer);

	tmp = SPIread();
	sprintf(buffer, "read CMD8 1: %x\r\n", tmp);
	UART_print(buffer);

	tmp = SPIread();
	sprintf(buffer, "read CMD8 1: %x\r\n", tmp);
	UART_print(buffer);

	tmp = SPIread();
	sprintf(buffer, "read CMD8 1: %x\r\n", tmp);
	UART_print(buffer);

	tmp = SD_sendCommand(55, 0x00000000, 0xff); //Send CMD55 (only prepares for ACMD41)

	sprintf(buffer, "read CMD55: %x\r\n", tmp);
	UART_print(buffer);

	tmp = 5;

	while(tmp != 0)
	{
		tmp = SD_sendCommand(55, 0x00000000, 0xff); //Send CMD55 (only prepares for ACMD41)

	sprintf(buffer, "read CMD55: %x\r\n", tmp);
	UART_print(buffer);
	tmp = SD_sendCommand(41, 0x40000000, 0xff); //send ACMD41
	sprintf(buffer, "read ACMD41: %x\r\n", tmp);
	UART_print(buffer);
	}

	tmp = SD_sendCommand(58, 0x00000000, 0); //send CMD58

	//SPIread(); //dump the first byte

	unsigned char buff[9];

	tmp = buff[0];

	for (uint8_t k = 0; k < 6; k++) {
		buff[k] = SPIread();
		//delay(2000);
	}

	sprintf(buffer, "read CMD58: %x %x %x %x %x %x\r\n", buff[0], buff[1], buff[2],
			buff[3], buff[4], buff[5]);
	UART_print(buffer);


	while(SD_sendCommand(16,512,0xff) != 0)
	{

	}
	//SD_sendCommand(9,0,0xff);

	//while()

	SPI1 ->MCR |= 0x1;
	SPI1->CTAR[0] &= ~0x5; //Switch to fast baudrate
	SPI1->CTAR[0] |= 0x80000000;
	SPI1 ->MCR &= ~0x1;

	return 0;

}

/*
 * SD_readBlock
 *
 * Read 512 byte block from SD card
 *
 *
 */

uint8_t SD_readBlock(uint32_t addr, uint8_t *buf) {


	//printf("SD_readBlock\r\n");
	uint16_t i;
	uint8_t tmp;
	uint8_t sta;

	//addr = addr << 9;

	//addr /=512;

	do
	{
		sta = SD_sendCommand(17, addr, 0x01); //Send CMD17

		if(sta == 0x40)
		{
			return 4;
		}
	}
	while (sta != 0);

	while (SPIread() != 0xFE) {
		;
	} //wait for token 0xFE

	if (sta == 0) {
		//printf("Reading block\r\n");
		for (i = 0; i < 512; i++)	//read 512 bytes
				{
			buf[i] = SPIread();
			//printf("%x %x\r\n",i,buf[i]);
		}

	tmp = SPIread();
	//printf("read after read block: %x\r\n",tmp);
	tmp = SPIread();
	//printf("read after read block: %x\r\n",tmp);

	return 0;
	}

	else
	{
		return 4;
	}



}

uint8_t SD_readMultipleBlock(uint32_t addr, uint8_t *buf, uint8_t count) {


	UART_print("SD_readMultipleBlock\r\n");
	uint16_t i;
	uint8_t tmp = 0;

	addr = addr << 9;

	SD_sendCommand(18, addr, 0xff);
	SPIread();
	tmp = SPIread();

	if (tmp != 0x00) {
		return 1;
	}

	do {

		while (SPIread() != 0xFE) {
			;
		}

		for (i = 0; i < 512; i++) {
			*buf++ = SPIread();
		}

		SPIread();
		SPIread();

	} while (--count);

	SD_sendCommand(12, 0x00, 0xFF);

	SPIread();	//delay

	return 0;
}

uint8_t SD_writeBlock(uint32_t addr, const uint8_t *buf) {


	UART_print("SD_writeBlock\r\n");

	uint16_t i, retry;
	uint8_t temp;

	//addr = addr << 9; //shift for SDHC type card

	uint8_t tmp;
	tmp = SD_sendCommand(24, addr, 0x01);

	if (tmp != 0x00) {
		return 1;
	}

	//wait SD card ready
	tmp = SPIread();
	//printf("read before send block: %x\r\n",tmp);
	tmp = SPIread();
	//printf("read before send block: %x\r\n",tmp);

	SPIsend(0xFE);

	for (i = 0; i < 512; i++) {
		SPIsend(buf[i]);
	}

	//Dummy CRC

	tmp = SPIread();
	//printf("read after send block: %x\r\n",tmp);
	tmp = SPIread();
	//printf("read after send block: %x\r\n",tmp);

	temp = SPIread();
	//printf("read after send block: %x\r\n",temp);
	temp = SPIread();
	//printf("read after send block: %x\r\n",temp);

	temp &= 0x1F;

	if (temp != 0x05) {

		printf("write not accepted\r\n");
		return 1;
	}

	while (SPIread() == 0x00) {
		retry++;

		if (retry > 0xfffe) {
			return 1;
		}
	}

	SPIread();
	return 0;
}

uint8_t SD_writeMultipleBlock(uint32_t addr, const uint8_t *buf, uint8_t count)

{
	printf("SD_writeMultipleBlock\r\n");
	while(1)
	{;}
	return 0;
}


uint8_t SD_getCID_CSD(uint8_t cmd, uint8_t *buf)
{
	printf("SD_getCID\r\n");

	if(SD_sendCommand(cmd,0,0xff) != 0)
	{
		printf("Fail get CID\r\n");
		return 1;
	}

	uint8_t tmp = 0;
	tmp=SPIread();
	tmp=SPIread();
	for(uint8_t p=0; p<16; p++)
	{
		tmp = SPIread();
		*buf++ = tmp;


		sprintf(buffer, "Read CMD9 %x \r\n",tmp);
		UART_print(buffer);

	}

	return 0;
}

void SD_error(uint8_t res, char *msg)
{
	if(res) {
    	printf("Card fail %s code:%d\r\n",msg,res);

    	while(1)
    	{
    		;
    	}
    }
    else {
    	printf("Card operation ok\r\n");
    }
}



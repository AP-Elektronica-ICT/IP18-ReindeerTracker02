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

dspi_command_data_config_t config;

#define SD_DEBUG_MODE 0

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

	SPI1->MCR |= 0x80000000; //enable master mode
	SPI1->MCR &= ~0x00004000; //enable clocks

	SPI1->CTAR[0] = 0;
	SPI1->CTAR[0] |= 0x38000000; //Set FMSZ (frame size) to 8 bits, so we'll send std bytes
	SPI1->CTAR[0] |= 0x5; //Set Baud Rate Scaler 64 to have 320KHz SCK frequency

	SPI1 ->MCR &= ~0x1; //Start SPI

	//Baud rate calculation: FCPU / PBR * ( (1+PBR) / BR )

	config.isPcsContinuous = true;
	config.clearTransferCount = true;
	config.isEndOfQueue = false;
	config.whichCtar = 0;
	config.whichPcs = 1;
}

/*
 *
 * Send SD card protocol specific command to card
 *
 * param:
 *
 * cmdidx = index of card command
 * arg = 4 bytes of argument data
 * crc = crc byte
 *
 */
uint8_t SD_sendCommand(uint8_t cmdidx, uint32_t arg, uint8_t crc) {

	#if SD_DEBUG_MODE

	printf("Sending card command: CMD%d, arg: %lx, crc: %x\r\n",cmdidx,arg,crc);

	#endif

	SPIread();SPIread(); //Cycle some clocks to bus to prepare SD card (maybe not necessary)

	cmdidx |= 0x40; // OR the 6th bit to cmdidx byte. This is the "transmission" bit which is always 1

	DSPI_MasterWriteDataBlocking(SPI1, &config, cmdidx);

	DSPI_MasterWriteDataBlocking(SPI1, &config, (uint8_t) (arg  >> 24));
	DSPI_MasterWriteDataBlocking(SPI1, &config, (uint8_t) (arg >> 16));
	DSPI_MasterWriteDataBlocking(SPI1, &config, (uint8_t) (arg >> 8));
	DSPI_MasterWriteDataBlocking(SPI1, &config, (uint8_t) (arg & 0x000000ff));

	DSPI_MasterWriteDataBlocking(SPI1, &config, crc);

	uint8_t resp = 0x80;
	do
	{
		resp = SPIread();
	}
	while(resp & 0x80);

	//printf("Got response: %x\r\n",resp);

	return resp;
}

/*
 *
 * SPIread
 *
 * Read data from card. Send byte 0xff, card will transmit it's response at the same time.
 * Then read SPI data register for the card data
 */

uint8_t SPIread() {

	uint8_t data = 0;

	#if SD_DEBUG_MODE
	printf("SPIread\r\n");
	#endif

	SPI1->MCR |= 0x00010000; //Chip Select low during transfer

	DSPI_MasterWriteDataBlocking(SPI1, &config, 0xff);

	while (!(SPI1->SR & SPI_SR_RFDF_MASK))
		;

	data = SPI1->POPR; //read

	SPI1->SR = SPI_SR_RFDF_MASK; // clear the reception flag (not self-clearing)

	return data;
}

/*
 *
 * SPIsend
 *
 * Send simply one byte data to card
 */

void SPIsend(uint8_t data) {

	//printf("SPIsend\r\n");
	SPI1->MCR |= 0x00010000;
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
	 * To initialize SD card first set CS high and send MOSI '1' for 80 cycles
	 */

	uint8_t tmp = 0; //variable for getting card command success status

	for (uint8_t i = 0; i < 10; i++) {

		SPI1->MCR &= ~0x00010000; //put CS high
		DSPI_MasterWriteDataBlocking(SPI1, &config, 0xff);

	}

	while (tmp != 0x01) //Send CMD0 (card software reset) until card responds with R1 response, with idle state bit(0x01)
	{
		tmp = SD_sendCommand(0x00, 0, 0x95);
		printf("read CMD0: %x\r\n", tmp);

	}

	tmp = SD_sendCommand(0x08, 0x1aa, 0x87); //Next send CMD8
	printf("read CMD8 1: %x\r\n", tmp);

	tmp = SPIread(); //read all return bytes of CMD8
	tmp = SPIread();
	tmp = SPIread();
	tmp = SPIread();
	tmp = SPIread();

	tmp = 5;

	while(tmp != 0) //Send CMD55 and ACMD41 in loop until card responds with 0x00
	{
		tmp = SD_sendCommand(55, 0x00000000, 0xff); //Send CMD55 (only prepares for ACMD41)
		printf("read CMD55: %x\r\n", tmp);
		tmp = SD_sendCommand(41, 0x40000000, 0xff); //send ACMD41
		printf("read ACMD41: %x\r\n", tmp);

	}

	tmp = SD_sendCommand(58, 0x00000000, 0); //send CMD58

	unsigned char buff[9];
	tmp = buff[0];

	for (uint8_t k = 0; k < 6; k++) {
		buff[k] = SPIread();
		//delay(2000);
	}

	printf("read CMD58: %x %x %x %x %x %x\r\n", buff[0], buff[1], buff[2],
			buff[3], buff[4], buff[5]);

	while(SD_sendCommand(16,512,0xff) != 0)
	{

	}

	SPI1 ->MCR |= 0x1; //Halt SPI
	SPI1->CTAR[0] &= ~0x5; //Switch to fast baudrate. First clear old setting
	SPI1->CTAR[0] |= 0x80000000; //Write DBR (double baudrate bit)
	SPI1 ->MCR &= ~0x1; //Start SPI again

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

	#if SD_DEBUG_MODE
	printf("SD_readBlock\r\n");
	#endif
	uint8_t tmp;
	uint16_t i;
	//addr = addr << 9;

	do
	{
		tmp = SD_sendCommand(17, addr, 0x01); //Send CMD17

		if(tmp == 0x40)
		{
			return 4;
		}
	}
	while (tmp != 0);

	while (SPIread() != 0xFE){} //wait for token 0xFE

	if (tmp == 0)
	{
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


	printf("SD_readMultipleBlock\r\n");
	uint16_t i;
	uint8_t tmp = 0;

	//addr = addr << 9;

	SD_sendCommand(18, addr, 0xff);
	SPIread();
	tmp = SPIread();

	if (tmp != 0x00) {
		return 1;
	}

	do {

		while (SPIread() != 0xFE) {;}

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

	#if SD_DEBUG_MODE
	printf("SD_writeBlock\r\n");
	#endif

	uint16_t i, retry;
	retry = 0;
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

	//Send 2 dummy bytes as CRC so card is happy
	tmp = SPIread();
	//printf("read after send block: %x\r\n",tmp);
	tmp = SPIread();
	//printf("read after send block: %x\r\n",tmp);

	//Send 2 more dummy bytes to receive data response token
	temp = SPIread();
	//printf("read after send block: %x\r\n",temp);
	temp = SPIread();
	//printf("read after send block: %x\r\n",temp);

	temp &= 0x1F; //Check last 5 bits of data response token

	if (temp != 0x05) { //If response token says fail
		printf("write not accepted\r\n");
		return 1;
	}

	while (SPIread() == 0x00) { //after reading block, poll bus until card pulls data line high and is ready for next command
		retry++;

		if (retry > 0xffffe) {
			printf("write block retry error\r\n");
			return 1;
		}
	}

	SPIread();
	return 0;
}

/*
 *
 * SD_writeMultipleBlock
 *
 * Not yet implemented, if it's needed we'll see as it will get stuck here in the while loop
 */

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
		printf("Read CMD9 %x \r\n",tmp);

	}

	return 0;
}

/*
 *
 * Check SD card operation result res
 *
 * If res >0 then display error and result code and stop program to while loop
 *
 * msg = user defined message for error
 */
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
    	//printf("Card operation ok\r\n");
    }
}



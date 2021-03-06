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

dspi_master_config_t spi_config;
dspi_master_ctar_config_t spi_ctar_config;

extern uint32_t uartClkSrcFreq;

#define SD_DEBUG_MODE 0

/*
 * initSpiConfig
 *
 * This is called just once to fill the config structs for SPI
 */

void initSpiConfig() {

	//use SPI1 module
	/*
	 * Pins:
	 * PTE 1: MISO
	 * PTE 2: SCLK
	 * PTE 3: MOSI
	 * PTE 4: CS
	 * PTE 6: card detect(not used)
	 *
	 * SPI1 use Alt2 mux
	 *
	 */
	PORT_SetPinMux(PORTE, 1u, kPORT_MuxAlt7);
	PORT_SetPinMux(PORTE, 2u, kPORT_MuxAlt2);
	PORT_SetPinMux(PORTE, 3u, kPORT_MuxAlt7);
	PORT_SetPinMux(PORTE, 4u, kPORT_MuxAlt2);

	spi_config.ctarConfig = spi_ctar_config;
	spi_config.enableContinuousSCK = false;
	spi_config.pcsActiveHighOrLow = kDSPI_PcsActiveLow;
	spi_config.whichCtar = 0;
	spi_config.enableModifiedTimingFormat = false;
	spi_config.enableRxFifoOverWrite = false;
	spi_config.samplePoint = 0U;

	spi_ctar_config.baudRate = 250000;
	spi_ctar_config.bitsPerFrame = 8;

	config.isPcsContinuous = false;
	config.clearTransferCount = true;
	config.isEndOfQueue = false;
	config.whichCtar = 0;
	config.whichPcs = 1;

}

/*
 * SD_startComm
 *
 * Wrapper function to start communication with SD card. Set pin mux for SPI and initialize SPI with current settings
 */

void SD_startComm()
{
	PORT_SetPinMux(PORTE, 1u, kPORT_MuxAlt7);
	PORT_SetPinMux(PORTE, 2u, kPORT_MuxAlt2);
	PORT_SetPinMux(PORTE, 3u, kPORT_MuxAlt7);
	PORT_SetPinMux(PORTE, 4u, kPORT_MuxAlt2);
	DSPI_MasterInit(SPI1, &spi_config, uartClkSrcFreq );
}

/*
 * SPI_deactivate_pins
 *
 * Deactivate SPI pins back to disabled state to lower power consumption.
 */

void SPI_deactivate_pins()
{
	PORT_SetPinMux(PORTE, 1u, kPORT_PinDisabledOrAnalog);
	PORT_SetPinMux(PORTE, 2u, kPORT_PinDisabledOrAnalog);
	PORT_SetPinMux(PORTE, 3u, kPORT_PinDisabledOrAnalog);
	PORT_SetPinMux(PORTE, 4u, kPORT_PinDisabledOrAnalog);
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

	DSPI_MasterWriteDataBlocking(SPI1, &config, cmdidx); //write first byte which is the command index

	DSPI_MasterWriteDataBlocking(SPI1, &config, (uint8_t) (arg  >> 24)); //split 32-bit argument word to four 8-bit bytes
	DSPI_MasterWriteDataBlocking(SPI1, &config, (uint8_t) (arg >> 16)); //and send one at a time
	DSPI_MasterWriteDataBlocking(SPI1, &config, (uint8_t) (arg >> 8));
	DSPI_MasterWriteDataBlocking(SPI1, &config, (uint8_t) (arg & 0x000000ff));

	DSPI_MasterWriteDataBlocking(SPI1, &config, crc); //last send the crc byte

	uint8_t resp = 0x80;
	do
	{
		resp = SPIread(); //cycle the clock bus until we receive a byte where first bit is '0'
							//this means the card has received the command
	}
	while(resp & 0x80);


#if SD_DEBUG_MODE
	printf("Got response: %x\r\n",resp);
#endif

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
	DSPI_MasterWriteDataBlocking(SPI1, &config, data);

}

/*
 * cardInit
 *
 * Do SD card initialization sequence
 *
 */

uint8_t cardInit() {

	uint8_t tmp = 0; //variable for getting card command success status

	/*Change to slow baudrate for init*/

	spi_ctar_config.baudRate = 250000; //set 250khz baudrate
	config.isPcsContinuous = true; //keep CS asserted between transfers

	spi_config.ctarConfig = spi_ctar_config; //update spi_config as ctar_config is now changed

	DSPI_MasterInit(SPI1, &spi_config, uartClkSrcFreq ); //Reinit SPI with new settings
	DSPI_StartTransfer(SPI1);

	/*
	 * To initialize SD card first set CS high and send MOSI '1' for 80 cycles
	 */
	for (uint8_t i = 0; i < 10; i++) {

		SPI1->MCR &= ~0x00010000; //put CS high
		DSPI_MasterWriteDataBlocking(SPI1, &config, 0xff);

	}

	SPI1->MCR |= 0x00010000; //put CS active state to low, now CS works as usually(active low)

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

	/* Now switch to full speed on SPI*/

	spi_ctar_config.baudRate = 2000000;
	config.isPcsContinuous = false;
	spi_config.ctarConfig = spi_ctar_config;

	DSPI_MasterInit(SPI1, &spi_config, uartClkSrcFreq );
	DSPI_StartTransfer(SPI1);

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
		tmp = SD_sendCommand(17, addr, 0x01); //Send CMD17 which is the read command

		if(tmp == 0x40)
		{
			return 4;
		}
	}
	while (tmp != 0);

	while (SPIread() != 0xFE){} //wait for card to send 0xFE response token which means card will send the data block

	if (tmp == 0)
	{
		//printf("Reading block\r\n");
		for (i = 0; i < 512; i++)	//read 512 bytes
		{
			buf[i] = SPIread();
			//printf("%x %x\r\n",i,buf[i]);
		}

		//receive 2 CRC bytes, but we do not use them
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



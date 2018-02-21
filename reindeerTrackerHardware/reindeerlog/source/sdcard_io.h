/*
 * sdcard_io.h
 *
 *  Created on: Feb 9, 2018
 *      Author: pauli
 */

#ifndef SOURCE_SDCARD_IO_H_
#define SOURCE_SDCARD_IO_H_

#include <stdint.h>


void initSpiConfig();
void SD_startComm();

uint8_t SD_sendCommand(uint8_t cmdidx, uint32_t arg, uint8_t crc);
void SPIsend(uint8_t data);

uint8_t SD_readBlock(uint32_t addr, uint8_t *buf);
uint8_t SD_readMultipleBlock(uint32_t addr, uint8_t *buf, uint8_t count);

uint8_t SD_writeMultipleBlock(uint32_t addr, const uint8_t *buf, uint8_t count);
uint8_t SD_writeBlock(uint32_t addr, const uint8_t *buf);

uint8_t SPIread();

void SPI_deactivate_pins();
uint8_t cardInit();
uint8_t SD_getCID_CSD(uint8_t cmd,uint8_t *buf);

void SD_error(uint8_t res, char *msg);

#endif /* SOURCE_SDCARD_IO_H_ */

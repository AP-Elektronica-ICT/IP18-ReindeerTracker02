/*
 * sdcard_io.h
 *
 *  Created on: Feb 9, 2018
 *      Author: pauli
 */

#ifndef SOURCE_SDCARD_IO_H_
#define SOURCE_SDCARD_IO_H_

#include <stdint.h>
//#include "diskio.h"

//DSTATUS sd_disk_initialize(uint8_t physicalDrive);

void initSPI();

void SPIsend_command(uint8_t cmdidx, uint8_t crc, uint8_t cspol);

void SPIsend_command2(uint8_t cmdidx, uint32_t arg, uint8_t crc, uint8_t cspol);
void SPIsend(uint8_t data, uint8_t cspol, uint8_t cs);

uint8_t SPIread();

void cardInit();


#endif /* SOURCE_SDCARD_IO_H_ */

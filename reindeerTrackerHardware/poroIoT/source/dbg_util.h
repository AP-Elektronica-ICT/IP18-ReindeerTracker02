/*
 * dbg_util.h
 *
 *  Created on: Apr 16, 2018
 *      Author: pauli
 */

#ifndef SOURCE_DBG_UTIL_H_
#define SOURCE_DBG_UTIL_H_

#include "nbiot_func.h"

uint8_t PCprint(char *data);
uint8_t checkPcInputAndProcess(char* PC_recBuf);
void checkNBDataAndPrint(char* NB_recBuf);
void checkGPSDataAndPrint(char* GPS_recBuf);

void blinkLed(uint16_t time);
void printInterruptFlags();


#endif /* SOURCE_DBG_UTIL_H_ */

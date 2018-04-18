/*
 * adc_func.h
 *
 *  Created on: Feb 9, 2018
 *      Author: teemu
 */

#ifndef SOURCE_ADC_FUNC_H_
#define SOURCE_ADC_FUNC_H_
#include <stdint.h>

void initAdc();
unsigned short ADC_read16b(uint8_t channel_select);
int32_t tempMeas();
uint32_t batteryMeas();


#endif /* SOURCE_ADC_FUNC_H_ */

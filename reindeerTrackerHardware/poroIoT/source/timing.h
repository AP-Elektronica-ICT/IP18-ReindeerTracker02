/*
 * timing.h
 *
 *  Created on: Mar 25, 2018
 *      Author: pauli
 */

#ifndef SOURCE_TIMING_H_
#define SOURCE_TIMING_H_

#include <stdint.h>


uint32_t millis();

void SysTick_Handler();
void rtcInit();
uint32_t rtcGetSeconds();

/*
 * Small delay_ms function
 */
void delay_ms(uint32_t del);


#endif /* SOURCE_TIMING_H_ */

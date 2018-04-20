/*
 * timing.h
 *
 *  Created on: Mar 25, 2018
 *      Author: pauli
 */

#ifndef SOURCE_TIMING_H_
#define SOURCE_TIMING_H_

#include <stdint.h>

#define RTC_REPORT_INTERVAL 40

#define LPTMR_TIMEOUT 8000

uint32_t millis();

void SysTick_Handler();
void rtcInit();
uint32_t rtcGetSeconds();

/*
 * Small delay_ms function
 */
void delay_ms(uint32_t del);

void initTimer();


#endif /* SOURCE_TIMING_H_ */

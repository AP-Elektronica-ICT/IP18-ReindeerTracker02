/*
 * timing.h
 *
 *  Created on: Mar 25, 2018
 *      Author: pauli
 */

#ifndef SOURCE_TIMING_H_
#define SOURCE_TIMING_H_

#include <stdint.h>

volatile uint32_t ticks = 0;

inline uint32_t millis()
{
	return ticks;
}

void SysTick_Handler()
{

	ticks++;
}


/*
 * Small delay_ms function
 */
void delay_ms(uint32_t del)
{

	uint32_t otime = millis()+del;

	while(millis() < otime)
	{

	}
}




#endif /* SOURCE_TIMING_H_ */

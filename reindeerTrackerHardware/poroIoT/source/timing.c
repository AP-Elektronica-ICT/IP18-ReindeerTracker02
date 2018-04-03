/*
 * timing.c
 *
 *  Created on: Mar 27, 2018
 *      Author: pauli
 */

#include "timing.h"

volatile uint32_t ticks = 0;

uint32_t millis()
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

/*
 * timing.c
 *
 *  Created on: Mar 27, 2018
 *      Author: pauli
 */

#include "timing.h"
#include "fsl_rtc.h"

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

void rtcInit() {

	rtc_config_t rtc_config;
	RTC_GetDefaultConfig(&rtc_config);
	//SIM -> SOPT1 |= 0xC0000;

	CLOCK_EnableClock(kCLOCK_Rtc0);
	RTC_Reset(RTC);
	RTC_Init(RTC, &rtc_config);

	RTC_SetOscCapLoad(RTC, kRTC_Capacitor_8p);
	RTC_SetClockSource(RTC);

	RTC ->TAR = 0xa;

	RTC_EnableInterrupts(RTC, kRTC_AlarmInterruptEnable);
	EnableIRQ(RTC_IRQn);

	RTC_StartTimer(RTC);

}

uint32_t rtcGetSeconds() {

	return RTC -> TSR;
}

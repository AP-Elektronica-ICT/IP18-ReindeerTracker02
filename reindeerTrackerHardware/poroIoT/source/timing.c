/*
 * timing.c
 *
 *  Created on: Mar 27, 2018
 *      Author: pauli
 */

#include "timing.h"
#include "fsl_rtc.h"
#include "fsl_lptmr.h"

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

	DisableIRQ(RTC_IRQn);

	CLOCK_EnableClock(kCLOCK_Rtc0);

	RTC_Reset(RTC);
	RTC_Init(RTC, &rtc_config);

	RTC_SetOscCapLoad(RTC, kRTC_Capacitor_4p);
	RTC_SetClockSource(RTC);

	RTC ->TAR = RTC_REPORT_INTERVAL;

	RTC ->TSR = 0;

	RTC ->IER = 0x04;

	RTC_StartTimer(RTC);

	EnableIRQ(RTC_IRQn);
}

uint32_t rtcGetSeconds() {

	return RTC -> TSR;
}

void initTimer()
{

	lptmr_config_t lptmr_config;

	/*
	 * Init dead reindeer timer. LPTIMER interrupt will wake up MCU after a certain time, IF accelerometer interrupt
	 * has not waked it earlier (and reset the timer)
	 */
	LPTMR_GetDefaultConfig(&lptmr_config);
	lptmr_config.bypassPrescaler = true;
	lptmr_config.value = kLPTMR_Prescale_Glitch_0;
	lptmr_config.prescalerClockSource = kLPTMR_PrescalerClock_1;
	LPTMR_Init(LPTMR0, &lptmr_config);

	LPTMR_SetTimerPeriod(LPTMR0, LPTMR_TIMEOUT);  //set dead reindeer timeout period
	NVIC_SetPriority(LPTMR0_IRQn,1);
	EnableIRQ(LPTMR0_IRQn);

}

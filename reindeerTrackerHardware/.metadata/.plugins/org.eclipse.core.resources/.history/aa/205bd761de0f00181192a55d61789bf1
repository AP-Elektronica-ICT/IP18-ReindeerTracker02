/*
 * adc_func.c
 *
 *  Created on: Feb 9, 2018
 *      Author: teemu
 */
#include "fsl_port.h"
extern void UART_print(char *data);

void initAdc(){

	//ADC0_DP1, ADC0_DM1, ADC1_DP
	//Write ADC0_C1 select channel AD4
		CLOCK_EnableClock(kCLOCK_Adc0);
		CLOCK_EnableClock(kCLOCK_PortC);
		ADC0 -> CFG1 |= 0x0000000C; /*16bits ADC*/
		ADC0 -> SC1[0] |= 0x1F; //Choose AD4 channel 00100
}
unsigned short ADC_read16b(void)
{
ADC0 -> SC1[0] |= 14 ; //Write to SC1A to start conversion
while(ADC0 -> SC2 & ADC_SC2_ADACT_MASK); //Conversion in progress

while(!(ADC0 -> SC1[0] & ADC_SC1_COCO_MASK)); //Wait until conversion complete
UART_print("kfeok");
return ADC0 -> R[0];
}


/*
 * adc_func.c
 *
 *  Created on: Feb 9, 2018
 *      Author: teemu
 */
#include "fsl_port.h"
#include "fsl_adc16.h"
extern void UART_print(char *data);

void initAdc(){



	//ADC0_DP1, ADC0_DM1, ADC1_DP
	//Write ADC0_C1 select channel AD4

	// PTC0 (X-axis), PTC1 (Y-axis), PTB2 (Z-axis), PTB3 (temp) ADC PIN

		CLOCK_EnableClock(kCLOCK_Adc0);
		CLOCK_EnableClock(kCLOCK_PortC);
		CLOCK_EnableClock(kCLOCK_PortB);

		adc16_config_t adc_config;

		ADC16_DoAutoCalibration(ADC0);

		adc_config.referenceVoltageSource = kADC16_ReferenceVoltageSourceVref;
		adc_config.clockSource = kADC16_ClockSourceAsynchronousClock;
        adc_config.enableAsynchronousClock = true;
	    adc_config.clockDivider = kADC16_ClockDivider8;
	    adc_config.resolution = kADC16_ResolutionSE16Bit;
	    adc_config.longSampleMode = kADC16_LongSampleDisabled;
	    adc_config.enableHighSpeed = false;
	    adc_config.enableLowPower = false;
	    adc_config.enableContinuousConversion = false;

	    ADC16_Init(ADC0, &adc_config);

	    //ADC0 -> SC1[0] |= 0x1F; //Choose AD4 channel 00100

}
unsigned short ADC_read16b(uint8_t channel_select) {

	adc16_channel_config_t channel_config;

	channel_config.enableDifferentialConversion = false;
	channel_config.enableInterruptOnConversionCompleted = false;

	switch ( channel_select ) {

	case 1:
		channel_config.channelNumber = 14;
		break;
	case 2:
		channel_config.channelNumber = 15;
		break;
	case 3:
		channel_config.channelNumber = 12;
		break;
	case 4:
		channel_config.channelNumber = 13;
		break;
	default:
		UART_print("No channel selected");
		return 0;
	}

	ADC16_SetChannelMuxMode(ADC0, kADC16_ChannelMuxA);
	ADC16_SetChannelConfig(ADC0, 0, &channel_config);

	ADC0 -> SC1[0] |= channel_config.channelNumber ; //Write to SC1A to start conversion

	while(ADC0 -> SC2 & ADC_SC2_ADACT_MASK); //Conversion in progress
		//UART_print("Conversion in progress: ");
		while(!(ADC0 -> SC1[0] & ADC_SC1_COCO_MASK)); //Wait until conversion complete
			//UART_print("Converted: ");

			return ADC0 -> R[0];

}


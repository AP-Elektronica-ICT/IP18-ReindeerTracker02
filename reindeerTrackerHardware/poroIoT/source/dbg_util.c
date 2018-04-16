/*
 * dbg_util.c
 *
 *  Created on: Apr 16, 2018
 *      Author: pauli
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "dbg_util.h"
#include "fsl_gpio.h"
#include "fsl_lpuart.h"

#include "nbiot_func.h"
#include "gps_func.h"
#include "timing.h"
#include "at_func.h"

extern volatile uint8_t PC_strReady;
extern volatile uint8_t GPS_strReady;
extern volatile uint8_t NB_strReady;
extern uint8_t streamGps;

extern volatile uint16_t NB_bufPtr, PC_bufPtr, GPS_bufPtr;

uint8_t PCprint(char *data)
{
	char c;
	uint8_t len = 0;
	while ((c = *data++))
	{
		while (!(LPUART1->STAT & kLPUART_TxDataRegEmptyFlag));

		LPUART1->DATA = c;
		len++;
	}
	return len;
}

uint8_t checkPcInputAndProcess(char* PC_recBuf)

{

	if (PC_strReady)
	{

		if (strstr(PC_recBuf, "iot") != NULL)
		{
			PCprint("Starting Reindeer IoT cycle\r\n");
			return 1;
		}
		else if (strstr(PC_recBuf, "gpsinfo=1") != NULL)
		{
			streamGps = 1;
			GPIO_ClearPinsOutput(GPIOA, 1 << 19u);
		}
		else if (strstr(PC_recBuf, "gpsinfo=0") != NULL)
		{
			streamGps = 0;
			GPIO_SetPinsOutput(GPIOA, 1 << 19u);
		}
		else if (strstr(PC_recBuf, "rfoff") != NULL)
		{
			GPIO_ClearPinsOutput(GPIOA, 1 << 1u); //Power off RF modules
		}
		else if (strstr(PC_recBuf, "rfon") != NULL)
		{
			GPIO_SetPinsOutput(GPIOA, 1 << 1u); //Power on RF modules
		}
		else if (strstr(PC_recBuf, "\xb5\x62") != NULL) //if input is UBX command!
		{
			PCprint("send to gps\r\n");
			uint8_t ubxMsgLen = calcUbxCrc(PC_recBuf + 2); //Calculate UBX checksum and add it to the message
			GPS_send(PC_recBuf, ubxMsgLen); //Send UBX message to module
		}
		else
		{

			NB_send(PC_recBuf);

		}
		memset(PC_recBuf, 0, strlen(PC_recBuf));
		PC_strReady = 0;
		PC_bufPtr = 0;
	}

	return 0;

}


void checkNBDataAndPrint(char* NB_recBuf)
{
	if (NB_strReady)
	{
		uint32_t moduleResponseTimeout = millis() + RESPONSE_TIMEOUT_NORMAL_VALUE; //reset timeout to initial value

		while (millis() < moduleResponseTimeout)

		{

			if (breakIfAtOk())
			{
				break;
			}

		}

		//now the timeout has expired since last character had arrived, so we can process data

		PCprint(NB_recBuf);
		PCprint("\r\n");
		memset(NB_recBuf, 0, 500);
		NB_bufPtr = 0;
		NB_strReady = 0;
	}
}


void checkGPSDataAndPrint(char* GPS_recBuf)
{
	/*
	 * If GPS string is ready and GPS data streaming is enabled, enter here to process GPS data buffer
	 */
	if (GPS_strReady && streamGps)
	{

		PCprint(GPS_recBuf);
		PCprint("\r\n"); //First print out whole buffer

		if (getGPS())
		{
			//char testLat[12] = ("6500.02359");
			//char testLon[12] = ("02530.56951");
			//parseData(testLat,testLon);

			char buffer[100];

			sprintf(buffer,"Parsed lat: %s Parsed lon: %s\r\n", parsedLat, parsedLon);
			PCprint(buffer);

		}


		char* ubxResponseStartPtr = strstr(GPS_recBuf, "\xb5\x62"); //Find pointer to UBX header. If there is no UBX response, pointer
																	//will be NULL
		if (ubxResponseStartPtr != NULL) //If pointer is not null, it means UBX response header is found
		{
			PCprint("Found UBX response\r\n");

			uint8_t responseLength = *(ubxResponseStartPtr + 4); //Find out UBX response length, it is always the 5th byte
																 //from beginning of the packet.
			printUbxResponseHex(ubxResponseStartPtr, responseLength + 6 + 2); //Print UBX response message. Function wants to know
			//how many chars to print. We must add 6+2 to print header and crc too
		}

		memset(GPS_recBuf, 0, 600);
		GPS_bufPtr = 0;
		GPS_strReady = 0;
	}
}

void blinkLed(uint16_t time)
{
	GPIO_ClearPinsOutput(GPIOA, 1 << 4u);
	delay_ms(time);
	GPIO_SetPinsOutput(GPIOA, 1 << 4u);
	delay_ms(100);
}

void printInterruptFlags()
{
	char buf[100];
	CLOCK_EnableClock(kCLOCK_Lptmr0);
	CLOCK_EnableClock(kCLOCK_Rtc0);
	sprintf(buf, "rtc flags: %lx, lptmr flags: %lx, llwu pin flags: %x\r\n",
	RTC->SR, LPTMR0->CSR, LLWU->F1);
	PCprint(buf);
	sprintf(buf, "rtc seconds: %ld\r\n", RTC->TSR);
	PCprint(buf);

	sprintf(buf, "uart2 flags: %x, lpuart1 flags: %lx, lpuart0 flags: %lx\r\n",
	UART2->S1, LPUART1->STAT, LPUART0->STAT);
	PCprint(buf);
}

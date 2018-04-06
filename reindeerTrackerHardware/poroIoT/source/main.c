/**
 * This is template for main module created by New Kinetis SDK 2.x Project Wizard. Enjoy!
 **/

#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"

#include "fsl_gpio.h"
#include "fsl_uart.h"
#include "fsl_port.h"
#include "fsl_common.h"
#include "fsl_i2c.h"
#include "fsl_smc.h"
#include "fsl_lptmr.h"

#include "at_func.h"
#include "fsl_lpuart.h"
#include "fsl_llwu.h"
#include "fsl_clock.h"

#include <stdio.h>
#include <stdlib.h>

#include "acc_func.h"
#include "i2c_func.h"
#include "adc_func.h"
#include "fsl_rtc.h"
#include "gps_func.h"
#include "ubx_func.h"
#include "nbiot_func.h"
#include "timing.h"

#define RESPONSE_TIMEOUT_NORMAL_VALUE 2000

lptmr_config_t lptmr_config;
smc_power_mode_vlls_config_t smc_power_mode_vlls_config;
uart_config_t uart_config;

volatile uint8_t wake = 3;
volatile uint8_t NB_strReady = 0;
volatile uint16_t NB_bufPtr = 0;

char NB_recBuf[500]; 	//buffer for receiving NB IoT module data

static char PC_recBuf[500];	//buffer for receiving from PC terminal
volatile uint16_t PC_bufPtr = 0;
volatile uint8_t PC_strReady = 0;

char GPS_recBuf[600];	//buffer for receiving from PC terminal
volatile uint16_t GPS_bufPtr = 0;
volatile uint8_t GPS_strReady = 0;
uint8_t streamGps = 0;

char parsedLat[15];
char parsedLon[15];

volatile uint32_t moduleResponseTimeout = RESPONSE_TIMEOUT_NORMAL_VALUE; //timeout variable for waiting all data from module

void initTimer() {

	/*
	 * Init dead reindeer timer. LPTIMER interrupt will wake up MCU after a certain time, IF accelerometer interrupt
	 * has not waked it earlier (and reset the timer)
	 */
	NVIC_ClearPendingIRQ(LPTMR0_IRQn);
	LPTMR_GetDefaultConfig(&lptmr_config);
	lptmr_config.bypassPrescaler = true;
	lptmr_config.value = kLPTMR_Prescale_Glitch_0;
	lptmr_config.prescalerClockSource = kLPTMR_PrescalerClock_1;
	LPTMR_Init(LPTMR0, &lptmr_config);
	LPTMR_SetTimerPeriod(LPTMR0, 7000);  // Period in ms

	EnableIRQ(LPTMR0_IRQn);
}

/*
 *
 * Init all needed UART buses. LPUART0 for NB-IoT, UART0 for PC, UART2 for GPS
 */

void initUART() {

	lpuart_config_t lpuart_config;

	SystemCoreClockUpdate();
	uint32_t uartClkSrcFreq = CLOCK_GetFreq(kCLOCK_BusClk); //get MCU clock frequency for setting correct baudrate
	uint32_t lpuartClkSrcFreq = CLOCK_GetFreq(BOARD_DEBUG_UART_CLKSRC);

	UART_GetDefaultConfig(&uart_config);
	LPUART_GetDefaultConfig(&lpuart_config);
	uart_config.baudRate_Bps = 9600;
	uart_config.enableTx = true;
	uart_config.enableRx = true;

	lpuart_config.baudRate_Bps = 9600;
	lpuart_config.enableTx = true;
	lpuart_config.enableRx = true;

	CLOCK_SetLpuart1Clock(0x3U);
	CLOCK_SetLpuart0Clock(0x3U);

	LPUART_Init(LPUART0, &lpuart_config, lpuartClkSrcFreq); //Init LPUART0 for NBiot

	UART_Init(UART2, &uart_config, uartClkSrcFreq); //UART2 for GPS with same settings!

	lpuart_config.baudRate_Bps = 9600;

	LPUART_Init(LPUART1, &lpuart_config, lpuartClkSrcFreq); //Init LPUART1 for PCuart

	LPUART_EnableInterrupts(LPUART0, kLPUART_RxDataRegFullInterruptEnable); //enable LPUART0 receive interrupt to receive data from NBiot

	EnableIRQ(LPUART0_IRQn);

	UART_EnableInterrupts(UART2, kUART_RxDataRegFullInterruptEnable); //enable LPUART0 receive interrupt to receive data from GPS
	EnableIRQ(UART2_FLEXIO_IRQn);

	LPUART_EnableInterrupts(LPUART1, kLPUART_RxDataRegFullInterruptEnable); //enable UART0 receive interrupt to receive data from PC
	EnableIRQ(LPUART1_IRQn);

}

/*
 * Send data to NBiot with LPUART0
 * String to be sent is pointed by *data
 *
 */

void NB_send(char *data) {

	char c = *data++; //assign c a character from the string and post-increment string pointer

	while (c) { //loop until c is zero which means string has ended and no more chars has to be sent

		while (!((LPUART0->STAT) & kLPUART_TxDataRegEmptyFlag)) {
		} //wait until LPUART0 Transmission Complete flag rises, so we can send new char
		LPUART0->DATA = c; //write new character to transmit buffer
		c = *data++; //assign next character to c and post-increment string pointer
	}
}

void GPS_send(char *data, uint8_t len) {

	char c = *data++; //assign c a character from the string and post-increment string pointer

	for (; len > 0; len--) { //loop until c is zero which means string has ended and no more chars has to be sent

		while (!((UART2->S1) & 0x80)) {
		} //wait until LPUART0 Transmission Complete flag rises, so we can send new char
		UART2->D = c; //write new character to transmit buffer
		c = *data++; //assign next character to c and post-increment string pointer
	}
}

uint8_t PCprint(char *data) {
	char c;
	uint8_t len = 0;
	while ((c = *data++)) {

		while (!(LPUART1->STAT & kLPUART_TxDataRegEmptyFlag)) {
		}

		LPUART1->DATA = c;
		len++;

	}

	return len;
}

int main(void) {

	PMC->REGSC |= 0x08;	//acknowledge wake up to voltage regulator module, this is needed with LLWU wake up

	NVIC_EnableIRQ(LLWU_IRQn);//enable LLWU interrupts. if we wake up from VLLS mode, it means that next MCU
							  //will jump to the LLWU interrupt vector
	struct reindeerData_t reindeerData; //create struct for our reindeer data that will be sent
	char mqttMessage[450];

	BOARD_InitPins();	//init all physical pins
	BOARD_BootClockVLPR(); //by uncommenting this we can use FRDM 50Mhz external clock, but will not work with modified board

	SysTick_Config(BOARD_DEBUG_UART_CLK_FREQ / 1000); //setup SysTick timer for 1ms interval for delay functions(see timing.h)

	/*initI2C();
	 configure_acc();
	 acc_init();*/
	//initAdc();
	initUART();

	static const gpio_pin_config_t LED_configOutput = { kGPIO_DigitalOutput, /* use as output pin */
	1, /* initial value */
	};
	GPIO_PinInit(GPIOA, 4u, &LED_configOutput);	//blue led as output

	char buf[30];

	CLOCK_EnableClock(kCLOCK_Lptmr0);
	//sprintf(buf, "lptimer int flag: %lx\r\n", LPTMR0->CSR);
	sprintf(buf, "LLWU int flag: %lx\r\n", LLWU->F1);
	PCprint(buf);

	initTimer();

	SMC_SetPowerModeProtection(SMC, kSMC_AllowPowerModeAll);
	smc_power_mode_vlls_config.subMode = kSMC_StopSub1; //!< Stop submode 1, for VLLS1/LLS1.

	LLWU->ME |= 0x01; 		// enable LLWU wakeup source from LPTMR module
	LLWU->PE2 |= 0x04; // enable LLWU wakeup source from accelerometer interrupt pin

	// 0x20 for stock frdm pin enable,
	LLWU->FILT1 |= 0x25;	// set pin wakeup from rising edge, 0x2A for frdm

	//EnableIRQ(PORTC_IRQn);

	LPTMR_EnableInterrupts(LPTMR0, kLPTMR_TimerInterruptEnable);//Sets Timer Interrupt Enable bit to 1
	LPTMR_StartTimer(LPTMR0);

	GPIO_PinInit(GPIOA, 19u, &LED_configOutput);

	//GPIO_ClearPinsOutput(GPIOA, 1 << 4u);

	/*
	 * set boost regulator enable pin as output. This pin will control the power to RF modules
	 */
	GPIO_PinInit(GPIOA, 1u, &LED_configOutput);

	GPIO_ClearPinsOutput(GPIOA, 1 << 1u); //Power on RF modules

	PCprint(
			"Reindeer IoT has started\r\nCommand \"iot\" to start executing reindeer track cycle\r\n"
					"Command \"gpsinfo=1\" or \"gpsinfo=0\" to switch GPS data on/off\r\n");
	PCprint(
			"Or enter normal AT commands here for SARA-N2\r\nModules powered on and booting now!\r\n");

	/*
	 * Copy all reindeer variables to struct before starting network operations
	 */

	char testLat[11] = ("6500.53");
	char testLon[11] = ("02534.554");
	strcpy(reindeerData.serialNum, "11111");
	strcpy(reindeerData.latitude, testLat);
	strcpy(reindeerData.longitude, testLon);
	strcpy(reindeerData.dead, "false");
	reindeerData.batteryLevel = 45;

	if (wake == 2) {

		PCprint("Woken by ACCEL, reindeer is !!!ALIVE!!!\r\n");
		SMC_PreEnterStopModes();
		SMC_SetPowerModeVlls(SMC, &smc_power_mode_vlls_config);
	}

	else if (wake == 0) {
		PCprint("wake was 0 going to sleep\r\n");
		SMC_PreEnterStopModes();
		SMC_SetPowerModeVlls(SMC, &smc_power_mode_vlls_config);
	}

	while (1) {

		/*
		 * Check if a string has arrived from PC (with CR line end)
		 */

		if (wake == 1) {

			strcpy(reindeerData.dead, "true");
			PCprint("Woken by LPTMR, reindeer is !!!%s!!\r\n",reindeerData.dead);

			while (true) {
				if (!GPS_strReady) {
					PCprint(GPS_recBuf);
					PCprint("\r\n"); //First print out whole buffer

					char testLat[12] = ("6500.02359");
					char testLon[12] = ("02530.56951");

					strcpy(reindeerData.latitude, testLat);
					strcpy(reindeerData.longitude, testLon);
					break;

					if (getGPS()) {

						parseData(testLat,testLon);

						strcpy(reindeerData.latitude, parsedLat);
						strcpy(reindeerData.longitude, parsedLon);
						break;
					}
					memset(GPS_recBuf, 0, 600);
					GPS_bufPtr = 0;
					GPS_strReady = 0;
				}

			}

			PCprint("Parsed latitude: %s\r\n", reindeerData.latitude);
			PCprint("Parsed longitude: %s\r\n", reindeerData.longitude);
			break;
		}

		if (PC_strReady) {

			if (strstr(PC_recBuf, "iot") != NULL) {
				PCprint("Starting Reindeer IoT cycle\r\n");
				break;
			} else if (strstr(PC_recBuf, "gpsinfo=1") != NULL) {
				streamGps = 1;
				GPIO_ClearPinsOutput(GPIOA, 1 << 19u);
			} else if (strstr(PC_recBuf, "gpsinfo=0") != NULL) {
				streamGps = 0;
				GPIO_SetPinsOutput(GPIOA, 1 << 19u);
			} else if (strstr(PC_recBuf, "rfoff") != NULL) {
				GPIO_ClearPinsOutput(GPIOA, 1 << 1u); //Power off RF modules
			} else if (strstr(PC_recBuf, "rfon") != NULL) {
				GPIO_SetPinsOutput(GPIOA, 1 << 1u); //Power on RF modules
			} else if (strstr(PC_recBuf, "\xb5\x62") != NULL) //if input is UBX command!
			{
				PCprint("send to gps\r\n");
				uint8_t ubxMsgLen = calcUbxCrc(PC_recBuf + 2); //Calculate UBX checksum and add it to the message
				GPS_send(PC_recBuf, ubxMsgLen); //Send UBX message to module
			} else {

				NB_send(PC_recBuf);

			}
			memset(PC_recBuf, 0, strlen(PC_recBuf));
			PC_strReady = 0;
			PC_bufPtr = 0;
		}

		if (NB_strReady) {
			moduleResponseTimeout = millis() + RESPONSE_TIMEOUT_NORMAL_VALUE; //reset timeout to initial value

			while (millis() < moduleResponseTimeout)

			{

				if (breakIfAtOk()) {
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

		/*
		 * If GPS string is ready and GPS data streaming is enabled, enter here to process GPS data buffer
		 */
		if (GPS_strReady && streamGps) {

			PCprint(GPS_recBuf);
			PCprint("\r\n"); //First print out whole buffer

			if (getGPS()) {
				//char testLat[12] = ("6500.02359");
				//char testLon[12] = ("02530.56951");

				//parseData(testLat,testLon);

				strcpy(reindeerData.latitude, parsedLat);
				strcpy(reindeerData.longitude, parsedLon);
				break;
			}

			//PCprint("Parsed latitude: %s\r\n", reindeerData.latitude);
			//PCprint("Parsed longitude: %s\r\n", reindeerData.longitude);

			char* ubxResponseStartPtr = strstr(GPS_recBuf, "\xb5\x62"); //Find pointer to UBX header. If there is no UBX response, pointer
																		//will be NULL

			if (ubxResponseStartPtr != NULL) //If pointer is not null, it means UBX response header is found
			{
				PCprint("Found UBX response\r\n");

				uint8_t responseLength = *(ubxResponseStartPtr + 4); //Find out UBX response length, it is always the 5th byte
																	 //from beginning of the packet. * means dereferencing pointer
				//dereferencing means "accessing the value where pointer points"

				//PCprint("UBX response length: %02x\r\n", responseLength);

				printUbxResponseHex(ubxResponseStartPtr,
						responseLength + 6 + 2); //Print UBX response message. Function wants to know
				//how many chars to print. We must add 6+2 to print header and crc too
			}

			memset(GPS_recBuf, 0, 600);
			GPS_bufPtr = 0;
			GPS_strReady = 0;
		}

	}

	/*
	 * Assemble data to json format and then to POST message
	 */

	uint8_t msgLen = assembleMqtt(&reindeerData, mqttMessage);

	//NB_send_msg(mqttMessage, msgLen);

	NB_create_pdp_send(mqttMessage, msgLen);
	PCprint("Roger include main.c\r\n");

	AT_send("CFUN=0", "", "OK");

	GPIO_ClearPinsOutput(GPIOA, 1 << 1u); //Power on RF modules

	SMC_PreEnterStopModes();
	SMC_SetPowerModeVlls(SMC, &smc_power_mode_vlls_config);

	while (1) {

	}
	//parseData(testLat, testLon);
}

void LLWU_IRQHandler() {

	GPIO_PortToggle(GPIOA, 1 << 4u);
	GPIO_PortToggle(GPIOA, 1 << 4u);
	wake = 1;

	/* If wakeup by LPTMR. */
	if (LLWU_GetInternalWakeupModuleFlag(LLWU, 0U)) {
		LPTMR_DisableInterrupts(LPTMR0, kLPTMR_TimerInterruptEnable);
		LPTMR_ClearStatusFlags(LPTMR0, kLPTMR_TimerCompareFlag);
		LPTMR_StopTimer(LPTMR0);
		wake = 1;
	}

	else if ( LLWU->F2 & 0x20) {// 0x04 for stock frdm acc wakeup reg, 0x01 for customized
		wake = 2;
		LLWU->F2 |= 0x20;
	}

	LLWU->F2 = 0x20;
}

void LPUART0_IRQHandler() {

	LPUART_ClearStatusFlags(LPUART0, kUART_RxDataRegFullFlag);
	GPIO_PortToggle(GPIOB, 1 << 22u); //toggle RED led to indicate data arrived from NB Iiootee module

	uint8_t uartData = LPUART0->DATA;
	if (uartData != 0) {

		NB_recBuf[NB_bufPtr] = uartData;
		NB_bufPtr++;

		if (uartData == 0x0d) {
			NB_strReady = 1;
			//NB_bufPtr = 0;
		}

	}

}

void UART2_FLEXIO_IRQHandler() {

	UART_ClearStatusFlags(UART2, kUART_RxDataRegFullFlag);

	uint8_t uartData = UART2->D;

	/*
	 * Here we use different method for collecting GPS data. because there can be other data than characters,
	 * like 0x00 in UBX messages, normal string functions would fail (mistaken null terminator)
	 * so we must collect every byte from the gps module
	 * so fill buffer to almost full with GPS data, then put GPS_strReady high and stop filling.
	 * Start filling again when data has been read and GPS_strReady is low.
	 *
	 */
	if (GPS_strReady == 0) {
		GPS_recBuf[GPS_bufPtr] = uartData; //put new byte to buffer
		GPS_bufPtr++;
	}

	/*
	 * When buffer is almost full, put strReady high and stop filling it
	 */
	if (GPS_bufPtr > 499) {
		GPS_strReady = 1;
	}

}

void LPUART1_IRQHandler() {

	uint8_t uartData = LPUART1->DATA;
	//GPIO_PortToggle(GPIOA, 1 << 4u);

	PC_recBuf[PC_bufPtr] = uartData;
	PC_bufPtr++;

	if (uartData == 0x0d) {
		PC_strReady = 1;
		PC_bufPtr = 0;

	}
}

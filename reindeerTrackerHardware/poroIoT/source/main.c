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
//#include "fsl_dspi.h"
#include "at_func.h"
#include "fsl_lpuart.h"

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


volatile uint8_t wake = 2;
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

uint32_t ms_ticks; //millisecond ticks value for the delay_ms function
/*
 void delay_ms(uint32_t del)
 {
 for (; del > 0; del--)
 {
 for(uint32_t t = 0; t<ms_ticks;t++)
 {
 __asm("nop");
 }
 }
 }
 */

void initTimer() {

	/*
	 * Init dead reindeer timer. LPTIMER interrupt will wake up MCU after a certain time, IF accelerometer interrupt
	 * has not waked it earlier (and reset the timer)
	 */

	LPTMR_GetDefaultConfig(&lptmr_config);
	lptmr_config.bypassPrescaler = true;
	lptmr_config.value = kLPTMR_Prescale_Glitch_0;
	lptmr_config.prescalerClockSource = kLPTMR_PrescalerClock_1;
	EnableIRQ(LPTMR0_IRQn);
	LPTMR_Init(LPTMR0, &lptmr_config);
	LPTMR_SetTimerPeriod(LPTMR0, 7000);  // 3000 for 20hz data rat
}

/*
 *
 * Init all needed UART buses. LPUART0 for NB-IoT, UART0 for PC, UART2 for GPS
 */

void initUART() {

	lpuart_config_t lpuart_config;
	uint32_t uartClkSrcFreq = BOARD_DEBUG_UART_CLK_FREQ; //get MCU clock frequency for setting correct baudrate

	UART_GetDefaultConfig(&uart_config);
	uart_config.baudRate_Bps = 9600;
	uart_config.enableTx = true;
	uart_config.enableRx = true;
	lpuart_config.baudRate_Bps = 57600;
	lpuart_config.enableTx = true;
	lpuart_config.enableRx = true;

	CLOCK_SetLpuart1Clock(0x1U);
	CLOCK_SetLpuart0Clock(0x1U);

	LPUART_Init(LPUART0, &lpuart_config, uartClkSrcFreq); //Init LPUART0 for NBiot

	UART_Init(UART2, &uart_config, uartClkSrcFreq); //UART2 for GPS with same settings!

	lpuart_config.baudRate_Bps = 115200;

	LPUART_Init(LPUART1, &lpuart_config, uartClkSrcFreq); //Init LPUART0 for NBiot

	LPUART_EnableInterrupts(LPUART0, kUART_RxDataRegFullInterruptEnable); //enable LPUART0 receive interrupt to receive data from NBiot

	EnableIRQ(LPUART0_IRQn);

	UART_EnableInterrupts(UART2, kUART_RxDataRegFullInterruptEnable); //enable LPUART0 receive interrupt to receive data from GPS
	EnableIRQ(UART2_FLEXIO_IRQn);

	LPUART_EnableInterrupts(LPUART1, kUART_RxDataRegFullInterruptEnable); //enable UART0 receive interrupt to receive data from PC
	EnableIRQ( LPUART1_IRQn );

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

int main(void) {

	PMC->REGSC |= 0x08;	//acknowledge wake up to voltage regulator module, this is needed with LLWU wake up
	EnableIRQ(LLWU_IRQn);//enable LLWU interrupts. if we wake up from VLLS mode, it means that next MCU
						 //will jump to the LLWU interrupt vector

	struct reindeerData_t reindeerData; //create struct for our reindeer data that will be sent
	char mqttMessage[450];

	BOARD_InitPins();	//init all physical pins
	//BOARD_BootClockRUN();  //by uncommenting this we can use FRDM 50Mhz external clock, but will not work with modified board
	BOARD_InitDebugConsole();

	/*
	 * Calculate how many processor ticks are in 1 ms to make accurate delay_ms function
	 * first take MCU clock frequency, divide by 1000ms and divide by 7 because our delay_ms loop takes 7 machine cycles
	 */
	ms_ticks = BOARD_DEBUG_UART_CLK_FREQ / 1000 / 7;

	SysTick_Config(BOARD_DEBUG_UART_CLK_FREQ / 1000); //setup SysTick timer for 1ms interval for delay functions(see timing.h)

	//initI2C();
	//initAdc();
	initUART();
	//configure_acc();
	//acc_init();
	//initTimer();

	static const gpio_pin_config_t LED_configOutput = { kGPIO_DigitalOutput, /* use as output pin */
	1, /* initial value */
	};


	/*
	SMC_SetPowerModeProtection(SMC, kSMC_AllowPowerModeVlls);
	smc_power_mode_vlls_config.subMode = kSMC_StopSub1; /*!< Stop submode 1, for VLLS1/LLS1.

	LLWU->ME |= 0x01; 		// enable LLWU wakeup source from LPTMR module
	LLWU->PE3 |= 0x01; // enable LLWU wakeup source from accelerometer interrupt pin
					   // 0x20 for stock frdm pin enable,
	LLWU->FILT1 |= 0x28;	// set pin wakeup from rising edge, 0x2A for frdm

	//EnableIRQ(PORTC_IRQn);

	LPTMR_EnableInterrupts(LPTMR0, LPTMR_CSR_TIE_MASK);	//Sets Timer Interrupt Enable bit to 1
	LPTMR_StartTimer(LPTMR0);
*/
	GPIO_PinInit(GPIOA, 4u, &LED_configOutput);	//blue led as output

	while(true) {
		GPIO_PortToggle(GPIOA, 1 << 4u);
		delay_ms(1000);
	}

	/*
	 * set boost regulator enable pin as output. This pin will control the power to RF modules
	 */
	GPIO_PinInit(GPIOA, 1u, &LED_configOutput);

	GPIO_SetPinsOutput(GPIOA, 1 << 1u); //Power on RF modules

	//fletcher8(PMC_set, 14);
	//fletcher8(ubx_cfg_prt, 7);
	//ubx_send(ubx_cfg_prt);

	printf(
			"Reindeer IoT has started\r\nCommand \"iot\" to start executing reindeer track cycle\r\n"
					"Command \"gpsinfo=1\" or \"gpsinfo=0\" to switch GPS data on/off\r\n");
	printf(
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
	/*
	 while (true) {

	 int16_t acc_val_x = read_acc_axis(0);
	 int16_t acc_val_y = read_acc_axis(1);
	 int16_t acc_val_z = read_acc_axis(2);
	 printf("Akseloreometer x: %d y: %d z: %d\r\n", acc_val_x, acc_val_y, acc_val_z);
	 delay_ms(500);
	 }
	 */
	if (wake == 2) {
		printf("Woken by ACCEL, reindeer is !!!ALIVE!!!\r\n");
		SMC_PreEnterStopModes();
		SMC_SetPowerModeVlls(SMC, &smc_power_mode_vlls_config);
	}

	while (1) {

		//int16_t acc_val = read_acc_axis(0);
		//printf("Accelereometer %d\r\n",acc_val);
		//break;
		/*
		 * Check if a string has arrived from PC (with CR line end)
		 */

		if (wake == 1) {
			strcpy(reindeerData.dead, "true");
			printf("Woken by LPTMR, reindeer is !!!%s!!\r\n",
					reindeerData.dead);

			while (true) {
				if (GPS_strReady) {
					printf(GPS_recBuf);
					printf("\r\n"); //First print out whole buffer

					char testLat[12] = ("6500.02359");
					char testLon[12] = ("02530.56951");

					strcpy(reindeerData.latitude, testLat);
					strcpy(reindeerData.longitude, testLon);
					break;

					if (getGPS()) {
						//char testLat[12] = ("6500.02359");
						//char testLon[12] = ("02530.56951");

						//parseData(testLat,testLon);

						strcpy(reindeerData.latitude, parsedLat);
						strcpy(reindeerData.longitude, parsedLon);
						break;
					}
					memset(GPS_recBuf, 0, 600);
					GPS_bufPtr = 0;
					GPS_strReady = 0;
				}

			}
			printf("Parsed latitude: %s\r\n", reindeerData.latitude);
			printf("Parsed longitude: %s\r\n", reindeerData.longitude);
			break;
		}

		if (PC_strReady) {

			if (strstr(PC_recBuf, "iot") != NULL) {
				printf("Starting Reindeer IoT cycle\r\n");
				break;
			} else if (strstr(PC_recBuf, "gpsinfo=1") != NULL) {
				streamGps = 1;
			} else if (strstr(PC_recBuf, "gpsinfo=0") != NULL) {
				streamGps = 0;
			} else if (strstr(PC_recBuf, "rfoff") != NULL) {
				GPIO_ClearPinsOutput(GPIOB, 1 << 11u); //Power on RF modules
			} else if (strstr(PC_recBuf, "rfon") != NULL) {
				GPIO_SetPinsOutput(GPIOB, 1 << 11u); //Power on RF modules
			} else if (strstr(PC_recBuf, "\xb5\x62") != NULL) //if input is UBX command!
			{
				printf("send to gps\r\n");
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

			printf(NB_recBuf);
			printf("\r\n");
			memset(NB_recBuf, 0, 500);
			NB_bufPtr = 0;
			NB_strReady = 0;
		}

		/*
		 * If GPS string is ready and GPS data streaming is enabled, enter here to process GPS data buffer
		 */
		if (GPS_strReady && streamGps) {

			//printf(GPS_recBuf);
			//printf("\r\n"); //First print out whole buffer

			if (getGPS()) {
				//char testLat[12] = ("6500.02359");
				//char testLon[12] = ("02530.56951");

				//parseData(testLat,testLon);

				strcpy(reindeerData.latitude, parsedLat);
				strcpy(reindeerData.longitude, parsedLon);
				break;
			}

			printf("Parsed latitude: %s\r\n", reindeerData.latitude);
			printf("Parsed longitude: %s\r\n", reindeerData.longitude);

			char* ubxResponseStartPtr = strstr(GPS_recBuf, "\xb5\x62"); //Find pointer to UBX header. If there is no UBX response, pointer
																		//will be NULL

			if (ubxResponseStartPtr != NULL) //If pointer is not null, it means UBX response header is found
			{
				printf("Found UBX response\r\n");

				uint8_t responseLength = *(ubxResponseStartPtr + 4); //Find out UBX response length, it is always the 5th byte
																	 //from beginning of the packet. * means dereferencing pointer
				//dereferencing means "accessing the value where pointer points"

				printf("UBX response length: %02x\r\n", responseLength);

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
	printf("Roger\r\n");
	//parseData(testLat, testLon);
}
/*
 void PORTC_IRQHandler() {

 PORTC->PCR[6] |= 0x01000000;

 while ( PORTC->PCR[6] & 0x01000000) {

 }

 LPTMR_Deinit(LPTMR0);			// Deinitiate timer to reset timer counte
 LPTMR_Init(LPTMR0, &lptmr_config);
 LPTMR_SetTimerPeriod(LPTMR0, 2000);  // 3000 for 20hz data rat
 LPTMR_EnableInterrupts(LPTMR0, LPTMR_CSR_TIE_MASK);	//Sets Timer Interrupt Enable bit to 1
 LPTMR_StartTimer(LPTMR0);
 GPIO_PortToggle(GPIOB, 1 << 22u);

 }
 */
void LLWU_IRQHandler() {

	if ( LLWU->F3 & 0x01) {	// 1 = LPTMR interrupt, 2 = Accel interrupt, 0 = No interrupts
		wake = 1;
		CLOCK_EnableClock(kCLOCK_Lptmr0);
		LPTMR0->CSR |= LPTMR_CSR_TCF_MASK;

	}

	else if ( LLWU->F2 & 0x01) {// 0x04 for stock frdm acc wakeup reg, 0x01 for customized
		wake = 2;
		LLWU->F2 |= 0x01;
	}

	LLWU->F2 = 0x01;
}

void LPTMR0_IRQHandler() {

	LPTMR0->CSR |= LPTMR_CSR_TCF_MASK;		// Clear the interrupt flag
	while ( LPTMR0->CSR & LPTMR_CSR_TCF_MASK) {

	}

	PMC->REGSC |= 0x08;

	GPIO_PortToggle(GPIOB, 1 << 21u); //light blue LED
}

void LPUART0_IRQHandler () {

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
	GPIO_PortToggle(GPIOB, 1 << 22u); //toggle RED led to indicate data arrived from GPS module

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

	LPUART_ClearStatusFlags(LPUART1, kUART_RxDataRegFullFlag);
	GPIO_PortToggle(GPIOB, 1 << 21u); //toggle BLUE led to indicate data arrived from computer

	uint8_t uartData = LPUART1->DATA;

	PC_recBuf[PC_bufPtr] = uartData;
	PC_bufPtr++;

	if (uartData == 0x0d) {
		PC_strReady = 1;

	}

}

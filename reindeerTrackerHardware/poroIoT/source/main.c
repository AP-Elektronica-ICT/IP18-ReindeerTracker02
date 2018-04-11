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

#define LPTMR_TIMEOUT 8000

lptmr_config_t lptmr_config;
smc_power_mode_vlls_config_t smc_power_mode_vlls_config;
uart_config_t uart_config;

volatile uint8_t wake = 5;
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

void initTimer()
{

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
	EnableIRQ(LPTMR0_IRQn);

}

void disableUartInterrupts()
{
	UART_DisableInterrupts(UART2, kUART_RxDataRegFullInterruptEnable); //enable LPUART0 receive interrupt to receive data from GPS
	LPUART_DisableInterrupts(LPUART1, kLPUART_RxDataRegFullInterruptEnable); //enable UART0 receive interrupt to receive data from PC
	LPUART_DisableInterrupts(LPUART0, kLPUART_RxDataRegFullInterruptEnable); //enable LPUART0 receive interrupt to receive data from NBiot

}

void configureSleepMode()
{
	SMC_SetPowerModeProtection(SMC, kSMC_AllowPowerModeAll);
	smc_power_mode_vlls_config.subMode = kSMC_StopSub1; //!< Stop submode 1, for VLLS1/LLS1.

	LLWU->ME |= 0x21; // enable LLWU wakeup source from LPTMR module and RTC alarm
	LLWU->PE2 |= 0x04; // enable LLWU wakeup source from accelerometer interrupt pin
	// 0x20 for stock frdm pin enable,
	LLWU->FILT1 |= 0x25; // set pin wakeup from rising edge, 0x2A for frdm
}

/*
 *
 * Init all needed UART buses. LPUART0 for NB-IoT, UART0 for PC, UART2 for GPS
 */

void initUART()
{

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

void deInit_Uart()
{
	UART_Deinit(UART2);
	LPUART_Deinit(LPUART0);
	LPUART_Deinit(LPUART1);
}

/*
 * Send data to NBiot with LPUART0
 * String to be sent is pointed by *data
 *
 */

void NB_send(char *data)
{
	char c = *data++; //assign c a character from the string and post-increment string pointer
	while (c)
	{ //loop until c is zero which means string has ended and no more chars has to be sent

		while (!((LPUART0->STAT) & kLPUART_TxDataRegEmptyFlag))
		{
		} //wait until LPUART0 Transmission Complete flag rises, so we can send new char
		LPUART0->DATA = c; //write new character to transmit buffer
		c = *data++; //assign next character to c and post-increment string pointer
	}
}

void GPS_send(char *data, uint8_t len)
{
	char c = *data++; //assign c a character from the string and post-increment string pointer
	for (; len > 0; len--)
	{ //loop until c is zero which means string has ended and no more chars has to be sent

		while (!((UART2->S1) & 0x80))
		{
		} //wait until LPUART0 Transmission Complete flag rises, so we can send new char
		UART2->D = c; //write new character to transmit buffer
		c = *data++; //assign next character to c and post-increment string pointer
	}
}

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
			UART2 ->S1,LPUART1 ->STAT, LPUART0 ->STAT);
	PCprint(buf);
}

int main(void)
{

	PMC->REGSC |= 0x08;	//acknowledge wake up to voltage regulator module, this is needed with LLWU wake up
	NVIC_EnableIRQ(LLWU_IRQn);//enable LLWU interrupts. if we wake up from VLLS mode, it means that next MCU
							  //will jump to the LLWU interrupt vector

	BOARD_InitPins();	//init all physical pins
	BOARD_BootClockVLPR(); //by uncommenting this we can use FRDM 50Mhz external clock, but will not work with modified board

	SysTick_Config(BOARD_DEBUG_UART_CLK_FREQ / 1000); //setup SysTick timer for 1ms interval for delay functions(see timing.h)
	delay_ms(10);

	initUART();
	EnableIRQ(RTC_IRQn);

	configureSleepMode();

	if (wake == 2) //wakeup by accelerometer
	{
		disableUartInterrupts();

		PCprint("Woken by ACCEL, reindeer is !!!ALIVE!!!\r\n");
		initTimer();
		LPTMR_EnableInterrupts(LPTMR0, kLPTMR_TimerInterruptEnable); //Sets Timer Interrupt Enable bit to 1
		LPTMR_StartTimer(LPTMR0);

		SMC_PreEnterStopModes();
		SMC_SetPowerModeVlls(SMC, &smc_power_mode_vlls_config);
		SMC_PostExitStopModes();

		printInterruptFlags();

		if (wake == 3)
		{
			PCprint(
					"rtc prevented entry to sleep mode\r\nsending weekly report now\r\n");
		}

	}

	if (wake == 0) //first startup, init all interrupt sources and go sleep
	{

		initI2C();
		configure_acc();
		acc_init();
		rtcInit();
		initTimer();

		PCprint("wake was 0 going to sleep\r\n");

		LPTMR_EnableInterrupts(LPTMR0, kLPTMR_TimerInterruptEnable); //Sets Timer Interrupt Enable bit to 1
		LPTMR_StartTimer(LPTMR0);

		SMC_PreEnterStopModes();
		SMC_SetPowerModeVlls(SMC, &smc_power_mode_vlls_config);
	}
	if (wake == 3)
	{
		PCprint("rtc alarm heratti perkele\r\nsending weekly report\r\n");
		initTimer();
		/*
		 * Enable RTC and LPTMR IRQ to flush possible interrupts
		 */
		EnableIRQ(RTC_IRQn);

		rtcInit(); //Init RTC again to have next alarm next week
		initUART();
		/*
		 * Disable LPTMR interrupt to not cause confusion during sending
		 */
		CLOCK_EnableClock(kCLOCK_Lptmr0);
	}

	struct reindeerData_t reindeerData; //create struct for our reindeer data that will be sent
	char mqttMessage[450];

	static const gpio_pin_config_t LED_configOutput =
	{ kGPIO_DigitalOutput, /* use as output pin */
	1, /* initial value */
	};
	GPIO_PinInit(GPIOA, 4u, &LED_configOutput);	//blue led as output

	GPIO_PinInit(GPIOA, 19u, &LED_configOutput);

	/*
	 * set boost regulator enable pin as output. This pin will control the power to RF modules
	 */
	GPIO_PinInit(GPIOA, 1u, &LED_configOutput);

	GPIO_ClearPinsOutput(GPIOA, 1 << 1u); //Power off RF modules

	PCprint(
			"Reindeer IoT has started\r\nCommand \"iot\" to start executing reindeer track cycle\r\n"
					"Command \"gpsinfo=1\" or \"gpsinfo=0\" to switch GPS data on/off\r\n");
	PCprint(
			"Or enter normal AT commands here for SARA-N2\r\nModules powered on and booting now!\r\n");

	/*
	 * Copy all reindeer variables to struct before starting network operations
	 */

	char testLat[12] = ("6500.02359");
	char testLon[12] = ("02530.56951");
	strcpy(reindeerData.serialNum, "11111");
	strcpy(reindeerData.latitude, testLat);
	strcpy(reindeerData.longitude, testLon);

	if (wake == 3)
	{
		strcpy(reindeerData.dead, "false");
	}
	else if (wake == 1)
	{
		EnableIRQ(LPTMR0_IRQn); //by enabling LPTMR IRQ it will automatically clear the flag
								//if the timer runs out during sending
		PCprint("reindeer is dead\r\n");
		strcpy(reindeerData.dead, "true");
	}

	reindeerData.batteryLevel = 45;
	PCprint("entering while loop\r\n");

	while (1)
	{

		/*
		 * Check if a string has arrived from PC (with CR line end)
		 */

		while (true)
		{
			break;
			if (!GPS_strReady) //Loop until get GPS coordinates
			{
				PCprint(GPS_recBuf);
				PCprint("\r\n"); //First print out whole buffer

				char testLat[12] = ("6500.02359");
				char testLon[12] = ("02530.56951");

				parseData(testLat, testLon);

				strcpy(reindeerData.latitude, testLat);
				strcpy(reindeerData.longitude, testLon);

				break; //For test purpose break away without real GPS coordinates

				if (getGPS())
				{

					parseData(testLat, testLon);

					strcpy(reindeerData.latitude, parsedLat);
					strcpy(reindeerData.longitude, parsedLon);
					break;
				}
				memset(GPS_recBuf, 0, 600);
				GPS_bufPtr = 0;
				GPS_strReady = 0;
			}

		}

		//break;

		if (PC_strReady)
		{

			if (strstr(PC_recBuf, "iot") != NULL)
			{
				PCprint("Starting Reindeer IoT cycle\r\n");
				break;
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

		if (NB_strReady)
		{
			moduleResponseTimeout = millis() + RESPONSE_TIMEOUT_NORMAL_VALUE; //reset timeout to initial value

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
																	 //from beginning of the packet.
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
	 * Start data sending
	 */

	GPIO_SetPinsOutput(GPIOA, 1 << 1u); //Power on RF modules
	delay_ms(500); //Wait until voltage stabilize
	NB_reboot();
	delay_ms(50);

	/*
	 * Assemble data to json format and then to POST message
	 */

	uint8_t msgLen = assembleMqtt(&reindeerData, mqttMessage);

	NB_create_pdp_send(mqttMessage, msgLen);
	PCprint("Roger include main.c\r\n");

	GPIO_ClearPinsOutput(GPIOA, 1 << 1u); //Power off RF modules

	/*
	 * Message sending is done and rf modules powered off
	 *
	 * If we just sent weekly report then we must have lptmr and rtc running
	 *
	 * If sent dead reindeer then se on siinä
	 */


	/*
	 * Reset LPTIMER so it starts from zero when device goes to sleep
	 * this prevents LPTIMER too early interrupt because it was already counting
	 */

	initTimer();
	LPTMR_EnableInterrupts(LPTMR0, kLPTMR_TimerInterruptEnable); //Sets Timer Interrupt Enable bit to 1
	LPTMR_StartTimer(LPTMR0);

	configureSleepMode();

	printInterruptFlags();
	disableUartInterrupts();

	SMC_PreEnterStopModes();
	SMC_SetPowerModeVlls(SMC, &smc_power_mode_vlls_config);
	SMC_PostExitStopModes();

	while (1)
	{
		PCprint("sss\r\n");
		delay_ms(1000);
	}
}

void LLWU_IRQHandler()
{

	/* If wakeup by LPTMR. */
	if ( LLWU->F3 & 0x01)
	{ // 1 = LPTMR interrupt, 2 = Accel interrupt, 3 = RTC alarm interrupt, 0 = No interrupts

		wake = 1;
		CLOCK_EnableClock(kCLOCK_Lptmr0);
		LPTMR0->CSR |= LPTMR_CSR_TCF_MASK;

	}
	else if (LLWU->F3 & 0x20) //if wakeup from RTC alarm
	{
		wake = 3;
		CLOCK_EnableClock(kCLOCK_Rtc0); //enable RTC bus clock so we can write registers
		RTC->TAR = 0x1f; //write to alarm register to clear interrupt flag
	}

	else if ( LLWU->F1 & 0x20)
	{	// 0x04 for stock frdm acc wakeup reg, 0x01 for customized

		wake = 2;
		LLWU->F1 |= 0x20;
	}

	LLWU->F1 = 0x20;
}

void LPUART0_IRQHandler()
{

	uint8_t uartData = LPUART0->DATA;
	if (uartData != 0)
	{

		NB_recBuf[NB_bufPtr] = uartData;
		NB_bufPtr++;

		if (uartData == 0x0d)
		{
			NB_strReady = 1;
			//NB_bufPtr = 0;
		}

	}

}

void UART2_FLEXIO_IRQHandler()
{

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
	if (GPS_strReady == 0)
	{
		GPS_recBuf[GPS_bufPtr] = uartData; //put new byte to buffer
		GPS_bufPtr++;
	}

	/*
	 * When buffer is almost full, put strReady high and stop filling it
	 */
	if (GPS_bufPtr > 499)
	{
		GPS_strReady = 1;
	}

}

void LPUART1_IRQHandler()
{

	uint8_t uartData = LPUART1->DATA;
	//GPIO_PortToggle(GPIOA, 1 << 4u);

	PC_recBuf[PC_bufPtr] = uartData;
	PC_bufPtr++;

	if (uartData == 0x0d)
	{
		PC_strReady = 1;
		PC_bufPtr = 0;

	}
}

void LPTMR0_IRQHandler()
{
	wake = 1;
	CLOCK_EnableClock(kCLOCK_Lptmr0);
	LPTMR0->CSR |= LPTMR_CSR_TCF_MASK;

	while (LPTMR0->CSR & LPTMR_CSR_TCF_MASK)
	{
	}
}

void RTC_IRQHandler()
{
	wake = 3;
	CLOCK_EnableClock(kCLOCK_Rtc0);

	RTC->SR &= ~0x10;	//disable counter for resetting TSR
	RTC->TSR = 0;		//reset TSR
	RTC->TAR = RTC_REPORT_INTERVAL; //clear interrupt flag by writing to TAR

	RTC->SR |= 0x10; //enable counter

	while (RTC->SR & 0x04) //wait until interrupt flag is cleared
	{
	}

}

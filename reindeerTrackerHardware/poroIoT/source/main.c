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
#include "dbg_util.h"

#define TEMP_CHANNEL 1
#define VOLTAGE_MEAS_CHANNEL 2
#define DEBUG_MODE 0
#define GPS_GIVEUP_TIME 90000


smc_power_mode_vlls_config_t smc_power_mode_vlls_config;
uart_config_t uart_config;

uint8_t temperature;
char temp_buf[50];
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

void configureSleepMode() {
	SMC_SetPowerModeProtection(SMC, kSMC_AllowPowerModeAll);
	smc_power_mode_vlls_config.subMode = kSMC_StopSub1; //!< Stop submode 1, for VLLS1/LLS1.
	LLWU->ME |= 0x20; // enable LLWU wakeup source from RTC alarm
	LLWU->PE2 |= 0x04; // enable LLWU wakeup source from accelerometer interrupt pin
}

/*
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
	LPUART_Init(LPUART1, &lpuart_config, lpuartClkSrcFreq); //Init LPUART1 for PCuart
	UART_Init(UART2, &uart_config, uartClkSrcFreq); //UART2 for GPS with same settings!

	NVIC_SetPriority(LPUART0_IRQn, 2);
	LPUART_EnableInterrupts(LPUART0, kLPUART_RxDataRegFullInterruptEnable); //enable LPUART0 receive interrupt to receive data from NBiot
	EnableIRQ(LPUART0_IRQn);

	NVIC_SetPriority(UART2_FLEXIO_IRQn, 3);
	UART_EnableInterrupts(UART2, kUART_RxDataRegFullInterruptEnable); //enable LPUART0 receive interrupt to receive data from GPS
	EnableIRQ(UART2_FLEXIO_IRQn);

	NVIC_SetPriority(LPUART1_IRQn, 4);
	LPUART_EnableInterrupts(LPUART1, kLPUART_RxDataRegFullInterruptEnable); //enable UART0 receive interrupt to receive data from PC
	EnableIRQ(LPUART1_IRQn);

}

void deInit_Uart() {
	UART_Deinit(UART2);
	LPUART_Deinit(LPUART0);
	LPUART_Deinit(LPUART1);
}

void disableUartInterrupts() {
	UART_DisableInterrupts(UART2, kUART_RxDataRegFullInterruptEnable); //enable LPUART0 receive interrupt to receive data from GPS
	LPUART_DisableInterrupts(LPUART1, kLPUART_RxDataRegFullInterruptEnable); //enable UART0 receive interrupt to receive data from PC
	LPUART_DisableInterrupts(LPUART0, kLPUART_RxDataRegFullInterruptEnable); //enable LPUART0 receive interrupt to receive data from NBiot
}

int main(void) {

	NVIC_SetPriority(LLWU_IRQn, 0);
	NVIC_EnableIRQ(LLWU_IRQn); //enable LLWU interrupts. if we wake up from VLLS mode, it means that next MCU
	//will jump to the LLWU interrupt vector
	PMC->REGSC |= 0x08;	//acknowledge wake up to voltage regulator module, this is needed with LLWU wake up

	BOARD_InitPins();	//init all physical pins
	BOARD_BootClockVLPR(); //by uncommenting this we can use FRDM 50Mhz external clock, but will not work with modified board

	SysTick_Config(BOARD_DEBUG_UART_CLK_FREQ / 1000); //setup SysTick timer for 1ms interval for delay functions(see timing.h)
	NVIC_SetPriority(SysTick_IRQn, 6);

	configureSleepMode();
	initUART();
	NVIC_SetPriority(RTC_IRQn, 5);
	//EnableIRQ(RTC_IRQn);

	static const gpio_pin_config_t LED_configOutput = { kGPIO_DigitalOutput, /* use as output pin */
	1, /* initial value */
	};
	GPIO_PinInit(GPIOA, 4u, &LED_configOutput);	//Indicator led as output

#if DEBUG_MODE == 0

	if (wake == 2) //wakeup by accelerometer
			{
		disableUartInterrupts();

		blinkLed(50);

		PCprint("Woken by ACCEL, reindeer is !!!ALIVE!!!\r\n");
		//initTimer();
		//LPTMR_EnableInterrupts(LPTMR0, kLPTMR_TimerInterruptEnable); //Sets Timer Interrupt Enable bit to 1
		//LPTMR_StartTimer(LPTMR0);

		SMC_PreEnterStopModes();
		SMC_SetPowerModeVlls(SMC, &smc_power_mode_vlls_config);
		SMC_PostExitStopModes();

		if (wake == 3) {
			PCprint(
					"rtc prevented entry to sleep mode\r\nsending weekly report now\r\n");
		}

	}

	if (wake == 0) //first startup, init all interrupt sources and go sleep
			{

		initI2C();
		delay_ms(10);
		configure_acc();
		acc_init();
		initAdc();
		rtcInit();
		//initTimer();

		PCprint("wake was 0 going to sleep\r\n");

		//LPTMR_EnableInterrupts(LPTMR0, kLPTMR_TimerInterruptEnable); //Sets Timer Interrupt Enable bit to 1
		//LPTMR_StartTimer(LPTMR0);

		SMC_PreEnterStopModes();
		SMC_SetPowerModeVlls(SMC, &smc_power_mode_vlls_config);
	}
	if (wake == 3) {
		PCprint("rtc alarm heratti perkele\r\nsending weekly report\r\n");

		initAdc();
		//initTimer();
		/*
		 * Enable RTC and LPTMR IRQ to flush possible interrupts
		 */
		EnableIRQ(RTC_IRQn);

		rtcInit(); //Init RTC again to have next alarm next week
		initUART();
		/*
		 * Disable LPTMR interrupt to not cause confusion during sending
		 */
		//CLOCK_EnableClock(kCLOCK_Lptmr0);
	}

#endif

	struct reindeerData_t reindeerData; //create struct for our reindeer data that will be sent
	char mqttMessage[450];
	char teunMessage[150];

	/*
	 * set boost regulator enable pin as output. This pin will control the power to RF modules
	 */
	GPIO_PinInit(GPIOA, 1u, &LED_configOutput);
	GPIO_PinInit(GPIOC, 6u, &LED_configOutput); //GNSS enable pin
	GPIO_PinInit(GPIOA, 2u, &LED_configOutput); //GnSS backup voltage pin
	GPIO_SetPinsOutput(GPIOA, 1 << 2u); //Set backup voltage

	GPIO_ClearPinsOutput(GPIOA, 1 << 1u); //Power off RF modules

	GPIO_SetPinsOutput(GPIOC, 1 << 6u); //Power off GPS

	blinkLed(500);
	PCprint("Reindeer IoT is reset \r\n");

	/*
	 * Copy all reindeer variables to struct before starting network operations
	 */

	strcpy(reindeerData.serialNum, "88888");

	if (wake == 3) {
		strcpy(reindeerData.dead, "false");
	} else if (wake == 1) {
		//EnableIRQ(LPTMR0_IRQn); //by enabling LPTMR IRQ it will automatically clear the flag
								//if the timer runs out during sending
		PCprint("reindeer is dead\r\n");
		strcpy(reindeerData.dead, "true");
	}

	reindeerData.batteryLevel = 0;
	reindeerData.temperature = temperature;
	PCprint("entering while loop\r\n");

#if DEBUG_MODE == 1

	char testLat[12] = ("6500.02359");
	char testLon[12] = ("02530.56951");

	initAdc();
	PCprint(
			"On debug mode, commands: rfon, rfoff, iot, gpsinfo=1, gpsinfo=0, AT commands\r\n");

	while (1)
	{

		//Check if a string has arrived from PC (with CR line end)

		uint8_t cmd = checkPcInputAndProcess(PC_recBuf);
		checkNBDataAndPrint(NB_recBuf);
		checkGPSDataAndPrint(GPS_recBuf);

		if (cmd == 1)
		{
			strcpy(reindeerData.dead, "false");
			parseData(testLat,testLon);
			strcpy(reindeerData.latitude, parsedLat);
			strcpy(reindeerData.longitude, parsedLon);
			break; //if PC commanded "iot" then break and start sending
		}
	}

#else

	GPIO_SetPinsOutput(GPIOA, 1 << 1u); //Power on RF modules
	NB_reboot();
	AT_send("CFUN=0","","OK");

	PCprint("Waiting for GPS coordinates...\r\n");
	GPIO_ClearPinsOutput(GPIOC, 1 << 6u); //Power on GPS
	delay_ms(1000);
	reindeerData.batteryLevel = (uint8_t) batteryMeas(); //Measure battery voltage in percentage
	//CLOCK_DisableClock(kCLOCK_Adc0);

	char buf[50];
	sprintf(buf, "Voltage: %d\r\n", reindeerData.batteryLevel);
	PCprint(buf);

	deInit_Uart();
	//delay_ms(30000); //wait for GPS acquisition with uart deinitialised

#if PRINT_GPS
	initUART();
#endif
	//delay_ms(10000); //wait so capacitor charges back a little

	GPIO_ClearPinsOutput(GPIOC, 1 << 6u); //power GPS

	uint32_t gps_timeout = millis();
	gps_timeout += GPS_GIVEUP_TIME; //1,5 min timeout

	while (true) {

		//break;

		if (GPS_strReady) //Loop until get GPS coordinates
		{

#if PRINT_GPS

			PCprint(GPS_recBuf); //Print GPS data buffer
			PCprint("\r\n");
#endif

			blinkLed(20);

			if (getGPS()) {

				strcpy(reindeerData.latitude, parsedLat);
				strcpy(reindeerData.longitude, parsedLon);
				break;
			}
			memset(GPS_recBuf, 0, 600);
			GPS_bufPtr = 0;
			GPS_strReady = 0;

			reindeerData.batteryLevel = (uint8_t) batteryMeas(); //Measure battery voltage in percentage
	//CLOCK_DisableClock(kCLOCK_Adc0);

			/*char buf[50];
			sprintf(buf, "Voltage: %d\r\n", reindeerData.batteryLevel);
			PCprint(buf);*/
		}

		if(millis() > gps_timeout) //could not find GPS in time, break and send zero coordinates
		{
			strcpy(reindeerData.latitude, "00.000000");
			strcpy(reindeerData.longitude, "00.000000");
			break;
		}

	}

#endif

#if PRINT_GPS == 0
	initUART();
#endif

	//Start data sending

	GPIO_SetPinsOutput(GPIOC, 1 << 6u); //turn off GPS

	PCprint("GPS turned off, wait for cap charge \r\n");
	//delay_ms(10000);
	//Assemble data to json format and then to MQTT message

	char address[30];

	uint8_t msgLen = assembleMqtt(&reindeerData, mqttMessage, teunMessage);

	NB_reboot();
	delay_ms(3000);

	strcpy(address, "0,\"168.235.64.81\",1884"); //yoricks server
	NB_create_pdp_send(address, mqttMessage, msgLen);

	PCprint("Roger include int main &lumen\r\n");

	AT_send("CFUN=0", "", "OK");
	delay_ms(500);
	GPIO_ClearPinsOutput(GPIOA, 1 << 1u); //Power off RF modules

	/*
	 * Message sending is done and rf modules powered off
	 * If we just sent weekly report then we must have lptmr and rtc running
	 * If sent dead reindeer then se on siinä

	 * Reset LPTIMER so it starts from zero when device goes to sleep
	 * this prevents LPTIMER too early interrupt because it was already counting
	 */

	/*initTimer();
	LPTMR_EnableInterrupts(LPTMR0, kLPTMR_TimerInterruptEnable); //Sets Timer Interrupt Enable bit to 1
	LPTMR_StartTimer(LPTMR0);*/

	configureSleepMode();
	disableUartInterrupts();

	SMC_PreEnterStopModes();
	SMC_SetPowerModeVlls(SMC, &smc_power_mode_vlls_config);
	SMC_PostExitStopModes();

	while (1) {
		PCprint("Failed to enter stop mode, retrying\r\n");
		delay_ms(1000);
		SMC_PreEnterStopModes();
		SMC_SetPowerModeVlls(SMC, &smc_power_mode_vlls_config);
		SMC_PostExitStopModes();
	}
}

void LLWU_IRQHandler() {

	/* If wakeup by LPTMR. */
	if ( LLWU->F3 & 0x01) { // 1 = LPTMR interrupt, 2 = Accel interrupt, 3 = RTC alarm interrupt, 0 = No interrupts

		wake = 1;
		CLOCK_EnableClock(kCLOCK_Lptmr0);
		LPTMR0->CSR |= LPTMR_CSR_TCF_MASK;

	} else if (LLWU->F3 & 0x20) //if wakeup from RTC alarm
			{
		wake = 3;
		CLOCK_EnableClock(kCLOCK_Rtc0); //enable RTC bus clock so we can write registers
		RTC->TAR = RTC_REPORT_INTERVAL; //write to alarm register to clear interrupt flag
	}

	else if ( LLWU->F1 & 0x20) {// 0x04 for stock frdm acc wakeup reg, 0x01 for customized

		wake = 2;
		LLWU->F1 |= 0x20;
		LLWU->FILT1 |= 0x80;
		while (LLWU->F1 & 0x20)
			;
	}

}

void LPUART0_IRQHandler() {
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
	PC_recBuf[PC_bufPtr] = uartData;
	PC_bufPtr++;

	if (uartData == 0x0d) {
		PC_strReady = 1;
		PC_bufPtr = 0;
	}
}

void LPTMR0_IRQHandler() {
	wake = 1;
	CLOCK_EnableClock(kCLOCK_Lptmr0);
	LPTMR0->CSR |= LPTMR_CSR_TCF_MASK;

	while (LPTMR0->CSR & LPTMR_CSR_TCF_MASK) {
	}
}

void RTC_IRQHandler() {

	CLOCK_EnableClock(kCLOCK_Rtc0);

	RTC->SR &= ~0x10;	//disable counter for resetting TSR
	RTC->TSR = 0;		//reset TSR

	if (RTC->SR & 0x04) {
		wake = 3;
		RTC->TAR = RTC_REPORT_INTERVAL; //clear interrupt flag by writing to TAR
		while (RTC->SR & 0x04) //wait until interrupt flag is cleared
		{
		}
	}

	RTC->SR |= 0x10; //enable counter
}

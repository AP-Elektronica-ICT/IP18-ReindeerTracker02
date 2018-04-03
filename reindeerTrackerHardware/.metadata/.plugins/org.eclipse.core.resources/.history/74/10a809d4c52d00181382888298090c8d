#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "at_func.h"
#include <string.h>

extern void UART3_send(char* data);
extern uint8_t UART3_receive();
extern void delay();
extern volatile uint8_t UART3_strReady;

extern char UART3_recBuf[1000];
extern volatile uint16_t UART3_bufPtr;

char AT_REQ[] = "AT";
char AT_GPSDBG_ON[] = "CGNSDBG=1";
char AT_GPSTST_ON[] = "CGNSTST=1";
char AT_GPSINFO[] = "CGNSINF";

char AT_CGMI[] = "CGMI";  // check manufacturer
char AT_NPIN[] = "NPIN=0,\"1234\"";  // sets, the PIN-code
char AT_COPS[] = "COPS";            //Register to operator network
char AT_CEREG[] = "CEREG?";  //connection status
char AT_NRB[] = "NRB";
char AT_CGDCONT[] = "CGDCONT=1,\"IP\",\"\",\"\"";
char AT_CGACT[] = "CGACT";
char AT_CSQ[] = "CSQ";  //Signal quality
char AT_CGPADDR[] = "CGPADDR";        //shows module ip address
char AT_NSOCR[] = "NSOCR=\"DGRAM\",17,42000,1";     //create UDP socket, port 420000
char AT_NSOST[] = "NSOST=0,\"195.34.89.241\",7,2,\"F8F8\"";
char AT_CMEE[] = "CMEE=";

char AT_NSORF[] = "NSORF=0,2";    //show received data, =<socker>, <data_lenght>
char AT_CFUN[] = "CFUN"; // sets functionality mode, edit this to change power mode
char AT_CCLK[] = "CCLK"; // sets and reads rtc example: AT+CCLK="14/07/01,15:00:00+01"
char AT_CREG[] = "CREG";

char AT_CIICR[] = "CIICR";
char AT_CIFSR[] = "CIFSR";
char AT_CIPSTATUS[] = "CIPSTATUS";
char AT_CIPSTART[] = "CIPSTART=\"TCP\",\"139.59.155.145\",1883";
char AT_CIPSEND[] = "CIPSEND=";
char AT_CGPS[] = "CGPS=";

void NB_init() {
	char cmd_buf[50];

	sprintf(cmd_buf, "AT+%s\r\n", AT_CMEE);

	printf(cmd_buf);
}

uint8_t AT_send(char *AT_cmd, char *AT_parameter, char *AT_exptAnswer) {

	char cmd_buf[100];

	uint32_t timeout = 0;
	uint8_t result = 2;
	uint32_t time_limit = 1000000;
	UART3_bufPtr = 0;
	memset(UART3_recBuf, 0, strlen(UART3_recBuf));
	memset(cmd_buf, 0, 100);

	sprintf(cmd_buf, "AT+%s%s\r\n", AT_cmd, AT_parameter);
	UART3_send(cmd_buf);
	printf("send to module: %s\r\n", cmd_buf);

	if (strstr(cmd_buf, "NRB") != NULL) {
		time_limit = 100000000;
	}

	while (timeout <= time_limit) {

		timeout++;
		if (UART3_strReady) {
			//UART3_strReady = 0;
			if (strstr(UART3_recBuf, AT_exptAnswer) != NULL) { //if received string contains "\r\n"
				printf(UART3_recBuf);

				result = 0;

				break;
			} else if (strstr(UART3_recBuf, "ERROR") != NULL) {

				result = 1;

				break;
			}
			UART3_strReady = 0;
		}
	}
	UART3_bufPtr = 0;
	memset(UART3_recBuf, 0, strlen(UART3_recBuf));

	return result;
}

uint8_t NB_setPin() {

	return AT_send(AT_NPIN, "", "+NPIN: \"OK\"");
	//delay(2000000);
	//AT_send(AT_COPS, "");

	//char cmd_buf[100];
	//sprintf(cmd_buf, "%s\r\n", AT_NPIN);
	//printf(cmd_buf);

}
uint8_t NB_connectStatus() {

	uint8_t res;

	/*
	 res = AT_send(AT_COPS, "=?", "OK");
	 if (res == 0) {
	 printf("OP scan ok\r\n");
	 } else if (res == 1) {
	 printf("error\r\n");
	 }
	 */
	delay(2200000);  //viivettä pitää olla
	res = AT_send(AT_COPS, "=0", "OK");
	if (res == 0) {
		printf("Elisa\r\n");
	} else if (res == 1) {
		printf("error\r\n");
	}

	res = 2;
	while (res != 0) {
		res = AT_send(AT_CEREG, "", "+CEREG: 0,1");
		if (res == 0) {
			printf("CEREG_OK\r\n");
		} else if (res == 1) {
			printf("error\r\n");
		}
	}
	res = AT_send(AT_CGDCONT, "", "OK");
	delay(2200000);
	if (res == 0) {
		printf("PDP context 1 defined\r\n");
	} else if (res == 1) {
		printf("error\r\n");
	}

	res = AT_send(AT_COPS, "=2", "OK");     //De-register from network
	delay(2200000);
	if (res == 0) {
		printf("De-registered\r\n");
	} else if (res == 1) {
		printf("error\r\n");
	}

	res = AT_send(AT_CGACT, "=1,1", "OK");     //Active PDP context 1
	delay(22000000);
	if (res == 0) {
		printf("PDP 1 activated\r\n");
	} else if (res == 1) {
		printf("error\r\n");
	}

	res = AT_send(AT_CGPADDR, "", "OK");     //Show ip address

	if (res == 0) {
		printf("ip  found\r\n");
	} else if (res == 1) {
		printf("error\r\n");
	}
	delay(2000000);
	res = AT_send(AT_NSOCR, "", "OK");     //Create UDP socket

		if (res == 0) {
			printf("Socket ready\r\n");
		} else if (res == 1) {
			printf("error\r\n");
		}

			delay(20000000);
		res = AT_send(AT_NSOST, "", "OK");     //Send message to server

			if (res == 0) {
				printf("msg sent\r\n");
			} else if (res == 1) {
				printf("error\r\n");
			}
			delay(20000000);
			res = AT_send(AT_NSORF, "", "OK");     //read echo data

						if (res == 0) {
							printf("echo\r\n");
						} else if (res == 1) {
							printf("error\r\n");
						}

	return 0;
}


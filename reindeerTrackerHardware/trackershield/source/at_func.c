#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "at_func.h"
#include <string.h>

extern volatile uint8_t UART3_strReady;
extern volatile uint8_t UART3_bufPtr;
extern void UART3_send(char* data);

extern char UART3_recBuf[50];

char AT_REQ[] = "AT";
char AT_GPSDBG_ON[] = "CGNSDBG=1";
char AT_GPSTST_ON[] = "CGNSTST=1";
char AT_GPSINFO[] = "CGNSINF";

char AT_NPIN[] = "CPIN=";  // sets, the PIN-code
char AT_CMEE[] = "CMEE=";
char AT_CFUN[] = "CFUN"; 	// sets functionality mode, edit this to change power mode
char AT_CCLK[] = "CCLK";	// sets and reads rtc example: AT+CCLK="14/07/01,15:00:00+01"
char AT_CREG[] = "CREG";
char AT_CGATT[] = "CGATT?";
char AT_CSTT[] = "CSTT=\"apn\",\"\",\"\"";
char AT_CIICR[] = "CIICR";
char AT_CIFSR[] = "CIFSR";
char AT_CIPSTATUS[] = "CIPSTATUS";
char AT_CIPSTART[] = "CIPSTART=\"TCP\",\"139.59.155.145\",1883";
char AT_CIPSEND[] = "CIPSEND=";

void NB_init() {
	char cmd_buf[50];

	sprintf(cmd_buf, "AT+%s\r\n", AT_CMEE);

	printf(cmd_buf);
}

void AT_send(char *AT_cmd, char *AT_parameter) {

	char cmd_buf[100];
	memset(cmd_buf, 0, 100);
	sprintf(cmd_buf, "AT+%s%s\r\n", AT_cmd, AT_parameter);
	UART3_send(cmd_buf);

	while (1) {

			  if(UART3_strReady) {
				  printf(UART3_recBuf);
				  UART3_strReady = 0;

				  if(strstr(UART3_recBuf, "OK") != NULL) {	//if received string contains "\r\n"
				  	  memset(UART3_recBuf, 0, strlen(UART3_recBuf));

					  break;
				  }
			  }
		  }

}

void NB_setPin(char* pinCode) {

	char cmd_buf[100];

	sprintf(cmd_buf, "%s2,\"%s\"\r\n", AT_NPIN, pinCode);

	printf(cmd_buf);

}
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "at_func.h"
#include <string.h>
#include "timing.h"

extern void NB_send(char* data);

extern uint8_t PCprint(char *data);

extern volatile uint8_t NB_strReady;

extern char NB_recBuf[];
extern volatile uint16_t NB_bufPtr;

char* AT_REQ = "AT";

char* AT_CGMI = "CGMI";  // check manufacturer
char* AT_NPIN = "NPIN=0,\"1234\"";  // sets, the PIN-code
char* AT_COPS = "COPS";            //Register to operator network
char* AT_CEREG = "CEREG?";  //connection status
char* AT_NRB = "NRB";
char* AT_CGDCONT = "CGDCONT=1,\"IP\",\"\",\"\"";
char* AT_CGACT = "CGACT";
char* AT_CSQ = "CSQ";  //Signal quality
char* AT_CGPADDR = "CGPADDR";        //shows module ip address
char* AT_NSOCR = "NSOCR=\"DGRAM\",17,42000,1"; //create UDP socket, port 420000
//char AT_NSOST[] = "NSOST=0,\"195.34.89.241\",7,2,\"f8f8\"";
char* AT_NSOST = "NSOST=";

char* AT_NSORF = "NSORF=0,2";    //show received data, =<socket>, <data_length>
char* AT_CMEE = "CMEE=";
char* AT_CFUN = "CFUN"; // sets functionality mode, edit this to change power mode

char* AT_CCLK = "CCLK"; // sets and reads rtc example: AT+CCLK="14/07/01,15:00:00+01"

/*
 * Small function to check if NBiot receive buffer contains OK
 * we can use this to break from response waiting loop if we already got OK answer
 */
uint8_t breakIfAtOk() {
	if (strstr(NB_recBuf, "OK") != NULL) {
		return 1;
	}
	return 0;
}

/*
 * Check AT command return result and print related ok message or error message.
 *
 * res = AT command return code. 0 = command succeeded, 1 = module responded with ERROR, 2 = there was no answer from module
 * *subject = string that specifies the command
 */

void AT_checkResult(uint8_t res, char *subject) {

	if (res == 0) {
		//PCprint("%s OK\r\n", subject);
	} else if (res == 1) {
		//PCprint("%s ERROR\r\n", subject);
	} else {
		//PCprint("%s NO RESPONSE\r\n", subject);
	}
}

/*
 *
 * Send AT command to module.
 * Parameter:
 *
 * *AT_cmd = pointer to AT command suffix (the part after "AT+"
 * *AT_parameter = parameters to add to the command
 * *AT_exptAnswer = expected answer string that module should answer to the command
 *
 */
uint8_t AT_send(char *AT_cmd, char *AT_parameter, char *AT_exptAnswer) {

	char cmd_buf[500];
	uint8_t result = 2;
	uint32_t time_limit = 1500;

	NB_bufPtr = 0;

	sprintf(cmd_buf, "AT+%s%s\r\n", AT_cmd, AT_parameter);
	NB_send(cmd_buf);

	PCprint("send to module: \r\n");
	PCprint(cmd_buf);

	if (strstr(cmd_buf, "NRB") != NULL) {
		time_limit = 10000;
	}

	time_limit = millis() + time_limit;
	while (millis() < time_limit) {
		if (NB_strReady) {

			PCprint(NB_recBuf);
			if (strstr(NB_recBuf, AT_exptAnswer) != NULL) //if received buffer contains expected answer
			{

				result = 0;
				break;
			} else if (strstr(NB_recBuf, "ERROR") != NULL) {

				result = 1;
				break;
			}
			NB_strReady = 0;
		}
	}
	NB_bufPtr = 0;
	memset(NB_recBuf, 0, 600);

	return result;
}

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "at_func.h"
#include <string.h>

extern void UART3_send(char* data);
extern uint8_t UART3_receive();
extern void delay_ms();
extern volatile uint8_t UART3_strReady;

extern char UART3_recBuf[1000];
extern volatile uint16_t UART3_bufPtr;

char AT_REQ[] = "AT";

char AT_CGMI[] = "CGMI";  // check manufacturer
char AT_NPIN[] = "NPIN=0,\"1234\"";  // sets, the PIN-code
char AT_COPS[] = "COPS";            //Register to operator network
char AT_CEREG[] = "CEREG?";  //connection status
char AT_NRB[] = "NRB";
char AT_CGDCONT[] = "CGDCONT=1,\"IP\",\"\",\"\"";
char AT_CGACT[] = "CGACT";
char AT_CSQ[] = "CSQ";  //Signal quality
char AT_CGPADDR[] = "CGPADDR";        //shows module ip address
char AT_NSOCR[] = "NSOCR=\"DGRAM\",17,42000,1"; //create UDP socket, port 420000
char AT_NSOST[] = "NSOST=0,\"195.34.89.241\",7,2,\"F8F8\"";
char AT_CMEE[] = "CMEE=";

char AT_NSORF[] = "NSORF=0,2";    //show received data, =<socker>, <data_lenght>
char AT_CFUN[] = "CFUN"; // sets functionality mode, edit this to change power mode
char AT_CCLK[] = "CCLK"; // sets and reads rtc example: AT+CCLK="14/07/01,15:00:00+01"

void NB_init()
{
	char cmd_buf[50];
	sprintf(cmd_buf, "AT+%s\r\n", AT_CMEE);
	printf(cmd_buf);
}

/*
 * Small function to check if NBiot receive buffer contains OK
 * we can use this to break from response waiting loop if we already got OK answer
 */
uint8_t breakIfAtOk()
{
	if(strstr(UART3_recBuf,"OK") != NULL)
	{
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

void AT_checkResult(uint8_t res, char *subject)
{

	if (res == 0)
	{
		printf("%s OK\r\n",subject);
	}
	else if (res == 1)
	{
		printf("%s ERROR\r\n", subject);
	}
	else
	{
		printf("%s NO RESPONSE\r\n",subject);
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
uint8_t AT_send(char *AT_cmd, char *AT_parameter, char *AT_exptAnswer)
{

	char cmd_buf[100];

	uint8_t result = 2;
	uint32_t time_limit = 1000000;
	UART3_bufPtr = 0;
	memset(UART3_recBuf, 0, strlen(UART3_recBuf));
	memset(cmd_buf, 0, 100);

	sprintf(cmd_buf, "AT+%s%s\r\n", AT_cmd, AT_parameter);
	UART3_send(cmd_buf);
	printf("send to module: %s\r\n", cmd_buf);

	if (strstr(cmd_buf, "NRB") != NULL)
	{
		time_limit = 100000000;
	}

	while (time_limit--)
	{

		if (UART3_strReady)
		{
			//UART3_strReady = 0;
			if (strstr(UART3_recBuf, AT_exptAnswer) != NULL) //if received buffer contains expected answer
			{
				printf(UART3_recBuf);
				result = 0;
				break;
			}
			else if (strstr(UART3_recBuf, "ERROR") != NULL)
			{

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

uint8_t NB_setPin()
{

	return AT_send(AT_NPIN, "", "+NPIN: \"OK\"");

}
uint8_t NB_connectStatus()
{

	uint8_t res;

	/*
	 res = AT_send(AT_COPS, "=?", "OK");
	 if (res == 0) {
	 printf("OP scan ok\r\n");
	 } else if (res == 1) {
	 printf("error\r\n");
	 }
	 */
	delay_ms(1000);  //viivettä pitää olla
	res = AT_send(AT_COPS, "=0", "OK");

	AT_checkResult(res, "Start operator search");

	res = 2;
	while (res != 0)
	{
		res = AT_send(AT_CEREG, "", "+CEREG: 0,1");
		if (res == 0)
		{
			printf("Network registration OK\r\n");
		}
		else if (res == 1)
		{
			printf("CEREG error\r\n");
		}
	}

	res = AT_send(AT_CGDCONT, "", "OK");
	delay_ms(1000);

	AT_checkResult(res, "PDP context definition");


	res = AT_send(AT_COPS, "=2", "OK");     //De-register from network
	delay_ms(1000);
	AT_checkResult(res, "De-registration");

	res = AT_send(AT_CGACT, "=1,1", "OK");     //Active PDP context 1
	delay_ms(1000);
	AT_checkResult(res, "PDP context activation");


	res = AT_send(AT_CGPADDR, "", "OK");     //Show ip address
	AT_checkResult(res, "Get IP address");

	delay_ms(1000);

	res = AT_send(AT_NSOCR, "", "OK");     //Create UDP socket

<<<<<<< HEAD
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
=======
	AT_checkResult(res, "Create socket");

	delay_ms(1000);
	res = AT_send(AT_NSOST, "", "OK");     //Send message to server

	AT_checkResult(res, "Send UDP packet");

	delay_ms(1000);
	res = AT_send(AT_NSORF, "", "OK");     //read echo data

	AT_checkResult(res, "Read echo");

>>>>>>> 4deff0ee02916f1afc30955ec12fb20a83cd7cd4

	return 0;
}


/*
 * nbiot_func.c
 *
 *  Created on: Mar 16, 2018
 *      Author: teemu
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nbiot_func.h"

const char postHeader[] = "POST /Reindeertracker/API/data/index.php HTTP/1.1\r\n"
"Host: 168.235.64.81\r\n"
"Content-Length: %d\r\n"
"Content-Type: application/json\r\n"
"Cache-Control: no-cache\r\n\r\n";




void assemblePacket(reindeerData_t *reindeerData, char *udpMessage)
{

	int length = 0;

	static char jsonMessage[150];

	length += sprintf(jsonMessage, "{\r\n\r\n    \"serialnumber\":\"%s\",\r\n",reindeerData->serialNum);
	length += sprintf(jsonMessage+length, "    \"lat\":\"%s\",\r\n",reindeerData->latitude);
	length += sprintf(jsonMessage+length, "    \"long\":\"%s\",\r\n",reindeerData->longitude);
	length += sprintf(jsonMessage+length, "    \"status\":\"%s\",\r\n",reindeerData->dead);
	length += sprintf(jsonMessage+length, "    \"battery\":\"%d\"\r\n\r\n}",reindeerData->batteryLevel);

	printf(jsonMessage);

	//strcpy(udpMessage,postHeader);

	int udpLength = sprintf(udpMessage,postHeader,length);

	strcpy(udpMessage+udpLength, jsonMessage);

	printf("%s\r\n",udpMessage);

	udpLength = strlen(udpMessage);

	printf("length of udp msg %d \r\nPrinting UDP message in hex\r\n",udpLength);

	for(uint16_t p=0;p<udpLength;p++)
	{

		printf("%02x",udpMessage[p]);
	}

	printf("\r\n");

}



/*
void NB_setPin(char* pinCode) {

	char cmd_buf[100];

	sprintf(cmd_buf, "%s2,\"%s\"\r\n", AT_NPIN, pinCode);

	printf(cmd_buf);

}
*/

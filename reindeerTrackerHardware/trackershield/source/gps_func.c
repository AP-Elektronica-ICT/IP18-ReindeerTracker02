/*
 * gps_func.c
 *
 *  Created on: Mar 15, 2018
 *      Author: nks
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "gps_func.h"
#include "at_func.h"

void activateGPS() {

	char* token;
	const char s[2] = ",";
	uint8_t counter = 0;

	char GLL_ID[10];	// GLL Message ID $GPGLL
	char latitude[15];
	char northSouth[2];	// Indicates North / South, N = north, S = south
	char longitude[15];
	char eastWest[2];	// Indicates East / West, E = east, W = west
	char time[10];		// UTC Time
	char status[2];	// Status indicates data validity V = Data invalid or receiver warning, A = data valid
	char posMode[2];	// Positioning mode according to NMEA protocol
	char checkSum[10];

	char* GPS_dataPtrs[9] = { GLL_ID, latitude, northSouth, longitude, eastWest,
			time, status, posMode, checkSum };

	AT_send(AT_CGPS, "1");

	while (strstr(GPS_dataPtrs[6], "A") == NULL) {		// Loop until string contains A status, A = Data valid

		while (UART3_receive() == 0) { 	// Wait for a string from GPS module

		}


		if (strstr(UART3_recBuf, "$GPGLL") != NULL) {		// Check if received message is GPGLL message

			token = strtok(UART3_recBuf, s);				// Strtok splices the string to different variables, using "," separator

			while (token != NULL) {

				GPS_dataPtrs[counter] = token;				// Save the splices contents to different variables
				token = strtok(NULL, s);
				counter++;

			}

			if (strstr(GPS_dataPtrs[6], "V") != NULL) {
				printf("Data invalid, waiting for valid data\r\n");
			}

			else {

				for (uint8_t cnr = 0; cnr <= 8; cnr++) {
					printf("%s\r\n", GPS_dataPtrs[cnr]);
				}
			}

			memset(UART3_recBuf, 0, strlen(UART3_recBuf));

		}

		UART3_strReady = 0;
	}
}

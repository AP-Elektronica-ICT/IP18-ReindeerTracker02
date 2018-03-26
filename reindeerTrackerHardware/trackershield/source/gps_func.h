/*
 * gps_func.h
 *
 *  Created on: Mar 15, 2018
 *      Author: nks
 */

#ifndef SOURCE_GPS_FUNC_H_
#define SOURCE_GPS_FUNC_H_


void getGPS();


void activateGPS();
extern char UART3_recBuf[1000];
extern uint8_t UART3_receive();
extern volatile uint8_t UART3_strReady;
void parseData(char* latStr, char* lonStr);
extern char parsedLat[15];
extern char parsedLon[15];

uint8_t calcUbxCrc(char *data);

void printUbxResponseHex(char* data, uint8_t dataLength);



#endif /* SOURCE_GPS_FUNC_H_ */

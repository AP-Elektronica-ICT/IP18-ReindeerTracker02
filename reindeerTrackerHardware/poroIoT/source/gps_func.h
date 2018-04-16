/*
 * gps_func.h
 *
 *  Created on: Mar 15, 2018
 *      Author: nks
 */

#ifndef SOURCE_GPS_FUNC_H_
#define SOURCE_GPS_FUNC_H_


uint8_t getGPS();

void GPS_send(char *data, uint8_t len);

void activateGPS();
extern char GPS_recBuf[600];
extern uint8_t UART3_receive();
extern volatile uint8_t UART3_strReady;
void parseData(char* latStr, char* lonStr);
extern char parsedLat[15];
extern char parsedLon[15];

uint8_t calcUbxCrc(char *data);

void printUbxResponseHex(char* data, uint8_t dataLength);



#endif /* SOURCE_GPS_FUNC_H_ */

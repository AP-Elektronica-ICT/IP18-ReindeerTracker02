/*
 * gps_func.h
 *
 *  Created on: Mar 15, 2018
 *      Author: nks
 */

#ifndef SOURCE_GPS_FUNC_H_
#define SOURCE_GPS_FUNC_H_

void getGPS();
extern char UART3_recBuf[50];
extern uint8_t UART3_receive();
extern volatile uint8_t UART3_strReady;
void parseData(char* latStr, char* lonStr);
extern char parsedLat[15];
extern char parsedLon[15];



#endif /* SOURCE_GPS_FUNC_H_ */

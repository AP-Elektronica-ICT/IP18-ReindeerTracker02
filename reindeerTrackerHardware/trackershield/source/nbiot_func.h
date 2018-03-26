/*
 * nbiot_func.h
 *
 *  Created on: Mar 16, 2018
 *      Author: teemu
 */

#ifndef SOURCE_NBIOT_FUNC_H_
#define SOURCE_NBIOT_FUNC_H_

extern const char postHeader[];

typedef struct reindeerData_t {

	char serialNum[6];
	char latitude[9];
	char longitude[9];
	char dead[6];
	uint8_t batteryLevel;
} reindeerData_t;

void assemblePacket(reindeerData_t *reindeerData, char *udpMessage);


#endif /* SOURCE_NBIOT_FUNC_H_ */

/*
 * nbiot_func.h
 *
 *  Created on: Mar 16, 2018
 *      Author: teemu
 */

#ifndef SOURCE_NBIOT_FUNC_H_
#define SOURCE_NBIOT_FUNC_H_

extern uint8_t res;

uint8_t NB_setPin();
void NB_reboot();
void NB_cops_register();
void NB_cops_deRegister();
void NB_cops_readRegister();
uint8_t NB_network_status();
void NB_define_pdp();
void NB_active_pdp();
void NB_show_ip();
void NB_create_socket();
uint8_t NB_send_msg(char *address, char *mqttMessage, uint8_t msgLen);
void NB_read_msg();
void NB_create_pdp_send(char *address, char *mqttMessage, uint8_t msgLen);
void NB_received_data();

void NB_send(char *data);

extern const char postHeader[];

typedef struct reindeerData_t {

	char serialNum[6];
	char latitude[12];
	char longitude[12];
	char dead[6];
	uint8_t batteryLevel;
	int8_t temperature;
} reindeerData_t;

void assemblePacket(reindeerData_t *reindeerData, char *udpMessage);
uint8_t assembleMqtt(reindeerData_t *reindeerData, char *udpMessage, char *teunMessage);

#endif /* SOURCE_NBIOT_FUNC_H_ */

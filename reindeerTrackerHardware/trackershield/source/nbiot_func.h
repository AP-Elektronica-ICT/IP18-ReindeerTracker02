/*
 * nbiot_func.h
 *
 *  Created on: Mar 16, 2018
 *      Author: teemu
 */

#ifndef SOURCE_NBIOT_FUNC_H_
#define SOURCE_NBIOT_FUNC_H_

extern uint8_t res;

void NB_setPin();
void NB_reboot();
void NB_cops_register();
void NB_cops_deRegister();
void NB_cops_readRegister();
void NB_network_status();
void NB_define_pdp();
void NB_active_pdp();
void NB_show_ip();
void NB_create_socket();
void NB_send_msg();
void NB_read_msg();
void NB_create_pdp_send();
void NB_received_data();

extern const char postHeader[];

typedef struct reindeerData_t {

	char serialNum[6];
	char latitude[9];
	char longitude[9];
	char dead[6];
	uint8_t batteryLevel;
} reindeerData_t;

void assemblePacket(reindeerData_t *reindeerData, char *udpMessage);
void assembleMqtt(reindeerData_t *reindeerData, char *udpMessage);

#endif /* SOURCE_NBIOT_FUNC_H_ */

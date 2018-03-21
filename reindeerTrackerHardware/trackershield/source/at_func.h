/*
 * at_commands.h
 *
 *  Created on: 8.12.2017
 *      Author: pauli
 */

#ifndef AT_COMMANDS_H_
#define AT_COMMANDS_H_

#define DEF_TIMEOUT 5

extern char AT_REQ[];
extern char AT_GPSDBG_ON[];
extern char AT_GPSTST_ON[];
extern char AT_GPSINFO[];

extern char AT_NPIN[];
extern char AT_CMEE[];
extern char AT_CREG[];
extern char AT_CGATT[];
extern char AT_CSTT[];
extern char AT_CIICR[];
extern char AT_CIPSTATUS[];
extern char AT_CIPSTART[];
extern char AT_CIFSR[];
extern char AT_CIPSEND[];
extern char AT_CGPS[];

void AT_send(char* AT_cmd, char* AT_parameter);
void NB_setPin(char* pinCode);
void NB_init();

#endif /* AT_COMMANDS_H_ */

/*int *GPS_comm_table[] =
{
	&AT_GPSDBG_ON[0],
	&AT_GPSTST_ON[0]
};*/
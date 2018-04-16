/*
 * at_commands.h
 *
 *  Created on: 8.12.2017
 *      Author: pauli
 */

#ifndef AT_COMMANDS_H_
#define AT_COMMANDS_H_

#define DEF_TIMEOUT 5

extern char* AT_REQ;

extern char* AT_COPS;
extern char* AT_CGMI;
extern char* AT_NPIN;
extern char* AT_CEREG;
extern char* AT_NRB;
extern char* AT_CGDCONT;
extern char* AT_CGACT;
extern char* AT_CGPADDR;
extern char* AT_CSQ;
extern char* AT_NSOCR;
extern char* AT_NSOST;
extern char* AT_NSORF;
extern char* AT_CMEE;

#define RESPONSE_TIMEOUT_NORMAL_VALUE 2000

uint8_t AT_send(char* AT_cmd, char* AT_parameter, char *AT_exptAnswer);


uint8_t NB_selectOp();
uint8_t NB_connectStatus();

uint8_t breakIfAtOk();

#endif /* AT_COMMANDS_H_ */


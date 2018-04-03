/*
 * ubx_func.h
 *
 *  Created on: Mar 19, 2018
 *      Author: nks
 */

#ifndef SOURCE_UBX_FUNC_H_
#define SOURCE_UBX_FUNC_H_
uint8_t fletcher8(char *crc, uint8_t len);
void ubx_send(char* ubx_cmd, uint8_t len);

#endif /* SOURCE_UBX_FUNC_H_ */

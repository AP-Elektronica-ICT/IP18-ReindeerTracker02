/*
 * i2c_func.h
 *
 *  Created on: Feb 9, 2018
 *      Author: root
 */

#ifndef SOURCE_I2C_FUNC_H_
#define SOURCE_I2C_FUNC_H_

#include <stdint.h>


uint8_t accReadReg(uint8_t reg);
void accWriteReg(uint8_t reg, uint8_t data);
void initI2C();



#endif /* SOURCE_I2C_FUNC_H_ */

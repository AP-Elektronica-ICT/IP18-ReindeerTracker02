/*
 * acc_func.h
 *
 *  Created on: Feb 9, 2018
 *      Author: nks
 */

#ifndef SOURCE_ACC_FUNC_H_
#define SOURCE_ACC_FUNC_H_
#define FXOS8700Q_CTRL_REG4 0x2D
#define FXOS8700Q_CTRL_REG5 0x2E
#define FXOS8700Q_CTRL_REG3 0x2C
#define FXOS8700Q_A_FFMT_CFG 0x15
#define FXOS8700Q_A_FFMT_THS 0x17

void acc_init();
int16_t read_acc_axis(uint8_t axis);
int16_t print_ext_acc_axis();
void configure_acc();


#endif /* SOURCE_ACC_FUNC_H_ */


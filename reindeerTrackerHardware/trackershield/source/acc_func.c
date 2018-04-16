/*
 * acc_func.c
 *
 *  Created on: Feb 9, 2018
 *      Author: nks
 */

#include <stdint.h>
#include <stdio.h>
#include "i2c_func.h"
//#define X_AXIS 	0
//#define Y_AXIS 	1
//#define Z_AXIS 	2

#include "adc_func.h"
#include "acc_func.h"

unsigned char buffer[50];

void acc_init(){
	 accWriteReg(0x2a,0x1c); //write accelerometer CTRL_REG1 (active mode)
	 accWriteReg(0x2a,0x1d); //write accelerometer CTRL_REG1 (active mode)
	 accWriteReg(0x5B,0x03); //Acc temperature sensor enable

}

void configure_acc() {

	uint8_t tmp = 0;

	tmp = accReadReg(FXOS8700Q_CTRL_REG4); //read old contents of CTRL REG 4 to tmp
	tmp |= 0x84; //make changes to it
	accWriteReg(FXOS8700Q_CTRL_REG4, tmp); //write it back

	tmp = accReadReg(FXOS8700Q_CTRL_REG5);
	tmp |= 0x04;
	accWriteReg(FXOS8700Q_CTRL_REG5, tmp);

	tmp = accReadReg(FXOS8700Q_CTRL_REG3);
	tmp |= 0x08;
	accWriteReg(FXOS8700Q_CTRL_REG3, tmp);

  tmp = accReadReg( FXOS8700Q_A_FFMT_THS);

  tmp |= 0x81;
  accWriteReg(FXOS8700Q_A_FFMT_THS, tmp);


	tmp = accReadReg( FXOS8700Q_A_FFMT_THS);
	tmp |= 0x8F;
	accWriteReg(FXOS8700Q_A_FFMT_THS, tmp);

  //accWriteReg(0x0f, 0x20);


}

int16_t read_acc_axis(uint8_t axis) {
	int8_t acc_temp = 0;

	switch (axis) {
	case 0:
		axis = 0x01;
		break;

	case 1:
		axis = 0x03;
		break;

	case 2:
		axis = 0x05;
		break;

	case 3:
		acc_temp = accReadReg(0x51);
		return (int16_t) acc_temp;
		break;

	}

	uint16_t acc_val = 0; //init a 16-bit variable to store 14-bit acceleration value

	uint8_t acc_buf = accReadReg(axis); //read MSB bits of acceleration value

	acc_val = acc_buf; //read MSB bits to the 16 bit variable

	//0000 0000 MMMM MMMM at this stage acc_val looks like this in memory

	acc_val <<= 8; // shift MSB bits left to have them in right place

	//MMMM MMMM 0000 0000

	acc_buf = accReadReg(axis + 1); //read LSB values to buffer
	acc_val |= acc_buf; //OR LSB values to the acc_value

	//MMMM MMMM LLLL LL00
	acc_val >>= 2; //shift right 2 bits to right-justify

	//00MM MMMM MMLL LLLLpaskaa

	int16_t out = 0;

	if (acc_val & (1 << 13)) //test if value is negative by masking 14th bit
			{
		acc_val = 0xffff - acc_val + 1;
		out = 0 - (acc_val & 0x1fff);

	}

	else {
		out = acc_val & 0x1fff;
	}
	return out;
}


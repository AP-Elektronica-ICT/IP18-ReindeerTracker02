/*
 * i2c_func.c
 *
 *  Created on: Feb 9, 2018
 *      Author: root
 */

#include "i2c_func.h"
#include <stdint.h>
#include "fsl_i2c.h"
#include "fsl_port.h"

#define FRDM_ACC_SADDR 0x1D
#define TRACKER_ACC_SADDR 0x1C

#define ACC_SADDR FRDM_ACC_SADDR

void initI2C()
{

	CLOCK_EnableClock(kCLOCK_I2c0);
	CLOCK_EnableClock(kCLOCK_PortE);
	i2c_master_config_t config;

	uint32_t ClkSrcFreq = CLOCK_GetCoreSysClkFreq();

	I2C_MasterGetDefaultConfig(&config);

	config.enableMaster = true;
    config.enableStopHold = false;
    config.baudRate_Bps = 100000;
    config.glitchFilterWidth = 0;

	I2C_MasterInit(I2C0, &config, ClkSrcFreq);

	PORT_SetPinMux(PORTE, 24u, kPORT_MuxAlt5);
	PORT_SetPinMux(PORTE, 25u, kPORT_MuxAlt5);
}

void accWriteReg(uint8_t reg, uint8_t data)
{

	I2C_MasterStart(I2C0, ACC_SADDR, kI2C_Write);

	/* Wait until the data register is ready for transmit. */
    while (!(I2C0->S & kI2C_IntPendingFlag)){}
	/* Clear the IICIF flag. */
    I2C0->S |= kI2C_IntPendingFlag;

    //select transmit mode
    I2C0 ->C1 |= I2C_C1_TX_MASK;

    I2C0 ->D = reg;
    /* Wait until data transfer complete. */
	while (!(I2C0->S & kI2C_IntPendingFlag))
	{
	}
	I2C0->S |= kI2C_IntPendingFlag;

	I2C0 ->D = data;
    /* Wait until data transfer complete. */
	while (!(I2C0->S & kI2C_IntPendingFlag))
	{
	}
	I2C0->S |= kI2C_IntPendingFlag;


	I2C_MasterStop(I2C0);
}

uint8_t accReadReg(uint8_t reg)
{
	uint8_t data = 0;

	I2C_MasterStart(I2C0, ACC_SADDR, kI2C_Write); //do START condition and send slave address

	/* Wait until the data register is ready for transmit. */
    while (!(I2C0->S & kI2C_IntPendingFlag)){}
	/* Clear the IICIF flag. */
    I2C0->S |= kI2C_IntPendingFlag;

    //select transmit mode
    I2C0 ->C1 |= I2C_C1_TX_MASK;

    I2C0 ->D = reg;
    /* Wait until data transfer complete. */
	while (!(I2C0->S & kI2C_IntPendingFlag))
	{
	}
	I2C0->S |= kI2C_IntPendingFlag;

	I2C_MasterRepeatedStart(I2C0, ACC_SADDR, kI2C_Read);

	while (!(I2C0->S & kI2C_IntPendingFlag))  {}
	/* Clear the IICIF flag. */
    I2C0->S = kI2C_IntPendingFlag;

    /* Setup the I2C peripheral to receive data. */
    I2C0->C1 &= ~(I2C_C1_TX_MASK | I2C_C1_TXAK_MASK);
    I2C0 ->C1 |= I2C_C1_TXAK_MASK; //set TXAK to send NACK after receiving byte

    data = I2C0 ->D; //dummy read to initiate receiving

    while (!(I2C0->S & kI2C_IntPendingFlag)){}
    /* Clear the IICIF flag. */
    I2C0->S = kI2C_IntPendingFlag;

    /*
     * Send STOP before reading received byte. Apparently Kinetis I2C
     * will signal the slave to send a next data byte when the data register is read,
     * so the slave device will jam the bus if we read it too early and do not read any more bytes
     */
    I2C_MasterStop(I2C0);

	data = I2C0 ->D; //read the received byte
	return data;

}

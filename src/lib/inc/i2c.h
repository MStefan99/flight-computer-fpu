/* 
 * File:   i2c.h
 * Author: mikha
 *
 * Created on November 23, 2021, 5:12 PM
 */

#ifndef I2C_H
#define	I2C_H


#include "device.h"

#include "lib/inc/tl/allocator.hpp"
#include "lib/inc/util.h"
#include "lib/inc/dma.h"


namespace i2c {
	void init();

	void write(uint8_t devAddr, uint8_t* buf, uint8_t size = 1);
	void read(uint8_t devAddr, uint8_t* buf, uint8_t size = 1);

	void writeRegister(uint8_t devAddr, uint8_t regAddr, uint8_t* buf, uint8_t size = 1);
	void readRegister(uint8_t devAddr, uint8_t regAddr, uint8_t* buf, uint8_t size = 1);
}


#endif	/* I2C_H */

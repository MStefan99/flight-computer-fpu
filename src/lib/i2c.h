/* 
 * File:   i2c.h
 * Author: mikha
 *
 * Created on November 23, 2021, 5:12 PM
 */

#ifndef I2C_H
#define	I2C_H


#include <cstring>
#include "device.h"
#include "lib/list.h"
#include "lib/dma.h"


namespace i2c {
	void init();
	
	void write(uint8_t devAddr, uint8_t* buf, uint8_t size = 1);
	void read(uint8_t devAddr, uint8_t* buf, uint8_t size = 1);
	
	void writeRegister(uint8_t devAddr, uint8_t regAddr, uint8_t* buf, uint8_t size = 1);
	void readRegister(uint8_t devAddr, uint8_t regAddr, uint8_t* buf, uint8_t size = 1);
	
	// If queue is used, startTransfer should be polled at regular intervals
	void startTransfer();
}


#endif	/* I2C_H */


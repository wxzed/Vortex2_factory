
#ifndef I2C_MODE_H__
#define I2C_MODE_H__
#include "stdint.h"

void twi_init (void);
void i2cWriteBytes(uint8_t addr_t, uint8_t Reg , uint8_t *pdata, uint8_t datalen );
uint16_t i2cReadBytes(uint8_t addr_t, uint8_t Reg ,uint8_t Num );
void i2c_beginTransmission(uint8_t address);
void i2c_endTransmission();
void writebyte(uint8_t data);
uint8_t i2c_read();
uint8_t i2c_requestFrom(uint8_t address, uint8_t size);


#endif 

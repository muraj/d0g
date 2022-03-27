#ifndef __I2C_ADAPTER_H__
#define __I2C_ADAPTER_H__

#include <stdint.h>
#include <stddef.h>

int i2c_create_adapter(unsigned adapter);
int i2c_read(int adapter, uint16_t address, uint8_t reg, void *buf, size_t len);
int i2c_write(int adapter, uint16_t address, uint8_t reg, void *buf, size_t len);

#define i2c_readReg(adapter, addr, reg, x) i2c_read(adapter, addr, reg, (x), sizeof(*(x)))
#define i2c_writeReg(adapter, addr, reg, x) i2c_write(adapter, addr, reg, (x), sizeof(*(x)))

#endif // __I2C_ADAPTER_H__

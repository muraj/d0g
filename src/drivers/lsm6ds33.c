#include <unistd.h>

#include "i2c_adapter.h"
#include "utils.h"
#include "bitfields.h"
#include "drivers/lsm6ds33.h"
#include "drivers/lsm6ds33_regs.h"

int lsm6ds33_init(lsm6ds33 *dev)
{
    uint8_t data;
    if (i2c_readReg(dev->adapter, dev->address, LSM6DS3_WHO_AM_I_REG, &data) != 0) {
        return -1;
    }
    if (data != LSM6DS3_CHIP_ID) {
        return -1;
    }

    // Reset the chip
    data = BIT_ENCODE(LSM6DS3_CTRL3_SW_RST, 1);
    if (i2c_writeReg(dev->adapter, dev->address, LSM6DS3_CTRL3_C, &data) != 0) {
        return -1;
    }

    // Wait for the chip to come back up
    do {
        sleep(1);
        if(i2c_readReg(dev->adapter, dev->address, LSM6DS3_CTRL3_C, &data) != 0) {
            return -1;
        }
    } while((BIT_VALUE(LSM6DS3_CTRL3_SW_RST, data)) == 2);

    // TODO make configurable
    // Set the accel data rate
    data = BIT_ENCODE(LSM6DS3_CTRL1_ODR, LSM6DS3_CTRL1_ODR_104HZ) |
           BIT_ENCODE(LSM6DS3_CTRL1_FS, LSM6DS3_CTRL1_FS_4G) |
           BIT_ENCODE(LSM6DS3_CTRL1_BW, LSM6DS3_CTRL1_BW_400HZ);
    if (i2c_writeReg(dev->adapter, dev->address, LSM6DS3_CTRL1_XL, &data) != 0) {
        return -1;
    }
    // Set the gyro data rate
    data = BIT_ENCODE(LSM6DS3_CTRL2_ODR, LSM6DS3_CTRL2_ODR_104HZ) | 
           BIT_ENCODE(LSM6DS3_CTRL2_FS, LSM6DS3_CTRL2_FS_250DPS);
    if (i2c_writeReg(dev->adapter, dev->address, LSM6DS3_CTRL2_G, &data) != 0) {
        return -1;
    }
    dev->gyro_scale = 0.00875f;

    // Set the block data update to change only when read
    // Enable auto-inc on multi-byte access (block reads)
    data = BIT_ENCODE(LSM6DS3_CTRL3_IF_INC, 1);
    if (i2c_writeReg(dev->adapter, dev->address, LSM6DS3_CTRL3_C, &data) != 0) {
        return -1;
    }

    return 0;
}

int lsm6ds33_sample(lsm6ds33 *dev, float gyro[3], float accel[3], float tempC[1])
{
    struct Data {
        int16_t temp;
        int16_t gyro[3];
        int16_t accel[3];
    } measurement;

    CASSERT(offsetof(struct Data, temp) == 0);
    CASSERT(offsetof(struct Data, gyro) == 2);
    CASSERT(offsetof(struct Data, accel) == 8);

    if (i2c_readReg(dev->adapter, dev->address, LSM6DS3_OUT_TEMP_L, &measurement) != 0) {
        return -1;
    }

    gyro[0] = measurement.gyro[0] * dev->gyro_scale;
    gyro[1] = measurement.gyro[1] * dev->gyro_scale;
    gyro[2] = measurement.gyro[2] * dev->gyro_scale;

    accel[0] = (atan2(measurement.accel[1], measurement.accel[2])+M_PI)*RAD_TO_DEG;
    accel[1] = (atan2(measurement.accel[2], measurement.accel[0])+M_PI)*RAD_TO_DEG;
    accel[2] = 0.0f;

    tempC[0] = 0.0f; // TODO

    return 0;
}

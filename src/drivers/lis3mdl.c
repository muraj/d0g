#include <unistd.h>

#include "i2c_adapter.h"
#include "utils.h"
#include "bitfields.h"
#include "drivers/lis3mdl.h"
#include "drivers/lis3mdl_regs.h"

int lis3mdl_init(lis3mdl *dev)
{
    uint8_t data;
    if (i2c_readReg(dev->adapter, dev->address, LIS3MDL_WHO_AM_I_REG, &data) != 0) {
        return -1;
    }

    if (data != LIS3MDL_CHIP_ID) {
        return -1;
    }

    // Reset the chip
    data = BIT_ENCODE(LIS3MDL_CTRL_REG2_SOFT_RST, 1);
    if (i2c_writeReg(dev->adapter, dev->address, LIS3MDL_CTRL_REG2, &data) != 0) {
        return -1;
    }

    // Wait for the chip to come back up
    do {
        sleep(1);
        if(i2c_readReg(dev->adapter, dev->address,
                       LIS3MDL_CTRL_REG2, &data) != 0)
        {
            return -1;
        }
    } while((BIT_VALUE(LIS3MDL_CTRL_REG2_SOFT_RST, data)) == 2);

    return 0;
}

int lis3mdl_sample(lis3mdl *dev, float v[3], float tempC[1])
{
    struct Data {
        int16_t gyro[3];
        int16_t temp;
    } measurement;

    CASSERT(offsetof(struct Data, gyro) == 0);
    CASSERT(offsetof(struct Data, temp) == 6);

    if (i2c_readReg(dev->adapter, dev->address,
                    LIS3MDL_REG_AUTO_INC | LIS3MDL_TEMP_OUT_L,
                    &measurement) != 0)
    {
        return -1;
    }

    return 0;
}

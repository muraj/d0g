#include "i2c_adapter.h"
#include "utils.h"
#include "bitfields.h"
#include <unistd.h>

#define LIS3MDL_CHIP_ID            0x3D

#define LIS3MDL_WHO_AM_I_REG       0X0F
// bitwise-OR this with a register address to enable auto-incrementing of the
// address for multi-address reads
#define LIS3MDL_REG_AUTO_INC       0x08

#define LIS3MDL_CTRL_REG1          0x20
#define LIS3MDL_CTRL_REG1_ST       0:0
#define LIS3MDL_CTRL_REG1_FAST_ODR 1:1
#define LIS3MDL_CTRL_REG1_DO       2:4
#define LIS3MDL_CTRL_REG1_DO_0_625HZ 0x0
#define LIS3MDL_CTRL_REG1_DO_1_25HZ  0x1
#define LIS3MDL_CTRL_REG1_DO_2_50HZ  0x2
#define LIS3MDL_CTRL_REG1_DO_5_00HZ  0x3
#define LIS3MDL_CTRL_REG1_DO_10_0HZ  0x4
#define LIS3MDL_CTRL_REG1_DO_20_0HZ  0x5
#define LIS3MDL_CTRL_REG1_DO_40_0HZ  0x6
#define LIS3MDL_CTRL_REG1_DO_80_0HZ  0x7
#define LIS3MDL_CTRL_REG1_OM       5:6
#define LIS3MDL_CTRL_REG1_OM_LPWR  0x0
#define LIS3MDL_CTRL_REG1_OM_MDWR  0x1
#define LIS3MDL_CTRL_REG1_OM_HPWR  0x2
#define LIS3MDL_CTRL_REG1_OM_UPWR  0x3
#define LIS3MDL_CTRL_REG1_TEMP_EN  7:7

#define LIS3MDL_CTRL_REG2          0x21
#define LIS3MDL_CTRL_REG2_SOFT_RST 2:2
#define LIS3MDL_CTRL_REG2_REBOOT   3:3
#define LIS3MDL_CTRL_REG2_FS       5:6
#define LIS3MDL_CTRL_REG2_FS_4G    0x0
#define LIS3MDL_CTRL_REG2_FS_8G    0x1
#define LIS3MDL_CTRL_REG2_FS_12G   0x2
#define LIS3MDL_CTRL_REG2_FS_16G   0x3

#define LIS3MDL_CTRL_REG3          0x22
#define LIS3MDL_CTRL_REG4          0x23
#define LIS3MDL_CTRL_REG4_BLE      1:1
#define LIS3MDL_CTRL_REG4_OMZ      2:3
#define LIS3MDL_CTRL_REG4_OMZ_LPWR 0x0
#define LIS3MDL_CTRL_REG4_OMZ_MPWR 0x1
#define LIS3MDL_CTRL_REG4_OMZ_HPWR 0x2
#define LIS3MDL_CTRL_REG4_OMZ_UPWR 0x3

#define LIS3MDL_CTRL_REG5          0x24
#define LIS3MDL_CTRL_REG5_BDU      6:6
#define LIS3MDL_CTRL_REG5_FAST_READ 7:7

#define LIS3MDL_STATUS_REG         0x27
#define LIS3MDL_STATUS_REG_XDA     0:0
#define LIS3MDL_STATUS_REG_YDA     1:1
#define LIS3MDL_STATUS_REG_ZDA     2:2
#define LIS3MDL_STATUS_REG_ZYXDA   3:3
#define LIS3MDL_STATUS_REG_XOR     4:4
#define LIS3MDL_STATUS_REG_YOR     5:5
#define LIS3MDL_STATUS_REG_ZOR     6:6
#define LIS3MDL_STATUS_REG_ZYXOR   7:7

#define LIS3MDL_OUT_X_L            0x28
#define LIS3MDL_OUT_X_H            0x29
#define LIS3MDL_OUT_Y_L            0x2A
#define LIS3MDL_OUT_Y_H            0x2B
#define LIS3MDL_OUT_Z_L            0x2C
#define LIS3MDL_OUT_Z_H            0x2D
#define LIS3MDL_TEMP_OUT_L         0x2E
#define LIS3MDL_TEMP_OUT_H         0x2F
#define LIS3MDL_INT_CFG            0x30
#define LIS3MDL_INT_SRC            0x31
#define LIS3MDL_THS_L              0x32
#define LIS3MDL_THS_H              0x33

typedef struct lis3mdl_t {
    int adapter;
    unsigned address;
} lis3mdl;

int lis3mdl_init(lis3mdl *dev, int adapter, unsigned address)
{
    uint8_t data;
    if (i2c_readReg(adapter, address, LIS3MDL_WHO_AM_I_REG, &data) != 0) {
        return -1;
    }

    if (data != LIS3MDL_CHIP_ID) {
        return -1;
    }

    // Reset the chip
    data = BIT_ENCODE(LIS3MDL_CTRL_REG2_SOFT_RST, 1);
    if (i2c_writeReg(adapter, address, LIS3MDL_CTRL_REG2, &data) != 0) {
        return -1;
    }

    // Wait for the chip to come back up
    do {
        sleep(1);
        if(i2c_readReg(adapter, address,
                       LIS3MDL_CTRL_REG2, &data) != 0)
        {
            return -1;
        }
    } while((BIT_VALUE(LIS3MDL_CTRL_REG2_SOFT_RST, data)) == 2);

    return 0;
}

int lis3mdl_sample(lis3mdl *dev /*vec_t *v*/)
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

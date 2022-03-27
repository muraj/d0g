#include "i2c_adapter.h"
#include "utils.h"
#include "bitfields.h"
#include <unistd.h>

#define LSM6DS3_CHIP_ID            0x69

#define LSM6DS3_WHO_AM_I_REG       0X0F

#define LSM6DS3_CTRL1_XL           0X10 
#define LSM6DS3_CTRL1_ODR          4:7
#define LSM6DS3_CTRL1_ODR_OFF      0x0
#define LSM6DS3_CTRL1_ODR_12_5HZ   0x1
#define LSM6DS3_CTRL1_ODR_26HZ     0x2
#define LSM6DS3_CTRL1_ODR_52HZ     0x3
#define LSM6DS3_CTRL1_ODR_104HZ    0x4
#define LSM6DS3_CTRL1_ODR_208HZ    0x5
#define LSM6DS3_CTRL1_ODR_416HZ    0x6
#define LSM6DS3_CTRL1_ODR_833HZ    0x7
#define LSM6DS3_CTRL1_ODR_1660HZ   0x8
#define LSM6DS3_CTRL1_ODR_3330HZ   0x9
#define LSM6DS3_CTRL1_ODR_6660HZ   0xA

#define LSM6DS3_CTRL1_FS           2:3
#define LSM6DS3_CTRL1_FS_2G        0x0
#define LSM6DS3_CTRL1_FS_16G       0x1
#define LSM6DS3_CTRL1_FS_4G        0x2
#define LSM6DS3_CTRL1_FS_8G        0x3

#define LSM6DS3_CTRL1_BW           0:1
#define LSM6DS3_CTRL1_BW_400HZ     0x0
#define LSM6DS3_CTRL1_BW_200HZ     0x1
#define LSM6DS3_CTRL1_BW_100HZ     0x2
#define LSM6DS3_CTRL1_BW_50HZ      0x3

#define LSM6DS3_CTRL2_G            0X11
#define LSM6DS3_CTRL2_ODR          4:7
#define LSM6DS3_CTRL2_ODR_OFF      0x0
#define LSM6DS3_CTRL2_ODR_12_5HZ   0x1
#define LSM6DS3_CTRL2_ODR_26HZ     0x2
#define LSM6DS3_CTRL2_ODR_52HZ     0x3
#define LSM6DS3_CTRL2_ODR_104HZ    0x4
#define LSM6DS3_CTRL2_ODR_208HZ    0x5
#define LSM6DS3_CTRL2_ODR_416HZ    0x6
#define LSM6DS3_CTRL2_ODR_833HZ    0x7
#define LSM6DS3_CTRL2_ODR_1660HZ   0x8

#define LSM6DS3_CTRL2_FS           1:3
#define LSM6DS3_CTRL2_FS_250DPS    0x0
#define LSM6DS3_CTRL2_FS_125DPS    0x1
#define LSM6DS3_CTRL2_FS_500DPS    0x2
#define LSM6DS3_CTRL2_FS_1000DPS   0x4
#define LSM6DS3_CTRL2_FS_2000DPS   0x6

#define LSM6DS3_CTRL3_C            0X12
#define LSM6DS3_CTRL3_SW_RST       0:0
#define LSM6DS3_CTRL3_BLE          1:1
#define LSM6DS3_CTRL3_IF_INC       2:2
#define LSM6DS3_CTRL3_SIM          3:3
#define LSM6DS3_CTRL3_PP_OD        4:4
#define LSM6DS3_CTRL3_H_LACTIVE    5:5
#define LSM6DS3_CTRL3_BDU          6:6
#define LSM6DS3_CTRL3_BOOT         7:7

#define LSM6DS3_CTRL4_C            0X13

#define LSM6DS3_CTRL5_C            0X14

#define LSM6DS3_CTRL6_C            0X15

#define LSM6DS3_CTRL7_G            0X16

#define LSM6DS3_CTRL8_XL           0X17
#define LSM6DS3_CTRL8_LP_ON_6D     0:0
#define LSM6DS3_CTRL8_HP_SLOPE_EN  2:2
#define LSM6DS3_CTRL8_HPCF         5:6
#define LSM6DS3_CTRL8_LPF2_EN      7:7

#define LSM6DS3_CTRL9_XL           0X18

#define LSM6DS3_STATUS             0X1E
#define LSM6DS3_STATUS_XLDA        0:0
#define LSM6DS3_STATUS_GDA         1:1
#define LSM6DS3_STATUS_TDA         2:2

#define LSM6DS3_OUT_TEMP_L         0X20
#define LSM6DS3_OUT_TEMP_H         0X21

#define LSM6DS3_OUTX_L_G           0X22
#define LSM6DS3_OUTX_H_G           0X23
#define LSM6DS3_OUTY_L_G           0X24
#define LSM6DS3_OUTY_H_G           0X25
#define LSM6DS3_OUTZ_L_G           0X26
#define LSM6DS3_OUTZ_H_G           0X27

#define LSM6DS3_OUTX_L_XL          0X28
#define LSM6DS3_OUTX_H_XL          0X29
#define LSM6DS3_OUTY_L_XL          0X2A
#define LSM6DS3_OUTY_H_XL          0X2B
#define LSM6DS3_OUTZ_L_XL          0X2C
#define LSM6DS3_OUTZ_H_XL          0X2D

typedef struct lsm6ds33_t {
    int adapter;
    unsigned address;
    float gyro_scale;
} lsm6ds33;

int lsm6ds33_init(lsm6ds33 *dev, int adapter, unsigned address)
{
    uint8_t data;
    if (i2c_readReg(adapter, address, LSM6DS3_WHO_AM_I_REG, &data) != 0) {
        return -1;
    }
    if (data != LSM6DS3_CHIP_ID) {
        return -1;
    }

    // Reset the chip
    data = BIT_ENCODE(LSM6DS3_CTRL3_SW_RST, 1);
    if (i2c_writeReg(adapter, address, LSM6DS3_CTRL3_C, &data) != 0) {
        return -1;
    }

    // Wait for the chip to come back up
    do {
        sleep(1);
        if(i2c_readReg(adapter, address, LSM6DS3_CTRL3_C, &data) != 0) {
            return -1;
        }
    } while((BIT_VALUE(LSM6DS3_CTRL3_SW_RST, data)) == 2);

    // Set the accel data rate
    data = BIT_ENCODE(LSM6DS3_CTRL1_ODR, LSM6DS3_CTRL1_ODR_104HZ) |
           BIT_ENCODE(LSM6DS3_CTRL1_FS, LSM6DS3_CTRL1_FS_4G) |
           BIT_ENCODE(LSM6DS3_CTRL1_BW, LSM6DS3_CTRL1_BW_400HZ);
    if (i2c_writeReg(adapter, address, LSM6DS3_CTRL1_XL, &data) != 0) {
        return -1;
    }
    // Set the gyro data rate
    data = BIT_ENCODE(LSM6DS3_CTRL2_ODR, LSM6DS3_CTRL2_ODR_104HZ) | 
           BIT_ENCODE(LSM6DS3_CTRL2_FS, LSM6DS3_CTRL2_FS_250DPS);
    if (i2c_writeReg(adapter, address, LSM6DS3_CTRL2_G, &data) != 0) {
        return -1;
    }
    dev->gyro_scale = 0.00875f;
    // Set the block data update to change only when read
    // Enable auto-inc on multi-byte access (block reads)
    data = BIT_ENCODE(LSM6DS3_CTRL3_IF_INC, 1);
    if (i2c_writeReg(adapter, address, LSM6DS3_CTRL3_C, &data) != 0) {
        return -1;
    }

    return 0;
}

int lsm6ds33_sample(lsm6ds33 *dev /*, vec_t *g, vec_t *a, float *tempC*/)
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

    float temp;
    float gyro[3];   
    float accel[2];

    gyro[0] = measurement.gyro[0] * dev->gyro_scale;
    gyro[1] = measurement.gyro[1] * dev->gyro_scale;
    gyro[2] = measurement.gyro[2] * dev->gyro_scale;

    accel[0] = (atan2(measurement.accel[1], measurement.accel[2])+M_PI)*RAD_TO_DEG;
    accel[1] = (atan2(measurement.accel[2], measurement.accel[0])+M_PI)*RAD_TO_DEG;

    return 0;
}

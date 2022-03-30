#ifndef __LSM6DS33_H__
#define __LSM6DS33_H__

#define LSM6DS33_DEFAULT_ADDRESS (0x6A)

typedef struct lsm6ds33_t {
    int adapter;
    unsigned address;
    float gyro_scale;
} lsm6ds33;

int lsm6ds33_init(lsm6ds33 *dev);
int lsm6ds33_sample(lsm6ds33 *dev, float gyro[3], float accel[3], float tempC[1]);

#endif // __LSM6DS33_H__

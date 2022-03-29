#ifndef __IMU_H__
#define __IMU_H__

#include "HandmadeMath/HandmadeMath.h"

typedef int (*pfn_sampleSensors)(float accel[3], float gyro[3], float mag[3], float dt[1], void *user);

typedef struct imu_t {
    pfn_sampleSensors sample_cb;
    hmm_quaternion orient;
    hmm_vec3 ang_vel;
// Internal
    hmm_vec3 _integ_gyro;
} imu;

int imu_init(imu *ctx);

int imu_update(imu *ctx, void *user);

#endif // __IMU_H__

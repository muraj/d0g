#ifndef __IMU_H__
#define __IMU_H__

struct imu_t;
typedef imu_t imu;

typedef struct vec3f_t {
    float x, y, z;
} vec3f;

typedef struct vec4f_t {
    float w, x, y, z;
} vec4f_t;

typedef int (*pfn_sampleSensors)(vec3f *accel, vec3f *gyro, vec3f *mag, void *user);

int imu_init(imu *ctx, pfn_sampleSensors cb, void *user);

int imu_get_orientation(vec4f_t *q, imu *ctx);

#endif // __IMU_H__

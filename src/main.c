#include <unistd.h>
#include <time.h>

#include "i2c_adapter.h"
#include "imu.h"
#include "drivers/lis3mdl.h"
#include "drivers/lsm6ds33.h"

struct imu_update_params {
    lis3mdl magDevice;
    lsm6ds33 gyroDevice;
    struct timespec timeStep;
};

static void timespec_diff(struct timespec *start, struct timespec *stop,
                   struct timespec *result)
{
    if ((stop->tv_nsec - start->tv_nsec) < 0) {
        result->tv_sec = stop->tv_sec - start->tv_sec - 1ULL;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000ULL;
    } else {
        result->tv_sec = stop->tv_sec - start->tv_sec;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec;
    }
}

static int sampleSensors(float accel[3], float gyro[3], float mag[3], float dt[1], void *user)
{
    float tempC;
    struct timespec t;
    struct imu_update_params *params = (struct imu_update_params *)user;

    if (lis3mdl_sample(&params->magDevice, mag, &tempC) != 0) {
        return -1;
    }
    if (lsm6ds33_sample(&params->gyroDevice, gyro, accel, &tempC) != 0) {
        return -1;
    }

	t = params->timeStep;

    if (clock_gettime(CLOCK_REALTIME, &params->timeStep) != 0) {
        return -1;
    }
	timespec_diff(&t, &params->timeStep, &t);

	dt[0] = t.tv_sec * 1.0e6f + t.tv_nsec / 1000.0f;

    return 0;

}

int main() {
    struct imu_update_params imu_params;
    imu imu_ctx;
    int i2c_adapter_fd = i2c_create_adapter(1);

    imu_params.magDevice.adapter = i2c_adapter_fd;
    imu_params.magDevice.address = LIS3MDL_DEFAULT_ADDRESS;
    imu_params.gyroDevice.adapter = i2c_adapter_fd;
    imu_params.magDevice.address = LSM6DS33_DEFAULT_ADDRESS;

    if (lis3mdl_init(&imu_params.magDevice) != 0) {
        return -1;
    }
    if (lsm6ds33_init(&imu_params.gyroDevice) != 0) {
        return -1;
    }

    imu_ctx.sample_cb = sampleSensors;

    if (imu_init(&imu_ctx) != 0) {
        return -1;
    }
    while (1) {
        if (imu_update(&imu_ctx, &imu_params) != 0) {
            return -1;
        }
        sleep(1);
    }
    return 0;
}

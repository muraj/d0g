#include "imu.h"

int imu_init(imu *ctx)
{
    ctx->_integ_gyro = HMM_Vec3(0, 0, 1);
    ctx->orient = HMM_Quaternion(0, 0, 0, 1);
    return 0;
}

int imu_update(imu *ctx, void *user)
{
    hmm_vec3 accel, mag;
    float dt;

    if (ctx->sample_cb(accel.Elements, ctx->ang_vel.Elements, mag.Elements, &dt, user) != 0) {
        return -1;
    }
    // Linearly integrate the gyro
    ctx->_integ_gyro = HMM_AddVec3(ctx->_integ_gyro, HMM_MultiplyVec3f(ctx->ang_vel, dt));
    return 0;
}

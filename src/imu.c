#include "imu.h"

int imu_init(imu *ctx)
{
    ctx->_integ_gyro = HMM_Vec3(0, 0, 1);
    ctx->orient = HMM_Quaternion(0, 0, 0, 1);
}

int imu_update(imu *ctx, void *user)
{
    hmm_vec3 accel, mag;
    float dt;
    ctx->sample_cb(accel.Elements, ctx->ang_vel.Elements, mag.Elements, &dt, user);
    // Linearly integrate the gyro
    ctx->_integ_gyro = HMM_AddVec3(ctx->_integ_gyro, HMM_MultiplyVec3f(ctx->ang_vel, dt));
}

#ifndef __LIS3MDL_H__
#define __LIS3MDL_H__

typedef struct lis3mdl_t {
    int adapter;
    unsigned address;
} lis3mdl;


int lis3mdl_init(lis3mdl *dev);
int lis3mdl_sample(lis3mdl *dev, float v[3], float tempC[1]);

#endif // __LIS3MDL_H__

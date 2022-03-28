#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#ifndef I2C_M_RD
#include <linux/i2c.h>
#endif

#include "i2c_adapter.h"

int i2c_create_adapter(unsigned adapter)
{
    int fd = 0;
    char filename[20];
    (void)snprintf(filename, sizeof(filename)-1, "/dev/i2c-%d", adapter);

    return open(filename, O_RDWR);
}

void i2c_destroy_adapter(int fd) {
    close(fd);
}

int i2c_read(int adapter, uint16_t address, uint8_t reg, void *buf, size_t len)
{
    struct i2c_msg msgs[2] = {
        {address, 0, sizeof(reg), &reg},
        {address, I2C_M_RD | I2C_M_NOSTART, len, buf},
    };
    struct i2c_rdwr_ioctl_data msgset = { msgs, 2 };

    if (ioctl(adapter, I2C_RDWR, &msgset)) {
        return -1;
    }

    return len;
}

int i2c_write(int adapter, uint16_t address, uint8_t reg, void *buf, size_t len)
{
    struct i2c_msg msgs[2] = {
        {address, 0, sizeof(reg), &reg},
        {address, I2C_M_NOSTART, len, buf},
    };
    struct i2c_rdwr_ioctl_data msgset = { msgs, 2 };

    if (ioctl(adapter, I2C_RDWR, &msgset)) {
        return -1;
    }

    return len;
}

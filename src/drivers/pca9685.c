#include "i2c_adapter.h"
#include "utils.h"
#include "bitfields.h"
#include <unistd.h>

#define PCA96835_DEFAULT_CLK_HZ (25000000UL)

#define PCA96835_MODE1          0x00
#define PCA96835_MODE1_ALLCALL 0:0
#define PCA96835_MODE1_SUB3    1:1
#define PCA96835_MODE1_SUB2    2:2
#define PCA96835_MODE1_SUB1    3:3
#define PCA96835_MODE1_SLEEP   4:4
#define PCA96835_MODE1_AI      5:5
#define PCA96835_MODE1_EXTCLK  6:6
#define PCA96835_MODE1_RESTART 7:7

#define PCA96835_MODE2          0x01
#define PCA96835_SUBADR1        0x02
#define PCA96835_SUBADR2        0x03
#define PCA96835_SUBADR3        0x04
#define PCA96835_ALLCALLADR     0x05
#define PCA96835_LED_ON_L(i)    (0x06+(i*0x4))
#define PCA96835_LED_ON_H(i)    (0x07+(i*0x4))
#define PCA96835_LED_OFF_L(i)   (0x08+(i*0x4))
#define PCA96835_LED_OFF_H(i)   (0x09+(i*0x4))
#define PCA96835_ALL_LED_ON_L   0xFA
#define PCA96835_ALL_LED_ON_H   0xFB
#define PCA96835_ALL_LED_OFF_L  0xFC
#define PCA96835_ALL_LED_OFF_H  0xFD
#define PCA96835_PRESCALE       0xFE
#define PCA96835_TESTMODE       0xFF

typedef struct pca96853_t {
    int adapter;
    unsigned address;
    uint32_t clk;
} pca96853;

int pca96853_reset(pca96853 *dev);
uint8_t pca96853_set_freq(pca96853 *dev, uint16_t freqHz);
int pca96853_init(pca96853 *dev)
{
    uint8_t data = 0;

    if (dev->clk == 0) {
        dev->clk = PCA96835_DEFAULT_CLK_HZ;
    }

    if (i2c_readReg(dev->adapter, dev->address, PCA96835_MODE1, &data) < 0) {
        return -1;
    }

    data &= ~BIT_ENCODE(PCA96835_MODE1_RESTART, 1);
    data |= BIT_ENCODE(PCA96835_MODE1_AI, 1);
    if (i2c_writeReg(dev->adapter, dev->address, PCA96835_MODE1, &data) < 0) {
        return -1;
    }

    return -1;
}

int pca96853_reset(pca96853 *dev)
{
    int status = 0;
#if 0
    uint8_t data = 0x06;

    status = i2c_writeReg(dev->adapter, 0x00, &data);
    if (status == 0) {
        sleep(1);
    }
#endif

    return status;
}

uint8_t pca96853_set_freq(pca96853 *dev, uint16_t freqHz)
{
    uint8_t old_mode = 0;
    uint8_t data = 0;
    if (i2c_readReg(dev->adapter, dev->address, PCA96835_MODE1, &old_mode) < 0) {
        return -1;
    }
    data = old_mode & ~BIT_ENCODE(PCA96835_MODE1_RESTART, 1);
    data |= BIT_ENCODE(PCA96835_MODE1_SLEEP, 1);
    // TODO: Group these up
    if (i2c_writeReg(dev->adapter, dev->address, PCA96835_MODE1, &data) < 0) {
        return -1;
    }
    data = (uint8_t)(dev->clk / (4096 * freqHz));
    if (i2c_writeReg(dev->adapter, dev->address, PCA96835_PRESCALE, &data) < 0) {
        return -1;
    }
    data = old_mode;
    if (i2c_writeReg(dev->adapter, dev->address, PCA96835_MODE1, &data) < 0) {
        return -1;
    }

    sleep(1);

    data |= BIT_ENCODE(PCA96835_MODE1_AI, 1);
    if (i2c_writeReg(dev->adapter, dev->address, PCA96835_MODE1, &data) < 0) {
        return -1;
    }

    return 0;
}

typedef struct PWMData_t {
    uint16_t on;
    uint16_t off;
} PWMData;

CASSERT(offsetof(struct PWMData_t, on) == 0);
CASSERT(offsetof(struct PWMData_t, off) == 2);

static void pca96853_enum_duty_packet(uint16_t duty, PWMData *data)
{
    data->on  = (duty == 0xFFFF) ? 0x1000 : 0;
    data->off = (duty == 0x0) ? 0x1000 : ((duty+1)>>4);
}

int pca96853_set_channel(pca96853 *dev, uint8_t ch, uint16_t duty)
{
    PWMData data;
    pca96853_enum_duty_packet(duty, &data);

    return i2c_writeReg(dev->adapter, dev->address, PCA96835_LED_ON_L(ch), &data);
}

int pca96853_set_channels(pca96853 *dev, uint16_t *duty)
{
    PWMData data[16];
    for (unsigned i = 0; i < 16; i++) {
        pca96853_enum_duty_packet(duty[i], &data[i]);
    }

    return i2c_writeReg(dev->adapter, dev->address, PCA96835_LED_ON_L(0), &data);
}

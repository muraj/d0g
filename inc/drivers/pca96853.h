#ifndef __PCA96853_H__
#define __PCA96853_H__

#define PCA96853_DEFAULT_ADDRESS (0x40)

typedef struct pca96853_t {
    int adapter;
    unsigned address;
    uint32_t clk;
} pca96853;

int pca96853_init(pca96853 *dev);
int pca96853_set_channel(pca96853 *dev, uint8_t ch, uint16_t duty);
int pca96853_set_channels(pca96853 *dev, uint16_t duty[16]);

#endif // __PCA96853_H__

#ifndef __BITFIELDS_H__
#define __BITFIELDS_H__

#define BIT_HI(bit) (0?bit)
#define BIT_LO(bit) (1?bit)
#define BIT_MASK_UPPER(bit) (~1U)<<BIT_HI(bit)
#define BIT_MASK_LOWER(bit) (~0U)<<BIT_LO(bit)
#define BIT_MASK(bit) 
#define BIT_ENCODE(bit, val) ((val) << BIT_LO(bit))
#define BIT_VALUE(bit, val) (((val) & ~BIT_MASK_UPPER(bit)) >> BIT_LO(bit))

#endif // __BITFIELDS_H__

#ifndef __UTILS_H__
#define __UTILS_H__

#include <math.h>

#ifndef RAD_TO_DEG
#define RAD_TO_DEG (57.29578f)
#endif

#ifndef CASSERT
#define CASSERT(x) __impl_CASSERT_LINE(x,__LINE__,__FILE__)
#define __impl_CASSERT_PASTE(a,b) a##b
#define __impl_CASSERT_LINE(x,line,file) \
    typedef char __impl_CASSERT_PASTE(assertion_failed_##file##_,line)[2*!!(x)-1]
#endif


#endif // __UTILS_H__

// Add by yaukey at 2020-06-20.
// All math basic defination.

#ifndef __YW_MATH_BASE_DEFINE_H__
#define __YW_MATH_BASE_DEFINE_H__

#include <math.h>

namespace yw
{
    // Float compare precision.
    #if defined(_WIN32) || defined(WIN32)
        #define YW_FLOAT_PRECISION FLT_EPSILON
    #elif defined(LINUX_X11) || defined(_LINUX)
        #define YW_FLOAT_PRECISION __FLT_EPSILON__
    #elif defined(_MAC_OSX)
        #define YW_FLOAT_PRECISION __FLT_EPSILON__
    #elif defined(__amigaos4__) || defined(_AMIGAOS4)
        #define YW_FLOAT_PRECISION __FLT_EPSILON__
    #endif

    // Math pi.
    #define YW_PI 3.14159265359f
    #define YW_TWO_PI 6.28318530718f
    #define YW_FOUR_PI 12.56637061436f
    #define YW_INV_PI 0.31830988618f
    #define YW_INV_TWO_PI 0.15915494309f
    #define YW_INV_FOUR_PI 0.07957747155f
    #define YW_HALF_PI 1.57079632679f
    #define YW_INV_HALF_PI 0.636619772367f
}

#endif // !__YW_MATH_BASE_DEFINE_H__

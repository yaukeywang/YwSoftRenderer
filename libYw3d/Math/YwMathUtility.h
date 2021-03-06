// Add by yaukey at 2017-09-19.
// All math utility deifnation.

#ifndef __YW_MATH_UTILITY_H__
#define __YW_MATH_UTILITY_H__

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

    // Constant convert radians to degree.
    const float RAD_TO_DEG = 180.0f / YW_PI;

    // Constant convert degree to radians.
    const float DEG_TO_RAD = YW_PI / 180.0f;

    // Converts radians to degrees.
    // @param[in] value radians.
    // @return degrees.
    inline float RadToDeg(const float value)
    {
        return 180.0f * value / YW_PI;
    }

    // Converts degrees to radians.
    // @param[in] value degrees.
    // @return radians.
    inline float DegToRad(const float value)
    {
        return YW_PI * value / 180.0f;
    }

    // Clamps a floating-point value.
    // @param[in] value to clamp.
    // @param[in] lower minimum value.
    // @param[in] upper maximum value.
    // @return clamped value in [lower,upper].
    inline float Clamp(const float value, const float lower, const float upper)
    {
        if (value < lower)
        {
            return lower;
        }
        else if (value > upper)
        {
            return upper;
        }
        else
        {
            return value;
        }
    }

    // Clamps an integer value.
    // @param[in] value to clamp.
    // @param[in] lower minimum value.
    // @param[in] upper maximum value.
    // @return clamped value in [lower,upper].
    inline int32_t Clamp(const int32_t value, const int32_t lower, const int32_t upper)
    {
        if (value < lower)
        {
            return lower;
        }
        else if (value > upper)
        {
            return upper;
        }
        else
        {
            return value;
        }
    }

    // Clamps a floating-point value to [0.0f,1.0f].
    // @param[in] value to saturate.
    // @return saturated value in [0.0f,1.0f].
    inline float Saturate(const float value)
    {
        return Clamp(value, 0.0f, 1.0f);
    }

    // Linearly interpolates between two values.
    // @param[in] a first value.
    // @param[in] b second value.
    // @param[in] t interpolation factor in [0.0f,1.0f].
    // @return interpolated value.
    inline float Lerp(const float a, const float b, const float t)
    {
        return a + (b - a) * t;
    }

    // Determining if an integer is a power of 2.
    // http://graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2
    // @param[in] n the number to determining.
    // @return true if n is power of 2, otherwise not.
    inline bool DetermineIfPowerOf2(uint32_t n)
    {
        return (n && !(n & (n - 1)));
    }

    // Compute the next highest power of 2 of 32-bit n.
    // 0->0, 1->1, 3->4, 5->8, 100->128...
    // http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
    // @param[in] n the number to compute.
    // @return the next highest power of 2 of 32-bit n.
    inline uint32_t RoundUpPowerOf2(uint32_t n)
    {
        n--;
        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;
        n++;

        return n;
    }
}

#endif // !__YW_MATH_UTILITY_H__

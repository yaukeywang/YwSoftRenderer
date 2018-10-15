// Add by yaukey at 2017-09-19.
// All math utility deifnation.

#ifndef __YW_MATH_UTILITY_H__
#define __YW_MATH_UTILITY_H__

#include <math.h>

namespace yw
{
    // Float compare precision.
    #define YW_FLOAT_PRECISION FLT_EPSILON

    // Math pi.
    #define YW_PI 3.1415926f

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
    // @return clamped value e [lower,upper].
    inline float Clamp(const float value, const float lower, const float upper)
    {
        if (value <= lower)
        {
            return lower;
        }
        else if (value >= upper)
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
    // @return clamped value e [lower,upper].
    inline int32_t Clamp(const int32_t value, const int32_t lower, const int32_t upper)
    {
        if (value <= lower)
        {
            return lower;
        }
        else if (value >= upper)
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
    // @return saturated value e [0.0f,1.0f].
    inline float Saturate(const float value)
    {
        return Clamp(value, 0.0f, 1.0f);
    }

    // Linearly interpolates between two values.
    // @param[in] a first value.
    // @param[in] b second value.
    // @param[in] t interpolation factor e [0.0f,1.0f].
    // @return interpolated value.
    inline float Lerp(const float a, const float b, const float t)
    {
        return a + (b - a) * t;
    }
}

#endif // !__YW_MATH_UTILITY_H__

// Add by yaukey at 2017-09-19.
// All math utility defination.

#ifndef __YW_MATH_UTILITY_H__
#define __YW_MATH_UTILITY_H__

#include "YwMathBaseDefine.h"
#include "YwMathVector.h"

namespace yw
{
    /**
     * @brief Constant convert radians to degree.
     */
    const float RAD_TO_DEG = 180.0f / YW_PI;
    
    /**
     * @brief Constant convert degree to radians.
     */
    const float DEG_TO_RAD = YW_PI / 180.0f;
    
    /**
     * @brief Converts radians to degrees.
     * @param[in] v radians.
     * @return degrees.
     */
    inline float RadToDeg(const float v)
    {
        return 180.0f * v / YW_PI;
    }
    
    /**
     * @brief Converts degrees to radians.
     * @param v degrees.
     * @return radians
     */
    inline float DegToRad(const float v)
    {
        return YW_PI * v / 180.0f;
    }
    
    /**
     * @brief Clamps a floating-point value.
     * @param[in] v value to clamp.
     * @param[in] l minimum value.
     * @param[in] u maximum value.
     * @return clamped value in [l,u].
     */
    inline float Clamp(const float v, const float l, const float u)
    {
        if (v < l)
        {
            return l;
        }
        else if (v > u)
        {
            return u;
        }
        else
        {
            return v;
        }
    }

    /**
     * @brief Clamps an integer value.
     * @param[in] v value to clamp.
     * @param[in] l minimum value.
     * @param[in] u maximum value.
     * @return clamped value in [l,u].
     */
    inline int32_t Clamp(const int32_t v, const int32_t l, const int32_t u)
    {
        if (v < l)
        {
            return l;
        }
        else if (v > u)
        {
            return u;
        }
        else
        {
            return v;
        }
    }

    /**
     * @brief Clamps a Vector2 value.
     * @param[in] v value to clamp.
     * @param[in] l minimum value.
     * @param[in] u maximum value.
     * @return clamped value in [l,u].
     */
    inline Vector2 Clamp(const Vector2& v, const float l, const float u)
    {
        return Vector2(Clamp(v.x, l, u), Clamp(v.y, l, u));
    }

    /**
     * @brief Clamps a Vector3 value.
     * @param[in] v value to clamp.
     * @param[in] l minimum value.
     * @param[in] u maximum value.
     * @return clamped value in [l,u].
     */
    inline Vector3 Clamp(const Vector3& v, const float l, const float u)
    {
        return Vector3(Clamp(v.x, l, u), Clamp(v.y, l, u), Clamp(v.z, l, u));
    }

    /**
     * @brief Clamps a Vector4 value.
     * @param[in] v value to clamp.
     * @param[in] l minimum value.
     * @param[in] u maximum value.
     * @return clamped value in [l,u].
     */
    inline Vector4 Clamp(const Vector4& v, const float l, const float u)
    {
        return Vector4(Clamp(v.x, l, u), Clamp(v.y, l, u), Clamp(v.z, l, u), Clamp(v.w, l, u));
    }

    /**
     * @brief Clamps a floating-point value to [0.0f,1.0f].
     * @param[in] v value to saturate.
     * @return saturated value in [0.0f,1.0f].
     */
    inline float Saturate(const float v)
    {
        return Clamp(v, 0.0f, 1.0f);
    }

    /**
     * @brief Clamps a Vector2 value to [0.0f,1.0f].
     * @param[in] v value to saturate.
     * @return saturated value in [0.0f,1.0f].
     */
    inline Vector2 Saturate(const Vector2& v)
    {
        return Vector2(Saturate(v.x), Saturate(v.y));
    }

    /**
     * @brief Clamps a Vector3 value to [0.0f,1.0f].
     * @param[in] v value to saturate.
     * @return saturated value in [0.0f,1.0f].
     */
    inline Vector3 Saturate(const Vector3& v)
    {
        return Vector3(Saturate(v.x), Saturate(v.y), Saturate(v.z));
    }

    /**
     * @brief Clamps a Vector4 value to [0.0f,1.0f].
     * @param[in] v value to saturate.
     * @return saturated value in [0.0f,1.0f].
     */
    inline Vector4 Saturate(const Vector4& v)
    {
        return Vector4(Saturate(v.x), Saturate(v.y), Saturate(v.z), Saturate(v.w));
    }

    /**
     * @brief Linearly interpolates between two values.
     * @param[in] a first value.
     * @param[in] b second value.
     * @param [in] t interpolation factor in [0.0f,1.0f].
     * @return Linearly interpolates between two values.
     */
    inline float Lerp(const float a, const float b, const float t)
    {
        return a + (b - a) * t;
    }

    /**
     * @brief Linearly interpolates between two Vector2 values.
     * @param[in] a first value.
     * @param[in] b second value.
     * @param [in] t interpolation factor in [0.0f,1.0f].
     * @return Linearly interpolates between two values.
     */
    inline Vector2 Lerp(const Vector2& a, const Vector2& b, const float t)
    {
        return Vector2(Lerp(a.x, b.x, t), Lerp(a.y, b.y, t));
    }

    /**
     * @brief Linearly interpolates between two Vector3 values.
     * @param[in] a first value.
     * @param[in] b second value.
     * @param [in] t interpolation factor in [0.0f,1.0f].
     * @return Linearly interpolates between two values.
     */
    inline Vector3 Lerp(const Vector3& a, const Vector3& b, const float t)
    {
        Vector3 result;
        return Vector3Lerp(result, a, b, t);
    }

    /**
     * @brief Linearly interpolates between two Vector4 values.
     * @param[in] a first value.
     * @param[in] b second value.
     * @param [in] t interpolation factor in [0.0f,1.0f].
     * @return Linearly interpolates between two values.
     */
    inline Vector4 Lerp(const Vector4& a, const Vector4& b, const float t)
    {
        Vector4 result;
        return Vector4Lerp(result, a, b, t);
    }

    /**
     * @brief The minimum result of two float value.
     * @param[in] a first value.
     * @param[in] b second value.
     * @return the minimum of two values.
     */
    inline float Minimum(const float a, const float b)
    {
        return min(a, b);
    }

    /**
     * @brief The minimum result of two vector2. These all operate component-wise. The calculation is per component.
     * @param[in] a first value.
     * @param[in] b second value.
     * @return the minimum of two scalars or each respective component of two vectors.
     */
    inline Vector2 Minimum(const Vector2& a, const Vector2& b)
    {
        return Vector2(min(a.x, b.x), min(a.y, b.y));
    }

    /**
     * @brief The minimum result of two vector3. These all operate component-wise. The calculation is per component.
     * @param[in] a first value.
     * @param[in] b second value.
     * @return the minimum of two scalars or each respective component of two vectors.
     */
    inline Vector3 Minimum(const Vector3& a, const Vector3& b)
    {
        return Vector3(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
    }

    /**
     * @brief The minimum result of two vector4. These all operate component-wise. The calculation is per component.
     * @param[in] a first value.
     * @param[in] b second value.
     * @return the minimum of two scalars or each respective component of two vectors.
     */
    inline Vector4 Minimum(const Vector4& a, const Vector4& b)
    {
        return Vector4(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w));
    }

    /**
     * @brief The maximum result of two float value.
     * @param[in] a first value.
     * @param[in] b second value.
     * @return the maximum of two values.
     */
    inline float Maximum(const float a, const float b)
    {
        return max(a, b);
    }

    /**
     * @brief The maximum result of two vector2. These all operate component-wise. The calculation is per component.
     * @param[in] a first value.
     * @param[in] b second value.
     * @return the maximum of two scalars or each respective component of two vectors.
     */
    inline Vector2 Maximum(const Vector2& a, const Vector2& b)
    {
        return Vector2(max(a.x, b.x), max(a.y, b.y));
    }

    /**
     * @brief The maximum result of two vector3. These all operate component-wise. The calculation is per component.
     * @param[in] a first value.
     * @param[in] b second value.
     * @return the maximum of two scalars or each respective component of two vectors.
     */
    inline Vector3 Maximum(const Vector3& a, const Vector3& b)
    {
        return Vector3(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
    }

    /**
     * @brief The maximum result of two vector4. These all operate component-wise. The calculation is per component.
     * @param[in] a first value.
     * @param[in] b second value.
     * @return the maximum of two scalars or each respective component of two vectors.
     */
    inline Vector4 Maximum(const Vector4& a, const Vector4& b)
    {
        return Vector4(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w));
    }

    /**
     * @brief Returns true if any component of v is not equal to 0. Returns false otherwise.
     * @param[in] v value that to be checked.
     * @return true if any component of x is not equal to 0. false otherwise.
     */
    inline bool Any(const float v)
    {
        return (v > YW_FLOAT_PRECISION || v < -YW_FLOAT_PRECISION) ? true : false;
    }

    /**
     * @brief Returns true if any component of v is not equal to 0. Returns false otherwise.
     * @param[in] v value that to be checked.
     * @return true if any component of x is not equal to 0. false otherwise.
     */
    inline bool Any(const Vector2& v)
    {
        return Vector2::Zero() != v ? true : false;
    }

    /**
     * @brief Returns true if any component of v is not equal to 0. Returns false otherwise.
     * @param[in] v value that to be checked.
     * @return true if any component of x is not equal to 0. false otherwise.
     */
    inline bool Any(const Vector3& v)
    {
        return Vector3::Zero() != v ? true : false;
    }

    /**
     * @brief Returns true if any component of v is not equal to 0. Returns false otherwise.
     * @param[in] v value that to be checked.
     * @return true if any component of x is not equal to 0. false otherwise.
     */
    inline bool Any(const Vector4& v)
    {
        return Vector4::Zero() != v ? true : false;
    }

    /**
     * Returns the specified value raised to the specified power.
     * @param[in] v The specified value.
     * @param[in] p The specified power.
     * @return The v parameter raised to the power of the p parameter.
     */
    inline float Pow(const float v, const float p)
    {
        return (float)::pow(v, p);
    }

    /**
     * Returns the specified value raised to the specified power.
     * @param[in] v The specified value.
     * @param[in] p The specified power.
     * @return The v parameter raised to the power of the p parameter.
     */
    inline Vector2 Pow(const Vector2& v, const Vector2& p)
    {
        return Vector2((float)::pow(v.x, p.x), (float)::pow(v.y, p.y));
    }

    /**
     * Returns the specified value raised to the specified power.
     * @param[in] v The specified value.
     * @param[in] p The specified power.
     * @return The v parameter raised to the power of the p parameter.
     */
    inline Vector3 Pow(const Vector3& v, const Vector3& p)
    {
        return Vector3((float)::pow(v.x, p.x), (float)::pow(v.y, p.y), (float)::pow(v.z, p.z));
    }

    /**
     * Returns the specified value raised to the specified power.
     * @param[in] v The specified value.
     * @param[in] p The specified power.
     * @return The v parameter raised to the power of the p parameter.
     */
    inline Vector4 Pow(const Vector4& v, const Vector4& p)
    {
        return Vector4((float)::pow(v.x, p.x), (float)::pow(v.y, p.y), (float)::pow(v.z, p.z), (float)::pow(v.w, p.w));
    }

    /**
     * @brief Determining if an integer is a times of 2.
     * @param[in] n the number to determining.
     * @return true if n is times of 2, otherwise not.
     */
    inline bool DetermineIfTimesOf2(uint32_t n)
    {
        return (n && !(n & 1));
    }

    /**
     * @brief Determining if an integer is a power of 2.
     * http://graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2
     * @param[in] n the number to determining.
     * @return true if n is power of 2, otherwise not.
     */
    inline bool DetermineIfPowerOf2(uint32_t n)
    {
        return (n && !(n & (n - 1)));
    }

    /**
     * @brief Compute the next highest power of 2 of 32-bit n.
     * 0->0, 1->1, 3->4, 5->8, 100->128...
     * http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
     * @param[in] n the number to compute.
     * @return the next highest power of 2 of 32-bit n.
     */
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

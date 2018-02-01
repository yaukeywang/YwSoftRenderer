// Add by Yaukey at 2018-01-29.
// YW Soft Renderer 3d surface class.

#ifndef __YW_3D_SURFACE_H__
#define __YW_3D_SURFACE_H__

#include "Yw3dBase.h"
#include "Yw3dTypes.h"

namespace yw
{
    // Yw3dSurface implements a 2-dimensional image.
    class Yw3dSurface : public IBase
    {
        friend class Yw3dDevice;

    protected:
        // Accessible by Yw3dDevice which is the only class that may create a surface.
        // @param[in] device a pointer to the parent Yw3dDevice-object.
        Yw3dSurface(class Yw3dDevice* device);

        // Accessible by IBase. The destructor is called when the reference count reaches zero.
        ~Yw3dSurface();

    protected:
        // Accessible by Yw3dDevice which is the only class that may create a surface.
        // @param[in] width width of the surface to be created in pixels.
        // @param[in] height height of the surface to be created in pixels.
        // @param[in] Yw3dFormat format of the surface to be created. Member of the enumeration m3dformat; m3dfmt_r32f, m3dfmt_r32g32f, m3dfmt_r32g32b32f or m3dfmt_r32g32b32a32f.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_OutOfMemory if memory allocation failed.
        // @return Yw3d_E_InvalidFormat if an invalid format was encountered.
        Yw3dResult Create(const uint32_t width, const uint32_t height, const Yw3dFormat format);

    public:
        // Samples the surface using nearest point sampling.
        // @param[out] outColor receives the color of the pixel to be looked up.
        // @param[in] u u-component of the lookup-vector.
        // @param[in] v v-component of the lookup-vector.
        void SamplePoint(Vector4& outColor, const float u, const float v);

        // Samples the surface using bi-linear filtering.
        // @param[out] outColor receives the color of the pixel to be looked up.
        // @param[in] u u-component of the lookup-vector.
        // @param[in] v v-component of the lookup-vector.
        void SampleLinear(Vector4& outColor, const float u, const float v);

        // Clears the surface to a given color.
        // @param[in] color color to clear the surface to.
        // @param[in] rect rectangle to restrict clearing to.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if the clear-rectangle exceeds the surface's dimensions.
        Yw3dResult Clear(const Vector4& color, const Yw3dRect* rect);

        // Copies the contents of the surface to another surface using the specified filtering method.
        // @param[in] srcRect source rectangle to copy to the other surface. (Pass 0 to copy the entire surface.)
        // @param[in] destSurface destination surface.
        // @param[in] destRect destination-rectangle. (Pass 0 for the entire destination-surface.)
        // @param[in] filter texture filter.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one of the two rectangles is invalid or exceeds surface-dimensions.
        // @return Yw3d_E_InvalidState if the destination surface couldn't be locked.
        Yw3dResult CopyToSurface(const Yw3dRect* srcRect, Yw3dSurface* destSurface, const Yw3dRect* destRect, const Yw3dTextureFilter filter);

        // Returns a pointer to the contents of the surface.
        // @param[out] lockedData receives the pointer to the surface-data.
        // @param[in] lockRect area that will be locked and accessible. (Pass in 0 to lock the entire surface.)
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_InvalidState if the surface is already locked.
        // @return Yw3d_E_OutOfMemory if memory allocation failed.
        // @note Locking the entire surface is a lot faster than locking a sub-region, because no lock-buffer has to be created and the application may write to the surface directly.
        Yw3dResult LockRect(void** lockedData, const Yw3dRect* lockRect);

        // Unlocks the surface; modifications to its contents will become active.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidState if the surface is not locked.
        Yw3dResult UnlockRect();

        // Returns the format of the surface. Member of the enumeration m3dformat; m3dfmt_r32f, m3dfmt_r32g32f, m3dfmt_r32g32b32f or m3dfmt_r32g32b32a32f.
        Yw3dFormat GetFormat() const;

        //< Returns the number of floats of the format, e [1,4].
        uint32_t GetFormatFloats() const;

        //< Returns the width of the surface in pixels.
        uint32_t GetWidth() const;

        //< Returns the height of the surface in pixels.
        uint32_t GetHeight() const;

        // Returns a pointer to the associated device. Calling this function will increase the internal reference count of the device. Failure to call Release() when finished using the pointer will Yw3dResult in a memory leak.
        class Yw3dDevice* AquireDevice();

    private:
        // Pointer to parent.
        class Yw3dDevice* m_Device;

        // Format of the surface. Member of the enumeration m3dformat; m3dfmt_r32f, m3dfmt_r32g32f, m3dfmt_r32g32b32f or m3dfmt_r32g32b32a32f.
        Yw3dFormat m_Format;

        // Width of the surface in pixels.
        uint32_t m_Width;

        // Height of the surface in pixels.
        uint32_t m_Height;

        // Width - 1 of the surface in pixels.
        uint32_t m_WidthMin1;

        // Height - 1 of the surface in pixels.
        uint32_t m_HeightMin1;

        // True if the whole surface has been locked.
        bool m_LockedComplete;

        // Information about the locked rectangle.
        Yw3dRect m_PartialLockRect;

        // Not null if a sub-rectangle of the surface has been locked.
        float* m_PartialLockData;

        // Pointer to surface data.
        float* m_Data;
    };
}

#endif // !__YW_3D_SURFACE_H__

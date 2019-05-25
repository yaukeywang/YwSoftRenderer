// Add by Yaukey at 2018-05-23.
// YW Soft Renderer 3-dimensional image class.

#ifndef __YW_3D_VOLUME_H__
#define __YW_3D_VOLUME_H__

#include "Yw3dBase.h"
#include "Yw3dTypes.h"

namespace yw
{
    // Yw3dVolume implements a 3-dimensional image.
    class Yw3dVolume : public IBase
    {
        friend class Yw3dDevice;

    protected:
        // Accessible by Yw3dDevice which is the only class that may create a volume.
        // @param[in] device a pointer to the parent Yw3dDevice-object.
        Yw3dVolume(class Yw3dDevice* device);

        // Accessible by IBase. The destructor is called when the reference count reaches zero.
        ~Yw3dVolume();

    protected:
        // Accessible by Yw3dDevice which is the only class that may create a volume.
        // @param[in] width width of the volume to be created in pixels.
        // @param[in] height height of the volume to be created in pixels.
        // @param[in] depth depth of the volume to be created in pixels.
        // @param[in] format format of the volume to be created. Member of the enumeration m3dformat; m3dfmt_r32f, m3dfmt_r32g32f, m3dfmt_r32g32b32f or m3dfmt_r32g32b32a32f.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_OutOfMemory if memory allocation failed.
        // @return Yw3d_E_InvalidFormat if an invalid format was encountered.
        Yw3dResult Create(uint32_t width, uint32_t height, uint32_t depth, Yw3dFormat format);

    public:
        // Samples the volume using nearest point sampling.
        // @param[out] color receives the color of the pixel to be looked up.
        // @param[in] u u-component of the lookup-vector.
        // @param[in] v v-component of the lookup-vector.
        // @param[in] w w-component of the lookup-vector.
        void SamplePoint(Vector4& color, float u, float v, float w);

        // Samples the volume using bi-linear filtering.
        // @param[out] color receives the color of the pixel to be looked up.
        // @param[in] u u-component of the lookup-vector.
        // @param[in] v v-component of the lookup-vector.
        // @param[in] w w-component of the lookup-vector.
        void SampleLinear(Vector4& color, float u, float v, float w);

        // Clears the volume to a given color.
        // @param[in] color color to clear the volume to.
        // @param[in] box box to restrict clearing to.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if the clear-box exceeds the volume's dimensions.
        Yw3dResult Clear(const Vector4& color, const Yw3dBox* box);

        // Copies the contents of the volume to another volume using the specified filtering method.
        // @param[in] srcBox source box to copy to the other volume. (Pass 0 to copy the entire volume.)
        // @param[in] destVolume destination volume.
        // @param[in] destBox destination-box. (Pass 0 for the entire destination-volume.)
        // @param[in] filter texture filter.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one of the two boxes is invalid or exceeds volume-dimensions.
        // @return Yw3d_E_InvalidState if the destination volume couldn't be locked.
        Yw3dResult CopyToVolume(const Yw3dBox* srcBox, Yw3dVolume* destVolume, const Yw3dBox* destBox, Yw3dTextureFilter filter);

        // Returns a pointer to the contents of the volume.
        // @param[out] o_ppData receives the pointer to the volume-data.
        // @param[in] i_pBox box that will be locked and accessible. (Pass in 0 to lock the entire volume.)
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_InvalidState if the volume is already locked.
        // @return Yw3d_E_OutOfMemory if memory allocation failed.
        // @note Locking the entire volume is a lot faster than locking a sub-region, because no lock-buffer has to be created and the application may write to the volume directly.
        Yw3dResult LockBox(void** data, const Yw3dBox* box);

        // Unlocks the volume; modifications to its contents will become active.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidState if the volume is not locked.
        Yw3dResult UnlockBox();

        // Returns the format of the volume. Member of the enumeration Yw3dFormat; Yw3d_FMT_R32F, Yw3d_FMT_R32G32F, Yw3d_FMT_R32G32B32F or Yw3d_FMT_R32G32B32A32F.
        Yw3dFormat GetFormat() const;

        // Returns the number of floats of the format, e [1,4].
        uint32_t GetFormatFloats() const;
        
        // Returns the width of the volume in pixels.
        uint32_t GetWidth() const;

        // Returns the height of the volume in pixels.
        uint32_t GetHeight() const;

        // Returns the depth of the volume in pixels.
        uint32_t GetDepth() const;

        // Returns a pointer to the associated device. Calling this function will increase the internal reference count of the device.
        // Failure to call Release() when finished using the pointer will result in a memory leak.
        class Yw3dDevice* AcquireDevice();

    private:
        // Pointer to parent.
        class Yw3dDevice* m_Device;

        // Format of the volume. Member of the enumeration Yw3dFormat; Yw3d_FMT_R32F, Yw3d_FMT_R32G32F, Yw3d_FMT_R32G32B32F or Yw3d_FMT_R32G32B32A32F.
        Yw3dFormat m_Format;

        // Width of the volume in pixels.
        uint32_t m_Width;

        // Height of the volume in pixels.
        uint32_t m_Height;

        // Depth of the volume in pixels.
        uint32_t m_Depth;

        // Width - 1 of the volume in pixels.
        uint32_t m_WidthMin1;

        // Height - 1 of the volume in pixels.
        uint32_t m_HeightMin1;

        // Depth - 1 of the volume in pixels.
        uint32_t m_DepthMin1;

        // True if the whole volume has been locked.
        bool m_LockedComplete;

        // Information about the locked box.
        Yw3dBox	m_PartialLockBox;

        // Not null if a sub-box of the volume has been locked.
        float* m_PartialLockData;

        // Pointer to volume data.
        float* m_Data;
    };
}

#endif // !__YW_3D_VOLUME_H__

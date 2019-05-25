// Add by Yaukey at 2018-05-23.
// YW Soft Renderer 3-dimensional texture class.

#ifndef __YW_3D_VOLUME_TEXTURE_H__
#define __YW_3D_VOLUME_TEXTURE_H__

#include "Yw3dBase.h"
#include "Yw3dTypes.h"
#include "Yw3dBaseTexture.h"

namespace yw
{
    // Yw3dVolumeTexture implements a 3-dimensional texture.
    class Yw3dVolumeTexture : public IYw3dBaseTexture
    {
        friend class Yw3dDevice;

    protected:
        // Accessible by Yw3dDevice which is the only class that may create a volume texture.
        // @param[in] device a pointer to the parent Yw3dDevice-object.
        Yw3dVolumeTexture(class Yw3dDevice* device);

        // Accessible by IBase. The destructor is called when the reference count reaches zero.
        ~Yw3dVolumeTexture();

    protected:
        // Accessible by Yw3dDevice which is the only class that may create a texture.
        // @param[in] width width of the texture to be created in pixels.
        // @param[in] height height of the texture to be created in pixels.
        // @param[in] depth depth of the texture to be created in pixels.
        // @param[in] mipLevels number of mip-levels to be created. Specify 0 to create a full mip-chain.
        // @param[in] format format of the texture to be created. Member of the enumeration Yw3dFormat; Yw3d_FMT_R32F, Yw3d_FMT_R32G32F, Yw3d_FMT_R32G32B32F or Yw3d_FMT_R32G32B32A32F.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_OutOfMemory if memory allocation failed.
        // @return Yw3d_E_InvalidFormat if an invalid format was encountered.
        Yw3dResult Create(uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevels, Yw3dFormat format);

        // Sampling this texture requires 3 floating point coordinates.
        Yw3dTextureSampleInput GetTextureSampleInput() const;

        // Accessible by Yw3dDevice.
        // Samples the texture and returns the looked-up color.
        // @param[out] color receives the color of the pixel to be looked up.
        // @param[in] u u-component of the lookup-vector.
        // @param[in] v v-component of the lookup-vector.
        // @param[in] w w-component of the lookup-vector.
        // @param[in] xGradient partial derivatives of the texture coordinates with respect to the screen-space x coordinate. If 0 the base mip-level will be chosen and the minification filter will be used for texture sampling.
        // @param[in] yGradient partial derivatives of the texture coordinates with respect to the screen-space y coordinate. If 0 the base mip-level will be chosen and the minification filter will be used for texture sampling.
        // @param[in] samplerStates texture sampler states.
        // @return Yw3d_S_OK if the function succeeds.
        Yw3dResult SampleTexture(Vector4& color, float u, float v, float w, const Vector4* xGradient, const Vector4* yGradient, const uint32_t* samplerStates);

    public:
        // Generates mip-sublevels through downsampling (using a box-filter) a given source mip-level.
        // @param[in] srcLevel the mip-level which will be taken as the starting point.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        Yw3dResult GenerateMipSubLevels(uint32_t srcLevel);

        // Clears the texture to a given color.
        // @param[in] mipLevel the mip-level to be cleared.
        // @param[in] color color to clear the texture to.
        // @param[in] box box to restrict clearing to.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if the clear-box exceeds the texture's dimensions.
        Yw3dResult Clear(uint32_t mipLevel, const Vector4& color, const Yw3dBox* box);

        // Returns a pointer to the contents of a given mip-level.
        // @param[in] mipLevel mip-level that is requested, 0 being the largest mip-level.
        // @param[out] data receives the pointer to the texture-data.
        // @param[in] box volume that will be locked and accessible. (Pass in 0 to lock entire texture.)
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_InvalidState if the texture is already locked.
        // @return Yw3d_E_OutOfMemory if memory allocation failed.
        // @note Locking the entire texture is a lot faster than locking a sub-region, because no lock-buffer has to be created and the application may write to the texture directly.
        Yw3dResult LockBox(uint32_t mipLevel, void** data, const Yw3dBox* box);

        // Unlocks the given mip-level; modifications to the texture will become active.
        // @param[in] mipLevel mip-level, 0 being the largest mip-level.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        Yw3dResult UnlockBox(uint32_t mipLevel);

        // Returns a volume-pointer to a given texture mip-level.
        // Calling this function will increase the internal reference count of the volume. Failure to call Release() when finished using the pointer will result in a memory leak.
        // @param[in] mipLevel mip-level, 0 being the largest mip-level.
        class Yw3dVolume* AcquireMipLevel(uint32_t mipLevel);

        //< Returns the format of the texture. Member of the enumeration Yw3dFormat; Yw3d_FMT_R32F, Yw3d_FMT_R32G32F, Yw3d_FMT_R32G32B32F or Yw3d_FMT_R32G32B32A32F.
        Yw3dFormat GetFormat() const;

        // Returns the number of floats of the format, e [1,4].
        uint32_t GetFormatFloats() const;

        // Returns the number of mip-levels this texture consists of.
        uint32_t GetMipLevels() const;

        // Returns the width of the given mip-level in pixels.
        // @param[in] mipLevel the mip-level whose width is requested.
        uint32_t GetWidth(uint32_t mipLevel = 0) const;

        // Returns the height of the given mip-level in pixels.
        // @param[in] mipLevel the mip-level whose height is requested.
        uint32_t GetHeight(uint32_t mipLevel = 0) const;

        // Returns the depth of the given mip-level in pixels.
        // @param[in] mipLevel the mip-level whose depth is requested.
        uint32_t GetDepth(uint32_t mipLevel = 0) const;

    private:
        // Number of mip-levels.
        uint32_t m_MipLevels;

        // Squared dimensions of the base mip-level, used for mip-calculations.
        float m_SquaredWidth;
        float m_SquaredHeight;
        float m_SquaredDepth;

        //  The texture's mip-levels data.
        class Yw3dVolume** m_MipLevelsData;
    };
}

#endif // !__YW_3D_VOLUME_TEXTURE_H__

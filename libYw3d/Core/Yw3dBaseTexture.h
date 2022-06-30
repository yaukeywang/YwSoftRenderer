// Add by Yaukey at 2018-03-31.
// YW Soft Renderer base texture class.

#ifndef __YW_3D_BASE_TEXTURE_H__
#define __YW_3D_BASE_TEXTURE_H__

#include "Yw3dBase.h"
#include "Yw3dTypes.h"

namespace yw
{
    // The base class of texture.
    class IYw3dBaseTexture : public IBase
    {
        friend class Yw3dDevice;

    protected:
        // Texture constructor.
        // @param[in] device a pointer to the parent Yw3dDevice-object.
        IYw3dBaseTexture(class Yw3dDevice* device);

        // Accessible by IBase. The destructor is called when the reference count reaches zero.
        virtual ~IYw3dBaseTexture();

    protected:
        // Returns a member of enum Yw3dTextureSampleInput, which determines the type of input vector SampleTexture() will receive when sampling the texture.
        virtual Yw3dTextureSampleInput GetTextureSampleInput() const = 0;

        // Samples the texture and returns the looked-up color.
        // @param[out] color receives the color of the pixel to be looked up.
        // @param[in] u u-component of the lookup-vector.
        // @param[in] v v-component of the lookup-vector.
        // @param[in] w w-component of the lookup-vector.
        // @param[in] lod level of detail.
        // @param[in] xGradient partial derivatives of the texture coordinates with respect to the screen-space x coordinate. If 0 the base mip-level will be chosen and the minification filter will be used for texture sampling.
        // @param[in] yGradient partial derivatives of the texture coordinates with respect to the screen-space y coordinate. If 0 the base mip-level will be chosen and the minification filter will be used for texture sampling.
        // @param[in] samplerStates texture sampler states.
        // @return Yw3d_S_OK if the function succeeds.
        virtual Yw3dResult SampleTexture(Vector4& color, float u, float v, float w, float lod, const Vector4* xGradient, const Vector4* yGradient, const uint32_t* samplerStates) = 0;

    public:
        // Returns a pointer to the associated device. Calling this function will increase the internal reference count of the device. 
        // Failure to call Release() when finished using the pointer will result in a memory leak.
        class Yw3dDevice* AcquireDevice();

    protected:
        // Pointer to device.
        class Yw3dDevice* m_Device;
    };
}

#endif


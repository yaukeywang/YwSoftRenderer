// Add by Yaukey at 2018-05-23.
// YW Soft Renderer cube map texture class.

#include "stdafx.h"
#include "Yw3dCubeTexture.h"
#include "Yw3dDevice.h"
#include "Yw3dTexture.h"
#include "System/Support/YwUtility.h"

namespace yw
{
    Yw3dCubeTexture::Yw3dCubeTexture(Yw3dDevice* device) :
        IYw3dBaseTexture(device)
    {

    }

    // Accessible by IBase. The destructor is called when the reference count reaches zero.
    Yw3dCubeTexture::~Yw3dCubeTexture()
    {

    }

    Yw3dResult Yw3dCubeTexture::Create(uint32_t edgeLength, uint32_t mipLevels, Yw3dFormat format)
    {
        return Yw3d_E_Unknown;
    }

    Yw3dTextureSampleInput Yw3dCubeTexture::GetTexSampleInput()
    {
        return Yw3d_TSI_Vector;
    }

    Yw3dResult Yw3dCubeTexture::SampleTexture(Vector4& color, float u, float v, float w, const Vector4* xGradient, const Vector4* yGradient, const uint32_t* samplerStates)
    {
        return Yw3d_E_Unknown;
    }

    Yw3dResult Yw3dCubeTexture::GenerateMipSubLevels(uint32_t srcLevel)
    {
        return Yw3d_E_Unknown;
    }

    Yw3dResult Yw3dCubeTexture::LockRect(Yw3dCubeFaces face, uint32_t mipLevel, void** data, const Yw3dRect* rect)
    {
        return Yw3d_E_Unknown;
    }

    Yw3dResult Yw3dCubeTexture::UnlockRect(Yw3dCubeFaces face, uint32_t mipLevel)
    {
        return Yw3d_E_Unknown;
    }

    Yw3dFormat Yw3dCubeTexture::GetFormat() const
    {
        return Yw3d_FMT_R32G32B32A32F;
    }

    uint32_t Yw3dCubeTexture::GetFormatFloats() const
    {
        return -1;
    }

    // Returns the number of mip-levels this texture consists of.
    uint32_t Yw3dCubeTexture::GetMipLevels() const
    {
        return -1;
    }

    uint32_t Yw3dCubeTexture::GetEdgeLength(uint32_t mipLevel)
    {
        return -1;
    }

    Yw3dTexture* Yw3dCubeTexture::AcquireCubeFace(Yw3dCubeFaces face)
    {
        return nullptr;
    }
}

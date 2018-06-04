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
        for (uint32_t faceIdx = Yw3d_CF_Positive_X; faceIdx <= Yw3d_CF_Negative_Z; faceIdx++)
        {
            m_CubeFaces[faceIdx] = nullptr;
        }
    }

    // Accessible by IBase. The destructor is called when the reference count reaches zero.
    Yw3dCubeTexture::~Yw3dCubeTexture()
    {
        for (uint32_t faceIdx = Yw3d_CF_Positive_X; faceIdx <= Yw3d_CF_Negative_Z; faceIdx++)
        {
            YW_SAFE_RELEASE(m_CubeFaces[faceIdx]);
        }
    }

    Yw3dResult Yw3dCubeTexture::Create(uint32_t edgeLength, uint32_t mipLevels, Yw3dFormat format)
    {
        if (0 == edgeLength)
        {
            LOGE(_T("Yw3dCubeTexture::Create: edge length is invalid.\n"));
            return Yw3d_E_InvalidParameters;
        }

        if ((format < Yw3d_FMT_R32F) || (format > Yw3d_FMT_R32G32B32A32F))
        {
            LOGE(_T("Yw3dCubeTexture::Create: invalid format specified.\n"));
            return Yw3d_E_InvalidFormat;
        }

        Yw3dResult resCreate = Yw3d_E_Unknown;
        for (uint32_t faceIdx = Yw3d_CF_Positive_X; faceIdx <= Yw3d_CF_Negative_Z; faceIdx++)
        {
            resCreate = m_Device->CreateTexture(&m_CubeFaces[faceIdx], edgeLength, edgeLength, mipLevels, format);
            if (YW3D_FAILED(resCreate))
            {
                return resCreate;
            }
        }

        return Yw3d_S_OK;
    }

    Yw3dTextureSampleInput Yw3dCubeTexture::GetTextureSampleInput() const
    {
        return Yw3d_TSI_Vector;
    }

    Yw3dResult Yw3dCubeTexture::SampleTexture(Vector4& color, float u, float v, float w, const Vector4* xGradient, const Vector4* yGradient, const uint32_t* samplerStates)
    {
        // Determine face and local u/v coordinates ...
        // source: https://www.nvidia.com/object/cube_map_ogl_tutorial.html

        // major axis 
        // direction     target                              sc     tc    ma 
        // ----------    ---------------------------------   ---    ---   --- 
        //  +rx          GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT   -rz    -ry   rx 
        //  -rx          GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT   +rz    -ry   rx 
        //  +ry          GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT   +rx    +rz   ry 
        //  -ry          GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT   +rx    -rz   ry 
        //  +rz          GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT   +rx    -ry   rz 
        //  -rz          GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT   -rx    -ry   rz

        float cu = 0.0f;
        float cv = 0.0f;
        float invMag = 1.0f;
        Yw3dCubeFaces face;

        const float absU = fabsf(u);
        const float absV = fabsf(v);
        const float absW = fabsf(w);

        if ((absU > absV) && (absU > absW))
        {
            if (u >= 0.0f)
            {
                // Major axis direction: +rx.
                face = Yw3d_CF_Positive_X;
                cu = -w;
                cv = -v;
                invMag = 1.0f / absU;
            }
            else
            {
                // Major axis direction: -rx.
                face = Yw3d_CF_Negative_X;
                cu = w;
                cv = -v;
                invMag = 1.0f / absU;
            }
        }
        else if ((absV > absU) && (absV > absW))
        {
            if (u >= 0.0f)
            {
                // Major axis direction: +ry.
                face = Yw3d_CF_Positive_Y;
                cu = u;
                cv = w;
                invMag = 1.0f / absV;
            }
            else
            {
                // Major axis direction: -ry.
                face = Yw3d_CF_Negative_Y;
                cu = u;
                cv = -w;
                invMag = 1.0f / absV;
            }
        }
        else //if ((absW > absU) && (absW > absV))
        {
            if (w >= 0)
            {
                // Major axis direction: +rz.
                face = Yw3d_CF_Positive_Z;
                cu = u;
                cv = -v;
                invMag = 1.0f / absW;
            }
            else
            {
                // Major axis direction: -rz.
                face = Yw3d_CF_Negative_Z;
                cu = -u;
                cv = -v;
                invMag = 1.0f / absW;
            }
        }

        // s = (sc/|ma| + 1) / 2.
        // t = (tc/|ma| + 1) / 2.
        invMag *= 0.5f;
        const float finalU = cu * invMag + 0.5f;
        const float finalV = cv * invMag + 0.5f;

        return m_CubeFaces[face]->SampleTexture(color, finalU, finalV, 0.0f, xGradient, yGradient, samplerStates);
    }

    Yw3dResult Yw3dCubeTexture::GenerateMipSubLevels(uint32_t srcLevel)
    {
        Yw3dResult resFace = Yw3d_E_Unknown;
        for (uint32_t faceIdx = Yw3d_CF_Positive_X; faceIdx <= Yw3d_CF_Negative_Z; faceIdx++)
        {
            resFace = m_CubeFaces[faceIdx]->GenerateMipSubLevels(srcLevel);
            if (YW3D_FAILED(resFace))
            {
                return resFace;
            }
        }

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dCubeTexture::LockRect(Yw3dCubeFaces face, uint32_t mipLevel, void** data, const Yw3dRect* rect)
    {
        if ((face < 0) || (face >= 6))
        {
            LOGE(_T("Yw3dCubeTexture::LockRect: invalid cube face requested.\n"));
            return Yw3d_E_InvalidParameters;
        }

        return m_CubeFaces[face]->LockRect(mipLevel, data, rect);
    }

    Yw3dResult Yw3dCubeTexture::UnlockRect(Yw3dCubeFaces face, uint32_t mipLevel)
    {
        if ((face < 0) || (face >= 6))
        {
            LOGE(_T("Yw3dCubeTexture::UnlockRect: invalid cube face specified.\n"));
            return Yw3d_E_InvalidParameters;
        }

        return m_CubeFaces[face]->UnlockRect(mipLevel);
    }

    Yw3dFormat Yw3dCubeTexture::GetFormat() const
    {
        return m_CubeFaces[0]->GetFormat();
    }

    uint32_t Yw3dCubeTexture::GetFormatFloats() const
    {
        return m_CubeFaces[0]->GetFormatFloats();
    }

    // Returns the number of mip-levels this texture consists of.
    uint32_t Yw3dCubeTexture::GetMipLevels() const
    {
        return m_CubeFaces[0]->GetMipLevels();
    }

    uint32_t Yw3dCubeTexture::GetEdgeLength(uint32_t mipLevel)
    {
        return m_CubeFaces[0]->GetWidth(mipLevel);
    }

    Yw3dTexture* Yw3dCubeTexture::AcquireCubeFace(Yw3dCubeFaces face)
    {
        if ((face < 0) || (face >= 6))
        {
            LOGE(_T("Yw3dCubeTexture::AcquireCubeFace: invalid cube face requested.\n"));
            return nullptr;
        }

        m_CubeFaces[face]->AddRef();
        return m_CubeFaces[face];
    }
}

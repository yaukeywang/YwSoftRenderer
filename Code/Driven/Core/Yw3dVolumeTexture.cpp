// Add by Yaukey at 2018-05-23.
// YW Soft Renderer 3-dimensional texture class.

#include "stdafx.h"
#include "Yw3dVolumeTexture.h"
#include "Yw3dDevice.h"
#include "Yw3dVolume.h"
#include "System/Support/YwUtility.h"

namespace yw
{
    Yw3dVolumeTexture::Yw3dVolumeTexture(Yw3dDevice* device) : 
        IYw3dBaseTexture(device),
        m_MipLevels(0),
        m_SquaredWidth(0.0f),
        m_SquaredHeight(0.0f),
        m_SquaredDepth(0.0f),
        m_MipLevelsData(nullptr)
    {

    }

    Yw3dVolumeTexture::~Yw3dVolumeTexture()
    {
        for (uint32_t level = 0; level < m_MipLevels; level++)
        {
            YW_SAFE_RELEASE(m_MipLevelsData[level]);
        }

        YW_SAFE_DELETE_ARRAY(m_MipLevelsData);
    }
}

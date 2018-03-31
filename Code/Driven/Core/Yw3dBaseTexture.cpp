// Add by Yaukey at 2018-03-31.
// YW Soft Renderer base texture class.

#include "stdafx.h"
#include "Yw3dBaseTexture.h"
#include "Yw3dDevice.h"

namespace yw
{
    IYw3dBaseTexture::IYw3dBaseTexture(Yw3dDevice* device) : 
        m_Device(device)
    {
        m_Device->AddRef();
    }

    // Accessible by IBase. The destructor is called when the reference count reaches zero.
    IYw3dBaseTexture::~IYw3dBaseTexture()
    {
        YW_SAFE_RELEASE(m_Device);
    }

    Yw3dDevice* IYw3dBaseTexture::AquireDevice()
    {
        if (nullptr != m_Device)
        {
            m_Device->AddRef();
        }

        return m_Device;
    }
}

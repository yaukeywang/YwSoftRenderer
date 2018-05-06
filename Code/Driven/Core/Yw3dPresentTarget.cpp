// Add by Yaukey at 2018-03-31.
// YW Soft Renderer present target class.

#include "stdafx.h"
#include "Yw3dPresentTarget.h"
#include "Yw3dDevice.h"

namespace yw
{
    IYw3dPresentTarget::IYw3dPresentTarget(Yw3dDevice* device) :
        m_Device(device)
    {
        // Note: cannot add a reference to parent device or the presenttarget will never be freed.
        // m_Device->AddRef();
    }

    IYw3dPresentTarget::~IYw3dPresentTarget()
    {
        // Note: see note in constructor.
        // SAFE_RELEASE(m_Device);
    }

    Yw3dDevice* IYw3dPresentTarget::AquireDevice()
    {
        if (nullptr != m_Device)
        {
            m_Device->AddRef();
        }

        return m_Device;
    }
}

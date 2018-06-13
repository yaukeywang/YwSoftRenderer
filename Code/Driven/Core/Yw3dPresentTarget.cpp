// Add by Yaukey at 2018-03-31.
// YW Soft Renderer present target class.

#include "stdafx.h"
#include "Yw3dPresentTarget.h"
#include "Yw3dDevice.h"

namespace yw
{
    // ------------------------------------------------------------------
    // Present target base.

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

    Yw3dDevice* IYw3dPresentTarget::AcquireDevice()
    {
        if (nullptr != m_Device)
        {
            m_Device->AddRef();
        }

        return m_Device;
    }

    // ------------------------------------------------------------------
    // Platform-dependent code.

    // ------------------------------------------------------------------
    // Windows platform.
#ifdef WIN32

    Yw3dPresentTargetWindows::Yw3dPresentTargetWindows(Yw3dDevice* device) : 
        IYw3dPresentTarget(device),
        m_DirectDraw(nullptr),
        m_DirectDrawClipper(nullptr),
        m_DDSurfaceLost(false)

    {
        m_DirectDrawSurfaces[0] = nullptr;
        m_DirectDrawSurfaces[1] = nullptr;

        m_16bitMaxVal[0] = 0;
        m_16bitMaxVal[1] = 0;
        m_16bitMaxVal[2] = 0;

        m_16bitShift[0] = 0;
        m_16bitShift[1] = 0;
        m_16bitShift[2] = 0;
    }

    Yw3dPresentTargetWindows::~Yw3dPresentTargetWindows()
    {
        // Get device parameters.
        Yw3dDeviceParameters deviceParameters = m_Device->GetDeviceParameters();

        // Release DirectDraw.
        if (nullptr != m_DirectDrawSurfaces[1])
        {
            if (deviceParameters.windowed)
            {
                m_DirectDrawSurfaces[1]->Release();
            }

            m_DirectDrawSurfaces[1] = nullptr;
        }

        YW_SAFE_RELEASE(m_DirectDrawSurfaces[0]);
        YW_SAFE_RELEASE(m_DirectDrawClipper);

        if (nullptr != m_DirectDraw)
        {
            if (deviceParameters.windowed)
            {
                m_DirectDraw->RestoreDisplayMode();
                m_DirectDraw->SetCooperativeLevel(deviceParameters.deviceWindow, DDSCL_NORMAL);
            }

            YW_SAFE_RELEASE(m_DirectDraw);
        }
    }

    Yw3dResult Yw3dPresentTargetWindows::Create()
    {
        return Yw3d_E_Unknown;
    }

    Yw3dResult Yw3dPresentTargetWindows::Present(const float* source, uint32_t floats)
    {
        return Yw3d_E_Unknown;
    }

    void Yw3dPresentTargetWindows::ProcessBits(uint32_t mask, uint16_t& lowBit, uint16_t& numberBits)
    {

    }

#endif
}

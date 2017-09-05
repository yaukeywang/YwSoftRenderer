// Add by yaukey at 2017-09-04.
// Input device base class.

#include "stdafx.h"
#include "InputDevice.h"
#include "InputManager.h"
#include "Utility.h"

namespace yw
{
    InputDevice::InputDevice(InputManager* inputManager, LPCTSTR deviceName, const GUID& deviceGUID) :
        m_InputManager(inputManager), m_DeviceName(deviceName), m_GUID(deviceGUID)
    {

    }

    InputDevice::~InputDevice()
    {
        Unacquire();
        YW_SAFE_RELEASE(m_Device);
    }

    bool InputDevice::CreateInputDevice(const DIDATAFORMAT* dataFormat, DWORD cooperativeLevel, DWORD bufferSize)
    {
        if (nullptr == dataFormat)
        {
            LOGE(_T("Create input device failed: Need data format!"));
            return false;
        }

        // Create a device.
        HRESULT hr = GetInputManager()->GetDirectInput()->CreateDevice(m_GUID, &m_Device, nullptr);
        if (FAILED(hr))
        {
            LOGE(_T("Create input device failed: CreateDevice failed!"));
            return false;
        }

        // Set data format.
        hr = m_Device->SetDataFormat(dataFormat);
        if (FAILED(hr))
        {
            LOGE(_T("Create input device failed: SetDataFormat failed!"));
            return false;
        }

        // Set cooperative level.
        m_DataFormat = dataFormat;
        m_CooperativeLevel = cooperativeLevel;
        hr = m_Device->SetCooperativeLevel(GetInputManager()->GetMainWindowHandle(), m_CooperativeLevel);
        if (FAILED(hr))
        {
            LOGE(_T("Create input device failed: SetCooperativeLevel failed!"));
            return false;
        }

        DIPROPDWORD dipdw = { { sizeof(DIPROPDWORD), sizeof(DIPROPHEADER), 0, DIPH_DEVICE }, bufferSize };
        hr = m_Device->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
        if (FAILED(hr))
        {
            LOGE(_T("Create input device failed: SetProperty failed!"));
            return false;
        }

        return true;
    }

    bool InputDevice::Acquire()
    {
        if (nullptr == m_Device)
        {
            LOGE(_T("Acquire input device failed: Not initialized!"));
            return false;
        }

        HRESULT hr = m_Device->Acquire();
        if (FAILED(hr))
        {
            LOGE(_T("Acquire input device failed: Acquire failed!"));
            return false;
        }

        return true;
    }

    bool InputDevice::Unacquire()
    {
        if (nullptr == m_Device)
        {
            LOGE(_T("Unacquire input device failed: Not initialized!"));
            return false;
        }

        HRESULT hr = m_Device->Unacquire();
        if (FAILED(hr))
        {
            LOGE(_T("Unacquire input device failed: Unacquire failed!"));
            return false;
        }

        return true;
    }
}

// Add by Yaukey at 2018-10-25.
// YW Soft Renderer application framework for input module implementation.

#include "YwInput.h"
#include "YwApplication.h"

// ------------------------------------------------------------------
// Platform-dependent code.

// ------------------------------------------------------------------
// Windows platform.
#if defined(_WIN32) || defined(WIN32)

namespace yw
{
    YwInputWindows::YwInputWindows(IApplication* application) : 
        IInput(application)
    {
        m_DirectInput = nullptr;
        m_DIDeviceKeyboard = nullptr;
        m_DIDeviceMouse = nullptr;

        ZeroMemory(m_Keys, sizeof(m_Keys));
        ZeroMemory(&m_MouseState, sizeof(m_MouseState));
    }

    YwInputWindows::~YwInputWindows()
    {
        UnacquireDevices();

        YW_SAFE_RELEASE(m_DIDeviceMouse);
        YW_SAFE_RELEASE(m_DIDeviceKeyboard);
        YW_SAFE_RELEASE(m_DirectInput);
    }

    bool YwInputWindows::Initialize()
    {
        // Initialize DirectInput and create DI devices.
        if (FAILED(DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_DirectInput, nullptr)))
        {
            return false;
        }

        // Create keyboard device.
        if (FAILED(m_DirectInput->CreateDevice(GUID_SysKeyboard, &m_DIDeviceKeyboard, nullptr)))
        {
            return false;
        }

        m_DIDeviceKeyboard->SetDataFormat(&c_dfDIKeyboard);
        m_DIDeviceKeyboard->SetCooperativeLevel(m_Application->GetWindowHandle(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

        // Create mouse device.
        if (FAILED(m_DirectInput->CreateDevice(GUID_SysMouse, &m_DIDeviceMouse, nullptr)))
        {
            return false;
        }

        m_DIDeviceMouse->SetDataFormat(&c_dfDIMouse);
        m_DIDeviceMouse->SetCooperativeLevel(m_Application->GetWindowHandle(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

        // Acquire devices.
        AcquireDevices();

        return true;
    }

    void YwInputWindows::Update()
    {
        // Check keyboard state.
        if (FAILED(m_DIDeviceKeyboard->GetDeviceState(sizeof(m_Keys), (void*)m_Keys)))
        {
            // Found not acquired, try again...
            m_DIDeviceKeyboard->Acquire();
            m_DIDeviceKeyboard->GetDeviceState(sizeof(m_Keys), (void*)m_Keys);
        }

        // Check mouse state.
        if (FAILED(m_DIDeviceMouse->GetDeviceState(sizeof(DIMOUSESTATE), (void*)&m_MouseState)))
        {
            // Found not acquired, try again...
            m_DIDeviceMouse->Acquire();
            m_DIDeviceMouse->GetDeviceState(sizeof(DIMOUSESTATE), (void*)&m_MouseState);
        }
    }

    void YwInputWindows::AcquireDevices()
    {
        m_DIDeviceKeyboard->Acquire();
        m_DIDeviceMouse->Acquire();
    }

    void YwInputWindows::UnacquireDevices()
    {
        m_DIDeviceKeyboard->Unacquire();
        m_DIDeviceMouse->Unacquire();
    }
}

#endif // End of Windows platform.

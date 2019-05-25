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

// ------------------------------------------------------------------
// Linux platform.
#if defined(LINUX_X11) || defined(_LINUX)

namespace yw
{
    YwInputLinux::YwInputLinux(IApplication* application) :
        IInput(application)
    {
    }
    
    YwInputLinux::~YwInputLinux()
    {
    }
    
    bool YwInputLinux::Initialize()
    {
        return false;
    }
    
    void YwInputLinux::Update()
    {
        
    }
    
    bool YwInputLinux::KeyDown(char keyCode)
    {
        return false;
    }
    
    bool YwInputLinux::KeyUp(char keyCode)
    {
        return false;
    }
    
    bool YwInputLinux::MouseButtonDown(uint32_t keyCode)
    {
        return false;
    }
    
    bool YwInputLinux::MouseButtonUp(uint32_t keyCode)
    {
        return false;
    }
    
    void YwInputLinux::GetMouseMovement(int32_t* deltaX, int32_t* deltaY) const
    {
        
    }
    
    int YwInputLinux::GetMouseWheelMovement() const
    {
        return 0;
    }
}

#endif

// ------------------------------------------------------------------
// Mac OSX platform.
#if defined(_MAC_OSX)

namespace yw
{
    YwInputMacOSX::YwInputMacOSX(IApplication* application) :
        IInput(application)
    {
    }
    
    YwInputMacOSX::~YwInputMacOSX()
    {
    }
    
    bool YwInputMacOSX::Initialize()
    {
        return false;
    }
    
    void YwInputMacOSX::Update()
    {
        
    }
    
    bool YwInputMacOSX::KeyDown(char keyCode)
    {
        return false;
    }
    
    bool YwInputMacOSX::KeyUp(char keyCode)
    {
        return false;
    }
    
    bool YwInputMacOSX::MouseButtonDown(uint32_t keyCode)
    {
        return false;
    }
    
    bool YwInputMacOSX::MouseButtonUp(uint32_t keyCode)
    {
        return false;
    }
    
    void YwInputMacOSX::GetMouseMovement(int32_t* deltaX, int32_t* deltaY) const
    {
        
    }
    
    int YwInputMacOSX::GetMouseWheelMovement() const
    {
        return 0;
    }
}

#endif

// ------------------------------------------------------------------
// Amiga OS 4 platform.
#if defined(__amigaos4__) || (_AMIGAOS4)

namespace yw
{
    YwInputAmigaOS4::YwInputAmigaOS4(IApplication* application) :
        IInput(application)
    {
    }
    
    YwInputAmigaOS4::~YwInputAmigaOS4()
    {
    }
    
    bool YwInputAmigaOS4::Initialize()
    {
        return false;
    }
    
    void YwInputAmigaOS4::Update()
    {
        
    }
    
    bool YwInputAmigaOS4::KeyDown(char keyCode)
    {
        return false;
    }
    
    bool YwInputAmigaOS4::KeyUp(char keyCode)
    {
        return false;
    }
    
    bool YwInputMacOSX::MouseButtonDown(uint32_t keyCode)
    {
        return false;
    }
    
    bool YwInputMacOSX::MouseButtonUp(uint32_t keyCode)
    {
        return false;
    }
    
    void YwInputAmigaOS4::GetMouseMovement(int32_t* deltaX, int32_t* deltaY) const
    {
        
    }
    
    int YwInputAmigaOS4::GetMouseWheelMovement() const
    {
        return 0;
    }
}

#endif

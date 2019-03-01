// Add by Yaukey at 2018-10-25.
// YW Soft Renderer application framework for input module implementation.

#ifndef __YW_INPUT_H__
#define __YW_INPUT_H__

#include "YwBaseInput.h"

// ------------------------------------------------------------------
// Platform-dependent code.

// ------------------------------------------------------------------
// Windows platform.
#if defined(_WIN32) || defined(WIN32)

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif  // !DIRECTINPUT_VERSION

#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

namespace yw
{
    // The input class for windows.
    class YwInputWindows : public IInput
    {
        friend class IApplication;
        friend class CApplication;

    protected:
        // Constructor.
        YwInputWindows(class IApplication* application);

        // Destructor.
        ~YwInputWindows();

    protected:
        bool Initialize();

    public:
        // Update logic.
        void Update();

        // Get if key down.
        inline bool KeyDown(char keyCode)
        {
            return (m_Keys[keyCode] & 0x80) ? true : false;
        }

        // Get if key up.
        inline bool KeyUp(char keyCode)
        {
            return (m_Keys[keyCode] & 0x80) ? false : true;
        }

        // Get if mouse down.
        inline bool MouseButtonDown(uint32_t keyCode) 
        {
            return (m_MouseState.rgbButtons[keyCode] & 0x80) ? true : false;
        }

        // Get if mouse up.
        inline bool MouseButtonUp(uint32_t keyCode)
        {
            return (m_MouseState.rgbButtons[keyCode] & 0x80) ? false : true;
        }
         
        // Get mouse movement.
        inline void GetMouseMovement(int32_t* deltaX, int32_t* deltaY) const
        {
            *deltaX = m_MouseState.lX;
            *deltaY = m_MouseState.lY;
        }

        // Get mouse wheel movement.
        inline int GetMouseWheelMovement() const
        {
            return m_MouseState.lZ;
        }

    private:
        // Acquire input devices.
        void AcquireDevices();

        // Unacquire devices.
        void UnacquireDevices();

    private:
        // The direct input object pointer.
        LPDIRECTINPUT8 m_DirectInput;

        // The keyboard device.
        LPDIRECTINPUTDEVICE8 m_DIDeviceKeyboard;

        // The mouse device.
        LPDIRECTINPUTDEVICE8 m_DIDeviceMouse;

        // The key map.
        char m_Keys[256];

        // The mouse state.
        DIMOUSESTATE m_MouseState;
    };
}

#endif // End of Windows platform.

// ------------------------------------------------------------------
// Linux platform.
#if defined(LINUX_X11) || defined(_LINUX)

namespace yw
{

}

#endif

// ------------------------------------------------------------------
// Mac OSX platform.
#if defined(_MAC_OSX)

namespace yw
{

}

#endif

#endif // !__YW_INPUT_H__

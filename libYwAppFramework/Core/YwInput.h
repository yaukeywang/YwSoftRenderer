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
        // @return true if successful, false if not.
        bool Initialize();

    public:
        // Update logic.
        void Update();

        // Get if key down.
        // @return true if key down, false is not.
        bool KeyDown(char keyCode);

        // Get if key up.
        // @return true if key up, false is not.
        bool KeyUp(char keyCode);

        // Get if mouse down.
        // @return true if mouse button down, false is not.
        bool MouseButtonDown(uint32_t keyCode);

        // Get if mouse up.
        // @return true if mouse button up, false is not.
        bool MouseButtonUp(uint32_t keyCode);
         
        // Get mouse movement.
        void GetMouseMovement(int32_t* deltaX, int32_t* deltaY) const;

        // Get mouse wheel movement.
        // @return mouse delta movement, maybe negative
        int32_t GetMouseWheelMovement() const;

        // Get mouse current position in window coordinate.
        // @param[in,out] posX current position x in window.
        // @param[in,out] posY current position y in window.
        void GetMousePosition(int32_t* posX, int32_t* posY) const;

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
    // The input class for windows.
    class YwInputLinux : public IInput
    {
        friend class IApplication;
        friend class CApplication;
        
    protected:
        // Constructor.
        YwInputLinux(class IApplication* application);
        
        // Destructor.
        ~YwInputLinux();
        
    protected:
        // Initialize the input system.
        // @return true if successful, false if not.
        bool Initialize();
        
    public:
        // Update logic.
        void Update();
        
        // Get if key down.
        // @return true if key down, false is not.
        bool KeyDown(char keyCode);
        
        // Get if key up.
        // @return true if key up, false is not.
        bool KeyUp(char keyCode);
        
        // Get if mouse down.
        // @return true if mouse button down, false is not.
        bool MouseButtonDown(uint32_t keyCode);
        
        // Get if mouse up.
        // @return true if mouse button up, false is not.
        bool MouseButtonUp(uint32_t keyCode);
        
        // Get mouse movement.
        void GetMouseMovement(int32_t* deltaX, int32_t* deltaY) const;
        
        // Get mouse wheel movement.
        // @return mouse delta movement, maybe negative
        int32_t GetMouseWheelMovement() const;

        // Get mouse current position in window coordinate.
        // @param[in,out] posX current position x in window.
        // @param[in,out] posY current position y in window.
        void GetMousePosition(int32_t* posX, int32_t* posY) const;
    };
}

#endif

// ------------------------------------------------------------------
// Mac OSX platform.
#if defined(_MAC_OSX)

namespace yw
{
    // The input class for windows.
    class YwInputMacOSX : public IInput
    {
        friend class IApplication;
        friend class CApplication;
        
    protected:
        // Constructor.
        YwInputMacOSX(class IApplication* application);
        
        // Destructor.
        ~YwInputMacOSX();
        
    protected:
        // Initialize the input system.
        // @return true if successful, false if not.
        bool Initialize();
        
    public:
        // Update logic.
        void Update();
        
        // Get if key down.
        // @return true if key down, false is not.
        bool KeyDown(char keyCode);
        
        // Get if key up.
        // @return true if key up, false is not.
        bool KeyUp(char keyCode);
        
        // Get if mouse down.
        // @return true if mouse button down, false is not.
        bool MouseButtonDown(uint32_t keyCode);
        
        // Get if mouse up.
        // @return true if mouse button up, false is not.
        bool MouseButtonUp(uint32_t keyCode);
        
        // Get mouse movement.
        void GetMouseMovement(int32_t* deltaX, int32_t* deltaY) const;
        
        // Get mouse wheel movement.
        // @return mouse delta movement, maybe negative
        int32_t GetMouseWheelMovement() const;

        // Get mouse current position in window coordinate.
        // @param[in,out] posX current position x in window.
        // @param[in,out] posY current position y in window.
        void GetMousePosition(int32_t* posX, int32_t* posY) const;
    };
}

#endif

// ------------------------------------------------------------------
// Amiga OS 4 platform.
#if defined(__amigaos4__) || (_AMIGAOS4)

namespace yw
{
    // The input class for windows.
    class YwInputAmigaOS4 : public IInput
    {
        friend class IApplication;
        friend class CApplication;
        
    protected:
        // Constructor.
        YwInputAmigaOS4(class IApplication* application);
        
        // Destructor.
        ~YwInputAmigaOS4();
        
    protected:
        // Initialize the input system.
        // @return true if successful, false if not.
        bool Initialize();
        
    public:
        // Update logic.
        void Update();
        
        // Get if key down.
        // @return true if key down, false is not.
        bool KeyDown(char keyCode);
        
        // Get if key up.
        // @return true if key up, false is not.
        bool KeyUp(char keyCode);
        
        // Get if mouse down.
        // @return true if mouse button down, false is not.
        bool MouseButtonDown(uint32_t keyCode);
        
        // Get if mouse up.
        // @return true if mouse button up, false is not.
        bool MouseButtonUp(uint32_t keyCode);
        
        // Get mouse movement.
        void GetMouseMovement(int32_t* deltaX, int32_t* deltaY) const;
        
        // Get mouse wheel movement.
        // @return mouse delta movement, maybe negative
        int32_t GetMouseWheelMovement() const;

        // Get mouse current position in window coordinate.
        // @param[in,out] posX current position x in window.
        // @param[in,out] posY current position y in window.
        void GetMousePosition(int32_t* posX, int32_t* posY) const;
    };
}

#endif

#endif // !__YW_INPUT_H__

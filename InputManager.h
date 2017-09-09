// Add by yaukey at 2017-08-30.
// All input management.

#ifndef __INPUT_MANAGER_H__
#define __INPUT_MANAGER_H__

#include "Base.h"
#include <dinput.h>
#include <dinputd.h>

namespace yw
{
    class InputDevice;
    typedef std::vector<InputDevice*> InputDeviceList;

    enum InputKeyId
    {
        IPKI_INVALID = -1,

        IPKI_MOUSE_LEFT,
        IPKI_MOUSE_RIGHT,
        IPKI_MOUSE_WHEEL,

        IPKI_MOUSE_3,
        IPKI_MOUSE_4,
        IPKI_MOUSE_5,
        IPKI_MOUSE_6,
        IPKI_MOUSE_7,

        IPKI_MOUSE_MOVE,
        IPKI_MOUSE_MOVE_WHEEL,

        IPKI_END
    };

    enum InputKeyState
    {
        IPKS_INVALID = -1,

        IPKS_PRESSED,
        IPKS_RELEASED,

        IPKS_END
    };

    struct InputKey
    {
        InputKeyId m_Id;
        InputKeyState m_State;
        int32_t m_X;
        int32_t m_Y;
        int32_t m_Z;

        InputKey(InputKeyId inputKeyId) : m_Id(inputKeyId), m_State(IPKS_INVALID), m_X(0), m_Y(0), m_Z(0) {}
    };

    class InputManager
    {
    public:
        InputManager();
        virtual ~InputManager();

    public:
        bool Initialize(HWND mainWindow);
        void Release();

        void Update();
        void Dispatch();

        HWND GetMainWindowHandle() const { return m_MainWindow; }
        LPDIRECTINPUT8 GetDirectInput() const { return m_DirectInput; }

        void AddInputDevice(InputDevice* inputDevice);
        void OnInputKey(const InputKey* inputKey);

    private:
        // The handle of the main window.
        HWND m_MainWindow;

        // The direct input device.
        LPDIRECTINPUT8 m_DirectInput;

        // The input device list.
        InputDeviceList m_InputDevices;
    };
}

#endif // !__INPUT_MANAGER_H__

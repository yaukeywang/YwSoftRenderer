// Add by yaukey at 2017-09-13.
// Input device keyboard class.

#ifndef __YW_INPUT_DEVICE_KEYBOARD_H__
#define __YW_INPUT_DEVICE_KEYBOARD_H__

#include "System/Input/YwInputDevice.h"
#include "System/Input/YwInputManager.h"

namespace yw
{
    class InputDevicekeyboard : public InputDevice
    {
    public:
        InputDevicekeyboard(InputManager* inputManager);
        ~InputDevicekeyboard();

    public:
        virtual bool Initialize();
        virtual void Release();
        virtual void Update();

    private:
        void BuildKeyboardId();
        void ProcessKeyboardEvent(DWORD key, InputKeyState newState);

    private:
        std::vector<InputKey> m_KeyStates;
    };
}

#endif // !__YW_INPUT_DEVICE_KEYBOARD_H__

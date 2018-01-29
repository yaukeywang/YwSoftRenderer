// Add by yaukey at 2017-09-05.
// Input device mouse class.

#ifndef __YW_INPUT_DEVICE_MOUSE_H__
#define __YW_INPUT_DEVICE_MOUSE_H__

#include "System/Input/YwInputDevice.h"
#include "System/Input/YwInputManager.h"

namespace yw
{
    class InputDeviceMouse : public InputDevice
    {
    public:
        InputDeviceMouse(InputManager* inputManager);
        ~InputDeviceMouse();

    public:
        virtual bool Initialize();
        virtual void Release();
        virtual void Update();

    protected:
        void BuildMouseId();
        void ProcessClickEvent(InputKey* inputKey, InputKeyState newState);
        void ProcessMoveEvent(InputKey* inputKey, int32_t newValueX, int32_t newValueY);
        void ProcessWheelEvent(InputKey* inputKey, int32_t newValueZ);

    protected:
        int32_t m_X;
        int32_t m_Y;
        std::vector<InputKey> m_MouseStates;
    };
}

#endif // !__YW_INPUT_DEVICE_MOUSE_H__

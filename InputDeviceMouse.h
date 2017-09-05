// Add by yaukey at 2017-09-05.
// Input device mouse class.

#ifndef __INPUT_DEVICE_MOUSE_H__
#define __INPUT_DEVICE_MOUSE_H__

#include "InputDevice.h"
#include "InputManager.h"

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
    };
}

#endif // !__INPUT_DEVICE_MOUSE_H__

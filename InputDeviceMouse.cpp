// Add by yaukey at 2017-09-05.
// Input device mouse class.

#include "stdafx.h"
#include "InputDeviceMouse.h"

namespace yw
{
    InputDeviceMouse::InputDeviceMouse(InputManager* inputManager) :
        InputDevice(inputManager, _T("Mouse"), GUID_SysMouse)
    {

    }

    InputDeviceMouse::~InputDeviceMouse()
    {

    }

    bool InputDeviceMouse::Initialize()
    {
        if (!CreateInputDevice(&c_dfDIMouse2, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND, 4096))
        {
            return false;
        }

        // Set mouse axis mode.
        DIPROPDWORD dipdw = { { sizeof(DIPROPDWORD), sizeof(DIPROPHEADER), 0, DIPH_DEVICE }, DIPROPAXISMODE_REL };
        HRESULT hr = GetDevice()->SetProperty(DIPROP_AXISMODE, &dipdw.diph);
        if (FAILED(hr))
        {
            return false;
        }

        if (!Acquire())
        {
            return false;
        }

        return true;
    }

    void InputDeviceMouse::Release()
    {
        
    }

    void InputDeviceMouse::Update()
    {
        LPDIRECTINPUTDEVICE8 device = GetDevice();
        if (nullptr == device)
        {
            return;
        }

        // Get mouse state.
        DIMOUSESTATE2 dims;
        memset(&dims, 0, sizeof(dims));

        HRESULT hr = device->GetDeviceState(sizeof(DIMOUSESTATE2), (LPVOID)&dims);
        if (FAILED(hr))
        {
            // Try to acquire agein.
            if (!Acquire())
            {
                return;
            }

            hr = device->GetDeviceState(sizeof(DIMOUSESTATE2), (LPVOID)&dims);
            if (FAILED(hr))
            {
                return;
            }
        }

        // Get mouse data.
        float deltaX = (float)dims.lX;
        float deltaY = (float)dims.lY;
        float deltaZ = (float)dims.lZ;

        // Get mouse buttons down.
    }
}

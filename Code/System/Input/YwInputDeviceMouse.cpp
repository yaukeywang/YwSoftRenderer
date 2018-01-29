// Add by yaukey at 2017-09-05.
// Input device mouse class.

#include "stdafx.h"
#include "YwInputDeviceMouse.h"
#include "System/Support/YwUtility.h"

namespace yw
{
    InputDeviceMouse::InputDeviceMouse(InputManager* inputManager) :
        InputDevice(inputManager, _T("Mouse"), GUID_SysMouse),
        m_X(0),
        m_Y(0)
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

        BuildMouseId();

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

        if (SUCCEEDED(hr))
        {
            // Get mouse data.
            int32_t deltaX = (int32_t)dims.lX;
            int32_t deltaY = (int32_t)dims.lY;
            int32_t deltaZ = (int32_t)dims.lZ;
            //LOGF(_T("Mouse x:%d, y:%d, z:%d"), (int)dims.lX, (int)dims.lY, (int)dims.lZ);

            m_X += deltaX;
            m_Y += deltaY;

            // Get mouse buttons click event.
            for (int8_t i = 0; i < 8; i++)
            {
                InputKeyState newState = (dims.rgbButtons[i] & 0x80) ? IPKS_PRESSED : IPKS_RELEASED;
                void ProcessClickEvent(InputKey* inputKey, InputKeyState newState);
                (&(m_MouseStates[i]), newState);
            }

            ProcessMoveEvent(&(m_MouseStates[8]), deltaX, deltaY);
            ProcessWheelEvent(&(m_MouseStates[9]), deltaZ);
        }
    }

    void InputDeviceMouse::BuildMouseId()
    {
        m_MouseStates.clear();
        m_MouseStates.assign(0xff, InputKey(IPKI_INVALID));

        m_MouseStates[0] = InputKey(IPKI_MOUSE_LEFT);
        m_MouseStates[1] = InputKey(IPKI_MOUSE_RIGHT);
        m_MouseStates[2] = InputKey(IPKI_MOUSE_WHEEL);
        m_MouseStates[3] = InputKey(IPKI_MOUSE_3);
        m_MouseStates[4] = InputKey(IPKI_MOUSE_4);
        m_MouseStates[5] = InputKey(IPKI_MOUSE_5);
        m_MouseStates[6] = InputKey(IPKI_MOUSE_6);
        m_MouseStates[7] = InputKey(IPKI_MOUSE_7);
        m_MouseStates[8] = InputKey(IPKI_MOUSE_MOVE);
        m_MouseStates[9] = InputKey(IPKI_MOUSE_MOVE_WHEEL);
    }

    void InputDeviceMouse::ProcessClickEvent(InputKey* inputKey, InputKeyState newState)
    {
        if (newState == inputKey->m_State)
        {
            return;
        }

        InputManager* inputManager = GetInputManager();

        // Mouse click, pressed or released.
        inputKey->m_State = newState;
        inputKey->m_X = 0;
        inputKey->m_Y = 0;
        inputKey->m_Z = 0;
        inputManager->OnInputKey(inputKey);

        // Debug test.
        //if (IPKS_PRESSED == newState)
        //{
        //    LOGF(_T("Mouse Button %d is Pressed."), inputKey->m_Id);
        //}
        //else if(IPKS_RELEASED == newState)
        //{
        //    LOGF(_T("Mouse Button %d is Released."), inputKey->m_Id);
        //}
    }

    void InputDeviceMouse::ProcessMoveEvent(InputKey* inputKey, int32_t newValueX, int32_t newValueY)
    {
        if ((0 == newValueX) && (0 == newValueY))
        {
            return;
        }

        // Mouse move.
        InputManager* inputManager = GetInputManager();

        inputKey->m_State = IPKS_INVALID;
        inputKey->m_X = m_X;
        inputKey->m_Y = m_Y;
        inputKey->m_Z = 0;
        inputManager->OnInputKey(inputKey);

        // Debug test.
        //LOGF(_T("Mouse Moving: (%d, %d)."), m_X, m_Y);
    }

    void InputDeviceMouse::ProcessWheelEvent(InputKey* inputKey, int32_t newValueZ)
    {
        // Mouse wheel.
        if (0 != newValueZ)
        {
            InputManager* inputManager = GetInputManager();

            inputKey->m_State = IPKS_INVALID;
            inputKey->m_X = 0;
            inputKey->m_Y = 0;
            inputKey->m_Z = newValueZ;
            inputManager->OnInputKey(inputKey);

            // Debug test.
            //LOGF(_T("Mouse Wheel: %d."), (int)newValueZ);
        }
    }
}

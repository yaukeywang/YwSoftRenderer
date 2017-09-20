// Add by yaukey at 2017-09-13.
// Input device keyboard class.

#include "stdafx.h"
#include "InputDevicekeyboard.h"
#include "Utility.h"

namespace yw
{
    InputDevicekeyboard::InputDevicekeyboard(InputManager* inputManager) :
        InputDevice(inputManager, _T("Keyboard"), GUID_SysKeyboard)
    {
    }

    InputDevicekeyboard::~InputDevicekeyboard()
    {
    }

    bool InputDevicekeyboard::Initialize()
    {
        if (!CreateInputDevice(&c_dfDIKeyboard, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND | DISCL_NOWINKEY, 32))
        {
            return false;
        }

        if (!Acquire())
        {
            return false;
        }

        BuildKeyboardId();

        return true;
    }

    void InputDevicekeyboard::Release()
    {

    }

    void InputDevicekeyboard::Update()
    {
        LPDIRECTINPUTDEVICE8 device = GetDevice();
        if (nullptr == device)
        {
            return;
        }

        DIDEVICEOBJECTDATA rgdod[256];
        DWORD dwItems = 256;
        HRESULT hr = device->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), rgdod, &dwItems, 0);
        if (FAILED(hr))
        {
            if (!Acquire())
            {
                return;
            }

            dwItems = 256;
            hr = device->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), rgdod, &dwItems, 0);
            if (FAILED(hr))
            {
                return;
            }
        }

        if (SUCCEEDED(hr))
        {
            for (DWORD i = 0; i < dwItems; i++)
            {
                DWORD key = rgdod[i].dwOfs;
                InputKeyState newState = ((rgdod[i].dwData & 0x80) != 0) ? IPKS_PRESSED : IPKS_RELEASED;
                ProcessKeyboardEvent(key, newState);
            }
        }
    }

    void InputDevicekeyboard::BuildKeyboardId()
    {
        m_KeyStates.clear();
        m_KeyStates.assign(0xff, InputKey(IPKI_INVALID));

        for (uint8_t i = IPKI_KEY_DEFINE_BEGIN; i < IPKI_KEY_DEFINE_END; i++)
        {
            m_KeyStates[i] = InputKey((InputKeyId)i);
        }
    }

    void InputDevicekeyboard::ProcessKeyboardEvent(DWORD key, InputKeyState newState)
    {
        if ((key < IPKI_KEY_DEFINE_BEGIN) || (key > IPKI_KEY_DEFINE_END))
        {
            return;
        }

        InputKey* inputKey = &m_KeyStates[key];
        if ((IPKS_RELEASED == newState) && (IPKS_PRESSED != inputKey->m_State))
        {
            return;
        }

        inputKey->m_State = newState;
        GetInputManager()->OnInputKey(inputKey);

        // Debug test.
        //if (IPKS_PRESSED == newState)
        //{
        //    LOGF(_T("Keyboard %d is Pressed."), inputKey->m_Id);
        //}
        //else if (IPKS_RELEASED == newState)
        //{
        //    LOGF(_T("Keyboard %d is Released."), inputKey->m_Id);
        //}
    }
}

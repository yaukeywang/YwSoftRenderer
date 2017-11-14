// Add by yaukey at 2017-08-30.
// All input management.

#include "stdafx.h"
#include "InputManager.h"
#include "InputDevice.h"
#include "InputDeviceMouse.h"
#include "InputDevicekeyboard.h"
#include "System/Support/Utility.h"

namespace yw
{
    InputManager::InputManager() :
        m_MainWindow(nullptr),
        m_DirectInput(nullptr)

    {
    }

    InputManager::~InputManager()
    {
        Release();
    }

    bool InputManager::Initialize(HWND mainWindow)
    {
        // Get main window.
        m_MainWindow = mainWindow;

        // Create input.
        HRESULT hr = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&m_DirectInput, nullptr);
        if (FAILED(hr))
        {
            LOGE(_T("InputManager::Initialize::DirectInput8Create failed!"));
            return false;
        }

        // Add input devices.
        m_InputDevices.clear();

        // Add mouse.
        InputDevice* deviceMouse = new InputDeviceMouse(this);
        deviceMouse->Initialize();
        m_InputDevices.push_back(deviceMouse);

        // Add keyboard.
        InputDevice* deviceKeyboard = new InputDevicekeyboard(this);
        deviceKeyboard->Initialize();
        m_InputDevices.push_back(deviceKeyboard);

        return true;
    }

    void InputManager::Release()
    {
        for (InputDeviceList::iterator it = m_InputDevices.begin(); it != m_InputDevices.end(); ++it)
        {
            YW_SAFE_RELEASE_DELETE(*it);
        }

        m_InputDevices.clear();

        m_MainWindow = nullptr;
        YW_SAFE_RELEASE(m_DirectInput);
    }

    void InputManager::Update()
    {
        for (InputDeviceList::const_iterator it = m_InputDevices.begin(); it != m_InputDevices.end(); ++it)
        {
            (*it)->Update();
        }
    }

    void InputManager::AddInputDevice(InputDevice* inputDevice)
    {
        if (nullptr == inputDevice)
        {
            return;
        }

        m_InputDevices.push_back(inputDevice);
    }

    void InputManager::OnInputKey(const InputKey* inputKey)
    {
        // Any input key event will come here, you can dispatch it by your own needs.
    }
}

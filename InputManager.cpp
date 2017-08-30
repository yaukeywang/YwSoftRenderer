// Add by yaukey at 2017-08-30.
// All input management.

#include "stdafx.h"
#include "InputManager.h"
#include "Utility.h"

namespace yw
{
    InputManager::InputManager() :
        m_MainWindow(nullptr),
        m_DirectInput(nullptr)

    {
    }

    InputManager::~InputManager()
    {
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

        return true;
    }

    void InputManager::Release()
    {
        m_MainWindow = nullptr;
        YW_SAFE_RELEASE(m_DirectInput);
    }

    void InputManager::Update()
    {

    }

    void InputManager::Dispatch()
    {

    }
}

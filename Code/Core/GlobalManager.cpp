// Add by yaukey at 2017-08-27.
// The global manager class.

#include "stdafx.h"
#include "GlobalManager.h"
#include "Logger.h"
#include "InputManager.h"
#include "AppBase.h"

namespace yw
{
    GlobalManager* GlobalManager::s_GlobalManager = nullptr;

    GlobalManager::GlobalManager() : 
        m_Logger(nullptr),
        m_InputManager(nullptr)
    {
        // Init global instance.
        ASSERT(nullptr == s_GlobalManager);
        s_GlobalManager = this;

        // Init modules.
        m_Logger = new Logger();
        //m_InputManager = new InputManager();  // Currently we use windows message for keyboard and mouse input, although InputManager has already shipped with mouse and keyboard input support, see: https://msdn.microsoft.com/en-us/library/windows/desktop/ee416842(v=vs.85).aspx.
    }

    GlobalManager::~GlobalManager()
    {
        Release();
    }

    bool GlobalManager::Initialize(AppBase* app)
    {
        // Initialize logger.
        if (nullptr != m_Logger)
        {
            m_Logger->Initialize();
        }

        // Initialize input manager.
        if (nullptr != m_InputManager)
        {
            m_InputManager->Initialize(app->GetMainWnd());
        }

        return true;
    }

    void GlobalManager::Release()
    {
        YW_SAFE_RELEASE_DELETE(m_Logger);
        YW_SAFE_RELEASE_DELETE(m_InputManager);
    }

    void GlobalManager::Update()
    {
        //
        // Logger is no update.
        //

        // Input manager update.
        if (nullptr != m_InputManager)
        {
            m_InputManager->Update();
        }
    }

    void GlobalManager::ReleaseGlobalManager()
    {
        YW_SAFE_DELETE(s_GlobalManager);
    }
}

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

    GlobalManager::GlobalManager()
    {
        // Init global instance.
        ASSERT(nullptr == s_GlobalManager);
        s_GlobalManager = this;

        // Init modules.
        m_Logger = new Logger();
        m_InputManager = new InputManager();
    }

    GlobalManager::~GlobalManager()
    {
        Release();
    }

    bool GlobalManager::Initialize(AppBase* app)
    {
        m_Logger->Initialize();
        m_InputManager->Initialize(app->GetMainWnd());

        return true;
    }

    void GlobalManager::Release()
    {
        YW_SAFE_RELEASE_DELETE(m_Logger);
        YW_SAFE_RELEASE_DELETE(m_InputManager);
    }

    void GlobalManager::ReleaseGlobalManager()
    {
        YW_SAFE_DELETE(s_GlobalManager);
    }
}

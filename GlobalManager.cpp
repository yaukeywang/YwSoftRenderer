// Add by yaukey at 2017-08-27.
// The global manager class.

#include "stdafx.h"
#include "GlobalManager.h"
#include "Logger.h"

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
    }

    GlobalManager::~GlobalManager()
    {
        // Clear global manager.
        s_GlobalManager = nullptr;

        // Delete logger.
        YW_SAFE_DELETE(m_Logger);
    }

    void GlobalManager::Initialize()
    {

    }

    void GlobalManager::Release()
    {

    }

    ILogger* GlobalManager::GetLogger()
    {
        return m_Logger;
    }

    GlobalManager* GlobalManager::GetGlobalManager()
    {
        if (nullptr == s_GlobalManager)
        {
            new GlobalManager();
        }

        return s_GlobalManager;
    }
}

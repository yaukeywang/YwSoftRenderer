// Add by yaukey at 2017-08-27.
// The global manager class.

#include "stdafx.h"
#include "GlobalMgr.h"
#include "Logger.h"

namespace yw
{
    GlobalMgr* GlobalMgr::s_GlobalMgr = nullptr;

    GlobalMgr::GlobalMgr()
    {
        // Init global instance.
        ASSERT(nullptr == s_GlobalMgr);
        s_GlobalMgr = this;

        // Init modules.
        m_Logger = new Logger();
    }

    GlobalMgr::~GlobalMgr()
    {
        // Clear global manager.
        s_GlobalMgr = nullptr;

        // Delete logger.
        YW_SAFE_DELETE(m_Logger);
    }

    void GlobalMgr::Initialize()
    {

    }

    void GlobalMgr::Release()
    {

    }

    ILogger* GlobalMgr::GetLogger()
    {
        return m_Logger;
    }

    GlobalMgr* GlobalMgr::GetGlobalMgr()
    {
        if (nullptr == s_GlobalMgr)
        {
            new GlobalMgr();
        }

        return s_GlobalMgr;
    }
}

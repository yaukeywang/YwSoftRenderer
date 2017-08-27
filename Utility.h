// Add by yaukey at 2017-08-27.
// All utilities.

#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "Base.h"
#include "Logger.h"
#include "GlobalMgr.h"

namespace yw
{
    // Log utilties.
    #define Log(msg) GlobalMgr::GetGlobalMgr()->GetLogger()->Log((msg))
    #define LogF(msg) GlobalMgr::GetGlobalMgr()->GetLogger()->LogFormat(msg)
    #define LogI(msg) GlobalMgr::GetGlobalMgr()->GetLogger()->LogInformation(msg)
    #define LogIF(msg) GlobalMgr::GetGlobalMgr()->GetLogger()->LogInformationFormat(msg)
    #define LogW(msg) GlobalMgr::GetGlobalMgr()->GetLogger()->LogWarning(msg)
    #define LogWF(msg) GlobalMgr::GetGlobalMgr()->GetLogger()->LogWarningFormat(msg)
    #define LogE(msg) GlobalMgr::GetGlobalMgr()->GetLogger()->LogError(msg)
    #define LogEF(msg) GlobalMgr::GetGlobalMgr()->GetLogger()->LogErrorFormat(msg)
}

#endif // __UTILITY_H__

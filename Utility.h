// Add by yaukey at 2017-08-27.
// All utilities.

#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "Base.h"
#include "Logger.h"
#include "GlobalManager.h"

namespace yw
{
    // Log utilties.
    #define Log(msg) GlobalManager::GetGlobalManager()->GetLogger()->Log((msg))
    #define LogF(msg) GlobalManager::GetGlobalManager()->GetLogger()->LogFormat(msg)
    #define LogI(msg) GlobalManager::GetGlobalManager()->GetLogger()->LogInformation(msg)
    #define LogIF(msg) GlobalManager::GetGlobalManager()->GetLogger()->LogInformationFormat(msg)
    #define LogW(msg) GlobalManager::GetGlobalManager()->GetLogger()->LogWarning(msg)
    #define LogWF(msg) GlobalManager::GetGlobalManager()->GetLogger()->LogWarningFormat(msg)
    #define LogE(msg) GlobalManager::GetGlobalManager()->GetLogger()->LogError(msg)
    #define LogEF(msg) GlobalManager::GetGlobalManager()->GetLogger()->LogErrorFormat(msg)
}

#endif // __UTILITY_H__

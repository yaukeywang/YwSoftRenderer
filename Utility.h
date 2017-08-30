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
    #define LOG(msg) GlobalManager::GetGlobalManager()->GetLogger()->Log((msg))
    #define LOGF(msg) GlobalManager::GetGlobalManager()->GetLogger()->LogFormat(msg)
    #define LOGI(msg) GlobalManager::GetGlobalManager()->GetLogger()->LogInformation(msg)
    #define LOGIF(msg) GlobalManager::GetGlobalManager()->GetLogger()->LogInformationFormat(msg)
    #define LOGW(msg) GlobalManager::GetGlobalManager()->GetLogger()->LogWarning(msg)
    #define LOGWF(msg) GlobalManager::GetGlobalManager()->GetLogger()->LogWarningFormat(msg)
    #define LOGE(msg) GlobalManager::GetGlobalManager()->GetLogger()->LogError(msg)
    #define LOGEF(msg) GlobalManager::GetGlobalManager()->GetLogger()->LogErrorFormat(msg)
}

#endif // __UTILITY_H__

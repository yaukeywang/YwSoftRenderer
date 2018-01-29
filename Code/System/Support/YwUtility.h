// Add by yaukey at 2017-08-27.
// All utilities.

#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "YwBase.h"
#include "YwLogger.h"
#include "YwGlobalManager.h"

namespace yw
{
    // Log utilties.
    #define LOG GlobalManager::GetGlobalManager()->GetLogger()->Log
    #define LOGF GlobalManager::GetGlobalManager()->GetLogger()->LogFormat
    #define LOGI GlobalManager::GetGlobalManager()->GetLogger()->LogInformation
    #define LOGIF GlobalManager::GetGlobalManager()->GetLogger()->LogInformationFormat
    #define LOGW GlobalManager::GetGlobalManager()->GetLogger()->LogWarning
    #define LOGWF GlobalManager::GetGlobalManager()->GetLogger()->LogWarningFormat
    #define LOGE GlobalManager::GetGlobalManager()->GetLogger()->LogError
    #define LOGEF GlobalManager::GetGlobalManager()->GetLogger()->LogErrorFormat
}

#endif // __UTILITY_H__

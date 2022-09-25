// Add by Yaukey at 2022-09-25.
// YW paths management.

#ifndef __YW_PATHS_H__
#define __YW_PATHS_H__

#include "YwBase.h"

namespace yw
{
    class Paths
    {
    public:
        static StringA GetFilePath(StringA inPath);
        static StringA GetFileName(StringA inPath);
        static StringA GetFileExtension(StringA inPath);
    };
}

#endif // !__YW_PATHS_H__

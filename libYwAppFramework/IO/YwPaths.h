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
        static String GetFilePath(String inPath);
        static StringA GetFilePathA(StringA inPath);
        static StringW GetFilePathW(StringW inPath);

        static String GetFileName(String inPath);
        static StringA GetFileNameA(StringA inPath);
        static StringW GetFileNameW(StringW inPath);

        static String GetFileExtension(String inPath);
        static StringA GetFileExtensionA(StringA inPath);
        static StringW GetFileExtensionW(StringW inPath);
    };
}

#endif // !__YW_PATHS_H__

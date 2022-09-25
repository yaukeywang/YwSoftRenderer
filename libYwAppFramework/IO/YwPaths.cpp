// Add by Yaukey at 2022-09-25.
// YW paths management.

#include "YwPaths.h"

namespace yw
{
    StringA Paths::GetFilePath(StringA inPath)
    {
        int32_t segmentIdx = (int32_t)inPath.find_last_of('/');
        if (segmentIdx < 0)
        {
            segmentIdx = (int32_t)inPath.find_last_of('\\');
        }

        if (segmentIdx < 0)
        {
            return StringA();
        }
        else
        {
            return inPath.substr(0, segmentIdx + 1);
        }
    }

    StringA Paths::GetFileName(StringA inPath)
    {
        int32_t segmentIdx = (int32_t)inPath.find_last_of('/');
        if (segmentIdx < 0)
        {
            segmentIdx = (int32_t)inPath.find_last_of('\\');
        }

        const int32_t dotIdx = (int32_t)inPath.find_last_of('.');
        if (segmentIdx < 0)
        {
            if (dotIdx < 0)
            {
                return StringA(inPath);
            }
            else
            {
                return inPath.substr(0, dotIdx);
            }
        }
        else
        {
            const int32_t startPos = segmentIdx + 1;
            if ((dotIdx < 0) || (dotIdx < segmentIdx))
            {
                return inPath.substr(startPos, inPath.length() - startPos);
            }
            else
            {
                return inPath.substr(startPos, dotIdx - startPos);
            }
        }
    }

    StringA Paths::GetFileExtension(StringA inPath)
    {
        const int32_t dotIdx = (int32_t)inPath.find_last_of('.');
        if (dotIdx <= 0)
        {
            return StringA();
        }

        int32_t segmentIdx = (int32_t)inPath.find_last_of('/');
        if (segmentIdx < 0)
        {
            segmentIdx = (int32_t)inPath.find_last_of('\\');
        }

        if (dotIdx <= segmentIdx)
        {
            return StringA();
        }

        const int32_t startPos = dotIdx + 1;
        return inPath.substr(startPos, inPath.length() - startPos);
    }
}

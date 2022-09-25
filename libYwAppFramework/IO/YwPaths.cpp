// Add by Yaukey at 2022-09-25.
// YW paths management.

#include "YwPaths.h"

namespace yw
{
    String Paths::GetFilePath(String inPath)
    {
        int32_t segmentIdx = (int32_t)inPath.find_last_of(_T('/'));
        if (segmentIdx < 0)
        {
            segmentIdx = (int32_t)inPath.find_last_of(_T('\\'));
        }

        if (segmentIdx < 0)
        {
            return String();
        }
        else
        {
            return inPath.substr(0, segmentIdx + 1);
        }
    }

    StringA Paths::GetFilePathA(StringA inPath)
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

    StringW Paths::GetFilePathW(StringW inPath)
    {
        int32_t segmentIdx = (int32_t)inPath.find_last_of(L'/');
        if (segmentIdx < 0)
        {
            segmentIdx = (int32_t)inPath.find_last_of(L'\\');
        }

        if (segmentIdx < 0)
        {
            return StringW();
        }
        else
        {
            return inPath.substr(0, segmentIdx + 1);
        }
    }

    String Paths::GetFileName(String inPath)
    {
        int32_t segmentIdx = (int32_t)inPath.find_last_of(_T('/'));
        if (segmentIdx < 0)
        {
            segmentIdx = (int32_t)inPath.find_last_of(_T('\\'));
        }

        const int32_t dotIdx = (int32_t)inPath.find_last_of(_T('.'));
        if (segmentIdx < 0)
        {
            if (dotIdx < 0)
            {
                return String(inPath);
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

    StringA Paths::GetFileNameA(StringA inPath)
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

    StringW Paths::GetFileNameW(StringW inPath)
    {
        int32_t segmentIdx = (int32_t)inPath.find_last_of(L'/');
        if (segmentIdx < 0)
        {
            segmentIdx = (int32_t)inPath.find_last_of(L'\\');
        }

        const int32_t dotIdx = (int32_t)inPath.find_last_of(L'.');
        if (segmentIdx < 0)
        {
            if (dotIdx < 0)
            {
                return StringW(inPath);
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

    String Paths::GetFileExtension(String inPath)
    {
        const int32_t dotIdx = (int32_t)inPath.find_last_of(_T('.'));
        if (dotIdx <= 0)
        {
            return String();
        }

        int32_t segmentIdx = (int32_t)inPath.find_last_of(_T('/'));
        if (segmentIdx < 0)
        {
            segmentIdx = (int32_t)inPath.find_last_of(_T('\\'));
        }

        if (dotIdx <= segmentIdx)
        {
            return String();
        }

        const int32_t startPos = dotIdx + 1;
        return inPath.substr(startPos, inPath.length() - startPos);
    }

    StringA Paths::GetFileExtensionA(StringA inPath)
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

    StringW Paths::GetFileExtensionW(StringW inPath)
    {
        const int32_t dotIdx = (int32_t)inPath.find_last_of(L'.');
        if (dotIdx <= 0)
        {
            return StringW();
        }

        int32_t segmentIdx = (int32_t)inPath.find_last_of(L'/');
        if (segmentIdx < 0)
        {
            segmentIdx = (int32_t)inPath.find_last_of(L'\\');
        }

        if (dotIdx <= segmentIdx)
        {
            return StringW();
        }

        const int32_t startPos = dotIdx + 1;
        return inPath.substr(startPos, inPath.length() - startPos);
    }
}

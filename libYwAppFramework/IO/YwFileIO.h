// Add by Yaukey at 2019-06-01.
// YW Soft Renderer file io class.

#ifndef __YW_FILE_IO_H__
#define __YW_FILE_IO_H__

#include "YwBase.h"

namespace yw
{
    class FileIO
    {
    public:
        // Constructor.
        FileIO();

        // Destructor.
        virtual ~FileIO();

    public:
        // Reading file and allocating data, with text or binary.
        uint32_t ReadFile(const StringA& filename, uint8_t** data, bool text = false);

        // Reading file and filling by already allocated data, with text or binary.
        uint32_t ReadFile(const StringA& filename, uint8_t* data, uint32_t dataSize, bool text = false);

        // Writing file with provided data.
        uint32_t WriteFile(const StringA& filename, uint8_t* data, uint32_t dataSize, bool text = false);

        // Check if file exist.
        bool FileExists(const StringA& filename);

        // Get file relative path.
        StringA GetFilePath(const StringA& fileName);
    };
}

#endif // !__YW_FILE_IO_H__

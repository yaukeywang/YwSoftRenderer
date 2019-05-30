// Add by Yaukey at 2018-10-10.
// YW Soft Renderer file io class.


#include "YwFileIO.h"
#include "YwBaseApplication.h"

namespace yw
{
    FileIO::FileIO(IApplication* application) :
        m_Application(application)
    {
    }

    FileIO::~FileIO()
    {
    }

    bool FileIO::Initialize()
    {
        return true;
    }

    uint32_t FileIO::ReadFile(StringA& filename, uint8_t** data, bool text)
    {
        // Open the file.
        FILE* file = fopen(filename.c_str(), text ? "rt" : "rb");
        if (nullptr == file)
        {
            return 0;
        }

        // Get file size.
        fseek(file, 0, SEEK_END);
        uint32_t fileSize = ftell(file);

        // Rewind to the begin.
        rewind(file);

        // Read data.
        *data = new uint8_t[text ? fileSize + 1 : fileSize];
        fileSize = (uint32_t)fread(*data, 1, fileSize, file);

        // Close file.
        fclose(file);

        // Processing data.
        if (text)
        {
            // Fill last character if data is text.
            (*data)[fileSize] = 0;

#if !(defined(_WIN32) || defined(WIN32))
            char* temp = new char[fileSize + 1];
            strcpy(temp, objData);

            char* src = temp;
            char* dest = objData;
            fileSize = 0;
            while (*src)
            {
                if ('\r' != *src)
                {
                    *dest++ = *src;
                    ++fileSize;
                }

                ++src;
            }

            *dest = 0;

            // Release temp data.
            YW_SAFE_DELETE_ARRAY(temp);
#endif
        }

        return fileSize;
    }

    uint32_t FileIO::ReadFile(StringA& filename, uint8_t* data, uint32_t dataSize, bool text)
    {
        return 0;
    }

    uint32_t FileIO::WriteFile(StringA& filename, uint8_t* data, uint32_t dataSize, bool text)
    {
        return 0;
    }

    bool FileIO::FileExists(StringA& filename)
    {
        return false;
    }
}

// Add by Yaukey at 2019-06-01.
// YW Soft Renderer file io class.

#ifndef __YW_FILE_IO_H__
#define __YW_FILE_IO_H__

#include "YwBase.h"

namespace yw
{
    class FileIO
    {
        friend class IApplication;
        friend class Application;

    protected:
        // Constructor.
        FileIO(class IApplication* application);

        // Destructor.
        virtual ~FileIO();

    protected:
        // Initialize this system.
        bool Initialize();

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

    public:
        // Get parent application.
        inline class IApplication* GetApplication()
        {
            return m_Application;
        }

    private:
        // The parent application.
        class IApplication* m_Application;
    };
}

#endif // !__YW_FILE_IO_H__

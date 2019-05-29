// Add by Yaukey at 2018-10-10.
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
        FileIO();
        virtual ~FileIO();

    protected:
        bool Initialize();

    public:
        uint32_t ReadFile(StringA& i_sFilename, uint8_t** data, bool text = false);
        uint32_t ReadFile(StringA& i_sFilename, uint8_t* data, uint32_t dataSize, bool text = false);
        uint32_t WriteFile(StringA& i_sFilename, uint8_t* data, uint32_t dataSize, bool text = false);
        bool FileExists(StringA& filename);

    public:
        // Get parent application.
        inline class IApplication* GetApplication()
        {
            return m_Application;
        }

    private:
        class IApplication* m_Application;
    };
}

#endif // !__YW_FILE_IO_H__

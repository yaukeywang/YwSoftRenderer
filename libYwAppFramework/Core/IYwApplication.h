// Add by Yaukey at 2018-07-18.
// YW Soft Renderer application framework base application interface.

#ifndef __I_YW_APPLICATION_H__
#define __I_YW_APPLICATION_H__

#include "Yw3d.h"

namespace yw
{
    // The application creation flags.
    struct ApplicationCreationFlags
    {
        // Window title.
        String windowTitle;

#ifdef WIN32
        // The window icon.
        HICON icon;
#endif
        // Window width and height.
        uint16_t windowWidth;
        uint16_t windowHeight;

        // Windowed or not.
        bool windowed;
    };

    // The base of application class.
    class IApplication
    {
    public:
        IApplication();
        ~IApplication();
    };
}

#endif // !__I_YW_APPLICATION_H__


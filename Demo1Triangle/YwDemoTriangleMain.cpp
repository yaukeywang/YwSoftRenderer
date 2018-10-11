// Add by Yaukey at 2018-10-10.
// YW Soft Renderer demo triangle main entry class.

#include "YwDemoTriangleApp.h"

// ------------------------------------------------------------------
// Platform-dependent code.


#if defined(_WIN32) || defined(WIN32)
// Windows platform.
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#elif LINUX_X11
// Linux platform.
int main()
#elif __amigaos4__
// amigaos platform.
int main(int argc, char** argv)
#endif
{
    yw::ApplicationCreationFlags creationFlags;
    creationFlags.windowTitle = _T("DemoTriangle");

    uint32_t windowWidth = 400;
    uint32_t windowHeight = 300;

    #ifdef __amigaos4__
    if (argc == 3)
    {
        windowWidth = iClamp(atoi(argv[1]), 160, 1600);
        windowHeight = iClamp(atoi(argv[2]), 160, 1280);
    }
    #endif

    #if defined(_WIN32) || defined(WIN32)
    creationFlags.icon = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(0));
    #endif

    creationFlags.windowWidth = windowWidth;
    creationFlags.windowHeight = windowHeight;
    creationFlags.windowed = true;

    yw::DemoTriangleApp demoApp;
    if (!demoApp.Initialize(creationFlags))
    {
        return 1;
    }

    return demoApp.Run();
}
// Add by yaukey at 2017-08-22.
// App main entry.

#include "stdafx.h"
#include "System/App/YwAppSoftRenderer.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	yw::AppSoftRenderer app(hInstance);
    app.Initialize();
	app.Run();
    app.Release();

    return 0;
}

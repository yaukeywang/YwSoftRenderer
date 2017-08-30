// Add by yaukey at 2017-08-30.
// All input management.

#ifndef __INPUT_MANAGER_H__
#define __INPUT_MANAGER_H__

#include "Base.h"
#include <dinput.h>
#include <dinputd.h>

namespace yw
{
    class InputManager
    {
    public:
        InputManager();
        virtual ~InputManager();

    public:
        bool Initialize(HWND mainWindow);
        void Release();
        
        void Update();
        void Dispatch();

    private:
        // The handle of the main window.
        HWND m_MainWindow;

        // The direct input device.
        LPDIRECTINPUT8 m_DirectInput;
    };
}

#endif // !__INPUT_MANAGER_H__

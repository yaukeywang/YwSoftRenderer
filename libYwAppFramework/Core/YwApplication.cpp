// Add by Yaukey at 2018-07-18.
// YW Soft Renderer application framework for platform-dependent implementation.

#include "YwApplication.h"
#include "YwInput.h"
#include "YwGraphics.h"
#include "YwScene.h"

// ------------------------------------------------------------------
// Platform-dependent code.

// ------------------------------------------------------------------
// Windows platform.
#if defined(_WIN32) || defined(WIN32)

namespace yw
{
    Application* Application::m_sApplication = nullptr;

    Application::Application()
    {
        assert(nullptr == m_sApplication);
        m_sApplication = this;
        m_TicksPerSecond.QuadPart = 0;
        m_StartTime.QuadPart = 0;
        m_LastTime.QuadPart = 0;
    }

    Application::~Application()
    {
        UnregisterClass(_T("<YwSoftRenderer Application Window>"), GetModuleHandle(NULL));
        m_sApplication = nullptr;
    }

    bool Application::Initialize(const ApplicationCreationFlags& creationFlags)
    {
        m_WindowTitle = creationFlags.windowTitle;
        m_WindowWidth = creationFlags.windowWidth;
        m_WindowHeight = creationFlags.windowHeight;
        m_Windowed = creationFlags.windowed;

        // Initialize the timer.
        if (!QueryPerformanceFrequency(&m_TicksPerSecond))
        {
            return false;
        }

        // Create the render window.
        WNDCLASS windowClass = { 0, &WindowProcedure, 0, 0, GetModuleHandle(NULL), creationFlags.icon, 0, 0, 0, _T("<YwSoftRenderer Application Window>") };
        RegisterClass(&windowClass);

        // Combine window style.
        uint32_t windowStyle = WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION | WS_SYSMENU;
        RECT rc;
        SetRect(&rc, 0, 0, m_WindowWidth, m_WindowHeight);
        AdjustWindowRect(&rc, windowStyle, false);

        // Create window.
        m_WindowHandle = CreateWindow(
            _T("<YwSoftRenderer Application Window>"), 
            m_WindowTitle.c_str(), 
            windowStyle, 
            CW_USEDEFAULT, 
            CW_USEDEFAULT, 
            rc.right - rc.left, 
            rc.bottom - rc.top, 
            0, 
            0, 
            GetModuleHandle(NULL), 
            NULL
        );

        // Set cursor.
        SetCursor(LoadCursor(0, IDC_WAIT));

        return CreateSubSystems(creationFlags);
    }

    int	Application::Run()
    {
        ShowWindow(m_WindowHandle, SW_SHOW);
        SetFocus(m_WindowHandle);
        SetCursor(LoadCursor(0, IDC_ARROW));
        m_Active = true;

        // Begin application loop.
        while (CheckMessages())
        {
            BeginFrame();
            Render();
            EndFrame();

            Sleep(1);
        }

        // End of application loop = termination of program.

        // Destroy world.
        DestroyWorld();

        return 0;
    }

    void Application::BeginFrame()
    {
        if (0 == m_StartTime.QuadPart)
        {
            QueryPerformanceCounter(&m_StartTime);
            m_LastTime.QuadPart = m_StartTime.QuadPart;
        }

        // Increase frame.
        ++m_FrameIdent;

        // Get latest keyboard and mouse state.
        m_Input->Update();

        // Update basic logic frame move.
        FrameMove();
        m_Scene->FrameMove();
    }

    bool Application::CheckMessages()
    {
        while (true)
        {
            MSG msg;
            if (m_Active)
            {
                if (!PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
                {
                    break;
                }
            }
            else
            {
                if (!GetMessage(&msg, 0, 0, 0))
                {
                    break;
                }
            }

            // Translate and dispatch message.
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            // Check if it is quit.
            if (WM_QUIT == msg.message)
            {
                return false;
            }
        }

        return true;
    }

    void Application::EndFrame()
    {
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);

        float timeDifference = (float)(currentTime.QuadPart - m_LastTime.QuadPart);
        m_DeltaTime = timeDifference / (float)m_TicksPerSecond.QuadPart;
        m_ElapsedTime += m_DeltaTime;
        m_FPS = (float)m_TicksPerSecond.QuadPart / timeDifference;

        m_InvFPS = 1.0f / m_FPS;
        m_LastTime.QuadPart = currentTime.QuadPart;
    }

    LRESULT CALLBACK Application::WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        return Application::GetApplication()->MessageProcedure(hWnd, uMsg, wParam, lParam);
    }

    LRESULT Application::MessageProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_ACTIVATE:
            m_Active = (LOWORD(wParam) != WA_INACTIVE);
            break;
        case WM_ENTERSIZEMOVE:
            m_Active = false;
            break;
        case WM_EXITSIZEMOVE:
            m_Active = true;
            break;
        case WM_SETCURSOR:
            {
                // Turn off Windows cursor in fullscreen mode.
                if (m_Active && !m_Windowed)
                {
                    SetCursor(0);
                    return true; // prevent Windows from setting cursor to window class cursor.
                }
            }
            break;
        case WM_POWERBROADCAST:
            // Don't allow suspend/standby.
            return false;
        case WM_SYSCOMMAND:
            {
                // Prevent moving/sizing and power loss in fullscreen mode.
                switch (wParam)
                {
                case SC_MOVE:
                case SC_SIZE:
                case SC_MAXIMIZE:
                case SC_KEYMENU:
                case SC_MONITORPOWER:
                    {
                        if (!m_Windowed)
                        {
                            return 1;
                        }
                    }
                    break;
                }
            }
            break;
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    Application* Application::GetApplication()
    {
        assert(nullptr != Application::m_sApplication);
        return Application::m_sApplication;
    }
}

#endif // End of Windows platform.

// ------------------------------------------------------------------
// Linux platform.
#if defined(LINUX_X11) || defined(_LINUX)
#endif

// ------------------------------------------------------------------
// Mac OSX platform.
#if defined(_MAC_OSX)
#endif

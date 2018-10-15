// Add by Yaukey at 2018-07-18.
// YW Soft Renderer application framework for platform-dependent implementation.

#ifndef __YW_APPLICATION_H__
#define __YW_APPLICATION_H__

#include "Yw3d.h"
#include "YwBaseApplication.h"

// ------------------------------------------------------------------
// Platform-dependent code.

// ------------------------------------------------------------------
// Windows platform.
#if defined(_WIN32) || defined(WIN32)

namespace yw
{
    class Application : public IApplication
    {
    public:
        // Constructor.
        Application();

        // Destructor.
        virtual ~Application();

    public:
        // Initialize application by parameters.
        bool Initialize(const ApplicationCreationFlags& creationFlags);

        // Application' s main loop entry.
        int	Run();

        // Create world.
        virtual bool CreateWorld() = 0;

        // Destroy world.
        virtual void DestroyWorld() = 0;

        // Update logic frame move.
        virtual void FrameMove() = 0;

        // Render objects.
        virtual void Render() = 0;

    public:
        static Application* GetApplication();

    private:
        // Begin each frame.
        void BeginFrame();

        // Check messages of this frame.
        bool CheckMessages();

        // End this frame.
        void EndFrame();

    private:
        // Overrided the window procedure.
        static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        // The window message procedure.
        LRESULT MessageProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        // Time relatived.
        LARGE_INTEGER m_TicksPerSecond;
        LARGE_INTEGER m_StartTime;
        LARGE_INTEGER m_LastTime;

    private:
        // The global application instance.
        static Application* m_sApplication;
    };
}

#endif // End of Windows platform.

#endif // !__YW_APPLICATION_H__


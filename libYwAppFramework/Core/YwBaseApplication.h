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
        virtual ~IApplication();

    public:
        // Initialize application by parameters.
        virtual bool Initialize(const ApplicationCreationFlags& creationFlags) = 0;

        // Application' s main loop entry.
        virtual int	Run() = 0;

        // Create world.
        virtual bool CreateWorld() = 0;

        // Destroy world.
        virtual void DestroyWorld() = 0;

        // Update logic frame move.
        virtual void FrameMove() = 0;

        // Render objects.
        virtual void Render() = 0;

    public:
        // Get window handle.
        inline WindowHandle GetWindowHandle() const
        {
            return m_WindowHandle;
        }

        // Get if application windowed.
        inline bool GetWindowed() const
        {
            return m_Windowed;
        }

        // Get if application active.
        inline bool GetActive() const
        {
            return m_Active;
        }

        // Get window title.
        inline const String& GetWindowTitle() const
        {
            return m_WindowTitle;
        }

        // Get window width.
        inline uint32_t GetWindowWidth() const
        {
            return m_WindowWidth;
        }

        // Get window height.
        inline uint32_t GetWindowHeight() const
        {
            return m_WindowHeight;
        }

    protected:
        // The window handle
        WindowHandle m_WindowHandle;

        // Windowed or not.
        bool m_Windowed;

        // Current active or not.
        bool m_Active;

        // The window title
        String m_WindowTitle;

        // Window width.
        uint32_t m_WindowWidth;

        // Window height.
        uint32_t m_WindowHeight;
    };
}

#endif // !__I_YW_APPLICATION_H__


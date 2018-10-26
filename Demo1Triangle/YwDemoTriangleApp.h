// Add by Yaukey at 2018-10-10.
// YW Soft Renderer demo triangle application class.

#ifndef __YW_DEMO_TRIANGLE_APP_H__
#define __YW_DEMO_TRIANGLE_APP_H__

#include "YwApplication.h"
#include "YwScene.h"

namespace yw
{
    class DemoTriangleApp : public Application
    {
    public:
        // Constructor.
        DemoTriangleApp();

        // Destructor.
        ~DemoTriangleApp();

    public:
        // Create world.
        bool CreateWorld();

        // Destroy world.
        void DestroyWorld();

        // Update logic frame move.
        void FrameMove();

        // Render objects.
        void Render();

    public:
        // Current rotation angle.
        float GetRotationAngle() const;

    private:
        // Current camera.
        class DemoTriangleCamera* m_Camera;

        // The demo triangle entity handle.
        HENTITY m_DemoTriangleHandle;

        // Window caption text update time.
        float m_UpdateTextTime;

        // The rotation angle.
        float m_RotateAngle;
    };
}

#endif // !__YW_DEMO_TRIANGLE_APP_H__


// Add by Yaukey at 2019-09-19.
// YW Soft Renderer demo model application class.

#ifndef __YW_DEMO_MODEL_APP_H__
#define __YW_DEMO_MODEL_APP_H__

#include "YwApplication.h"
#include "YwScene.h"

namespace yw
{
    class DemoModelApp : public Application
    {
    public:
        // Constructor.
        DemoModelApp();

        // Destructor.
        ~DemoModelApp();

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
        class DemoModelCamera* m_Camera;

        // The demo model entity handle.
        HENTITY m_DemoModelHandle;

        // Window caption text update time.
        float m_UpdateTextTime;

        // The rotation angle.
        float m_RotateAngle;
    };
}

#endif // !__YW_DEMO_MODEL_APP_H__


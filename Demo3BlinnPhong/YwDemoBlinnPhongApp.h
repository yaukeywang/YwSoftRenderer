// Add by Yaukey at 2019-09-19.
// YW Soft Renderer demo with blinn-phong application class.

#ifndef __YW_DEMO_BLINN_PHONG_APP_H__
#define __YW_DEMO_BLINN_PHONG_APP_H__

#include "YwApplication.h"
#include "YwScene.h"

namespace yw
{
    class DemoBlinnPhongApp : public Application
    {
    public:
        // Constructor.
        DemoBlinnPhongApp();

        // Destructor.
        ~DemoBlinnPhongApp();

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
        class DemoBlinnPhongCamera* m_Camera;

        // The demo entity handle.
        HENTITY m_DemoBlinnPhongHandle;

        // Window caption text update time.
        float m_UpdateTextTime;

        // The rotation angle.
        float m_RotateAngle;
    };
}

#endif // !__YW_DEMO_BLINN_PHONG_APP_H__


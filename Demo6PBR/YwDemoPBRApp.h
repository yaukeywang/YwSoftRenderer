// Add by Yaukey at 2020-02-24.
// YW Soft Renderer demo with PBR application class.

#ifndef __YW_DEMO_PBR_APP_H__
#define __YW_DEMO_PBR_APP_H__

#include "YwApplication.h"
#include "YwScene.h"

namespace yw
{
    class DemoPBRApp : public Application
    {
    public:
        // Constructor.
        DemoPBRApp();

        // Destructor.
        ~DemoPBRApp();

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
        // Current model rotation angle.
        float GetModelRotationAngle() const;

    private:
        // Current camera.
        class DemoPBRCamera* m_Camera;

        // The demo entity handle.
        HENTITY m_DemoPBRHandle;

        // Window caption text update time.
        float m_UpdateTextTime;

        // The model rotation angle.
        float m_ModelRotateAngle;
    };
}

#endif // !__YW_DEMO_PBR_APP_H__

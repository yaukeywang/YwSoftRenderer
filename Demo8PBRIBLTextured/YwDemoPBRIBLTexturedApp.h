// Add by Yaukey at 2023-06-04.
// YW Soft Renderer demo with PBR-IBL-Textured application class.

#ifndef __YW_DEMO_PBR_IBL_TEXTURED_APP_H__
#define __YW_DEMO_PBR_IBL_TEXTURED_APP_H__

#include "YwApplication.h"
#include "YwScene.h"

namespace yw
{
    class DemoPBRIBLTexturedApp : public Application
    {
    public:
        // Constructor.
        DemoPBRIBLTexturedApp();

        // Destructor.
        ~DemoPBRIBLTexturedApp();

    public:
        // Create world.
        bool CreateWorld();

        // Destroy world.
        void DestroyWorld();

        // Update logic frame move.
        void FrameMove();

        // Render objects.
        void Render();

    private:
        // Current camera.
        class DemoPBRIBLTexturedCamera* m_Camera;

        // The demo entity handle.
        HENTITY m_DemoPBRIBLTexturedHandle;

        // Window caption text update time.
        float m_UpdateTextTime;

        // User is dragging or not.
        bool m_Drag;
    };
}

#endif // !__YW_DEMO_PBR_IBL_TEXTURED_APP_H__

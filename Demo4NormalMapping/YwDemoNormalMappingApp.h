// Add by Yaukey at 2020-01-23.
// YW Soft Renderer demo with normal-mapping application class.

#ifndef __YW_DEMO_NORMAL_MAPPING_APP_H__
#define __YW_DEMO_NORMAL_MAPPING_APP_H__

#include "YwApplication.h"
#include "YwScene.h"

namespace yw
{
    class DemoNormalMappingApp : public Application
    {
    public:
        // Constructor.
        DemoNormalMappingApp();

        // Destructor.
        ~DemoNormalMappingApp();

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

        // Current light rotation angle.
        float GetLightRotationAngle() const;

    private:
        // Current camera.
        class DemoNormalMappingCamera* m_Camera;

        // The demo entity handle.
        HENTITY m_DemoNormalMappingHandle;

        // Window caption text update time.
        float m_UpdateTextTime;

        // The model rotation angle.
        float m_ModelRotateAngle;

        // The light rotation angle.
        float m_LightRotateAngle;
    };
}

#endif // !__YW_DEMO_NORMAL_MAPPING_APP_H__

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
        inline float GetModelRotationAngle() const
        {
            return m_ModelRotateAngle;
        }

        // Current light rotation angle.
        inline float GetLightRotationAngle() const
        {
            return m_LightRotateAngle;
        }

        // Current roughness of this demo.
        inline float GetRoughness() const
        {
            return m_Roughness;
        }

        // Current subsurface of this demo.
        inline float GetSubsurface() const
        {
            return m_Subsurface;
        }

    private:
        // Current camera.
        class DemoPBRCamera* m_Camera;

        // The demo entity handle.
        HENTITY m_DemoPBRHandle;

        // Window caption text update time.
        float m_UpdateTextTime;

        // The model rotation angle.
        float m_ModelRotateAngle;

        // The light rotation angle.
        float m_LightRotateAngle;

        // The roughness of this demo.
        float m_Roughness;

        // The subsurface of this demo.
        float m_Subsurface;
    };
}

#endif // !__YW_DEMO_PBR_APP_H__

// Add by Yaukey at 04-18.
// YW Soft Renderer demo with PBR-IBL application class.

#ifndef __YW_DEMO_PBR_IBL_APP_H__
#define __YW_DEMO_PBR_IBL_APP_H__

#include "YwApplication.h"
#include "YwScene.h"

namespace yw
{
    class DemoPBRIBLApp : public Application
    {
    public:
        // Constructor.
        DemoPBRIBLApp();

        // Destructor.
        ~DemoPBRIBLApp();

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
        // Current light rotation angle.
        inline float GetLightRotationAngle() const
        {
            return m_LightRotateAngle;
        }

        // Current roughness of this demo.
        inline float GetMetallic() const
        {
            return m_Metallic;
        }

        // Current subsurface of this demo.
        inline float GetSmoothness() const
        {
            return m_Smoothness;
        }

    private:
        // Current camera.
        class DemoPBRIBLCamera* m_Camera;

        // The demo entity handle.
        HENTITY m_DemoPBRIBLHandle;

        // Window caption text update time.
        float m_UpdateTextTime;

        // The light rotation angle.
        float m_LightRotateAngle;

        // The metallic of this demo.
        float m_Metallic;

        // The smooth of this demo.
        float m_Smoothness;

        // User is dragging or not.
        bool m_Drag;
    };
}

#endif // !__YW_DEMO_PBR_IBL_APP_H__

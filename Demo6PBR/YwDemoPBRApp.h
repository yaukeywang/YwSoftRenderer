// Add by Yaukey at 2020-02-24.
// YW Soft Renderer demo with PBR application class.

#ifndef __YW_DEMO_PBR_APP_H__
#define __YW_DEMO_PBR_APP_H__

#include "YwApplication.h"
#include "YwScene.h"
#include "YwArcBall.h"

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

        // Get model viewing rotation.
        inline const Quaternion& GetModelRotation()
        {
            return m_ArcBall.GetRotation();
        }

    private:
        // Current camera.
        class DemoPBRCamera* m_Camera;

        // The demo entity handle.
        HENTITY m_DemoPBRHandle;

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

        // The arc ball to view the model.
        ArcBall m_ArcBall;
    };
}

#endif // !__YW_DEMO_PBR_APP_H__

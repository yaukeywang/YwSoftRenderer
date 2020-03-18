// Add by Yaukey at 2020-02-02.
// YW Soft Renderer demo with triangle shader wireframe application class.

#ifndef __YW_DEMO_TRIANGLE_SHADER_WIREFRAME_APP_H__
#define __YW_DEMO_TRIANGLE_SHADER_WIREFRAME_APP_H__

#include "YwApplication.h"
#include "YwScene.h"

namespace yw
{
    class DemoTriangleShaderWireframeApp : public Application
    {
    public:
        // Constructor.
        DemoTriangleShaderWireframeApp();

        // Destructor.
        ~DemoTriangleShaderWireframeApp();

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

        // Get line width.
        inline float GetLineWidth() const
        {
            return m_LineWidth;
        }

        // Get pattern period.
        inline float GetPatternPeriod() const
        {
            return m_PatternPeriod;
        }

    private:
        // Current camera.
        class DemoTriangleShaderWireframeCamera* m_Camera;

        // The demo entity handle.
        HENTITY m_DemoTriangleShaderWireframeHandle;

        // Window caption text update time.
        float m_UpdateTextTime;

        // The model rotation angle.
        float m_ModelRotateAngle;

        // Line width.
        float m_LineWidth;
        
        // Pattern period.
        float m_PatternPeriod;
    };
}

#endif // !__YW_DEMO_TRIANGLE_SHADER_WIREFRAME_APP_H__

// Add by Yaukey at 2020-02-02.
// YW Soft Renderer demo with triangle shader wireframe class.

#ifndef __YW_DEMO_TRIANGLE_SHADER_WIREFRAME_H__
#define __YW_DEMO_TRIANGLE_SHADER_WIREFRAME_H__

#include "Yw3d.h"
#include "YwEntity.h"
#include "YwResourceManager.h"

namespace yw
{
    class DemoTriangleShaderWireframe : public IEntity
    {
    protected:
        // Constructor.
        DemoTriangleShaderWireframe(class Scene* scene);

        // Destructor.
        ~DemoTriangleShaderWireframe();

    public:
        // The instance create function.
        inline static IEntity* Create(class Scene* scene)
        {
            return new DemoTriangleShaderWireframe(scene);
        }

        // Initial demo.
        bool Initialize();

        // Returns true if the object has been moved -> scenegraph-care.
        bool FrameMove();

        // Render with a specified pass.
        void Render(int32_t pass);

    private:
        // Model and texture resources.
        class Model* m_Model;

        // Resource handle.
        HRESOURCE m_ModelHandle;

        // Shader related.
        class DemoTriangleShaderWireframeVertexShader* m_VertexShader;
        class DemoTriangleShaderWireframeTriangleShader* m_TriangleShader;
        class DemoTriangleShaderWireframeDefaultPixelShader* m_DefaultPixelShader;
        class DemoTriangleShaderWireframePatternPixelShader* m_PatternPixelShader;

        // Shader parameters.
        float m_LineWidth;
        float m_FadeDistance;
        float m_PatternPeriod;
        Vector4 m_FillColor;
        Vector4 m_WireColor;
        Vector4 m_PatternColor;
    };
}

#endif // !__YW_DEMO_TRIANGLE_SHADER_WIREFRAME_H__

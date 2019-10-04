// Add by Yaukey at 2019-09-19.
// YW Soft Renderer demo with blinn-phong class.

#ifndef __YW_DEMO_BLINN_PHONG_H__
#define __YW_DEMO_BLINN_PHONG_H__

#include "Yw3d.h"
#include "YwEntity.h"
#include "YwResourceManager.h"

namespace yw
{
    class DemoBlinnPhong : public IEntity
    {
    public:
        // Define vertex format.
        struct Vertexformat
        {
            Vector3 position;
            Vector4 color;
        };

    protected:
        // Constructor.
        DemoBlinnPhong(class Scene* scene);

        // Destructor.
        ~DemoBlinnPhong();

    public:
        // The instance create function.
        inline static IEntity* CreateDemoBlinnPhong(class Scene* scene)
        {
            return new DemoBlinnPhong(scene);
        }

        // Initial demo model.
        bool Initialize();

        // Returns true if the object has been moved -> scenegraph-care.
        bool FrameMove();

        // Render with a specified pass.
        void Render(int32_t pass);

    private:
        // Model.
        class Model* m_Model;

        // Model resource handle.
        HRESOURCE m_ModelHandle;

        // Shader related.
        class DemoBlinnPhongVertexShader* m_VertexShader;
        class DemoBlinnPhongPixelShader* m_PixelShader;

        // Lighting params.
        Vector3 m_LightDirection;
        Vector4 m_LightColor;
        Vector4 m_AlbedoColor;
        Vector4 m_SpecularColor;
        float m_Specular;
        float m_Gloss;
    };
}

#endif // !__YW_DEMO_BLINN_PHONG_H__

// Add by Yaukey at 2020-01-23.
// YW Soft Renderer demo with normal-mapping class.

#ifndef __YW_DEMO_NORMAL_MAPPING_H__
#define __YW_DEMO_NORMAL_MAPPING_H__

#include "Yw3d.h"
#include "YwEntity.h"
#include "YwResourceManager.h"

namespace yw
{
    class DemoNormalMapping : public IEntity
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
        DemoNormalMapping(class Scene* scene);

        // Destructor.
        ~DemoNormalMapping();

    public:
        // The instance create function.
        inline static IEntity* Create(class Scene* scene)
        {
            return new DemoNormalMapping(scene);
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
        class Yw3dTexture* m_ModelTexture;
        class Yw3dTexture* m_ModelNormalTexture;

        // Resource handle.
        HRESOURCE m_ModelHandle;
        HRESOURCE m_ModelTextureHandle;
        HRESOURCE m_ModelNormalTextureHandle;

        // Shader related.
        class DemoNormalMappingVertexShader* m_VertexShader;
        class DemoNormalMappingPixelShader* m_PixelShader;

        // Lighting params.
        Vector3 m_LightDirection;
        Vector4 m_LightColor;
        Vector4 m_AlbedoColor;
        Vector4 m_SpecularColor;
        float m_Specular;
        float m_Gloss;
    };
}

#endif // !__YW_DEMO_NORMAL_MAPPING_H__

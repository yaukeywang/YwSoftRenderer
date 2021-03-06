// Add by Yaukey at 2020-02-24.
// YW Soft Renderer demo with PBR class.

#ifndef __YW_DEMO_PBR_H__
#define __YW_DEMO_PBR_H__

#include "Yw3d.h"
#include "YwEntity.h"
#include "YwResourceManager.h"

namespace yw
{
    class DemoPBR : public IEntity
    {
    protected:
        // Constructor.
        DemoPBR(class Scene* scene);

        // Destructor.
        ~DemoPBR();

    public:
        // The instance create function.
        inline static IEntity* Create(class Scene* scene)
        {
            return new DemoPBR(scene);
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
        class Yw3dTexture* m_ModelSpecularTexture;

        // Resource handle.
        HRESOURCE m_ModelHandle;
        HRESOURCE m_ModelTextureHandle;
        HRESOURCE m_ModelNormalTextureHandle;
        HRESOURCE m_ModelSpecularTextureHandle;

        // Shader related.
        class DemoPBRVertexShader* m_VertexShader;
        class DemoPBRPixelShader* m_PixelShader;

        // Lighting params.
        Vector3 m_LightDirection;
        Vector4 m_LightColor;
        Vector4 m_AlbedoColor;
        Vector4 m_SpecularColor;

        // _Metallic/"Metallic"
        float m_Metallic;

        // _Glossiness/"Smoothness"
        float m_Smoothness;

        // _GlossMapScale/"Smoothness Scale"
        float m_SmoothnessScale;
    };
}

#endif // !__YW_DEMO_PBR_H__

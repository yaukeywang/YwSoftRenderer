// Add by Yaukey at 2020-04-18.
// YW Soft Renderer demo with PBR-IBL class.

#ifndef __YW_DEMO_PBR_IBL_H__
#define __YW_DEMO_PBR_IBL_H__

#include "Yw3d.h"
#include "YwEntity.h"
#include "YwResourceManager.h"

namespace yw
{
    class DemoPBRIBL : public IEntity
    {
    protected:
        // Constructor.
        DemoPBRIBL(class Scene* scene);

        // Destructor.
        ~DemoPBRIBL();

    public:
        // The instance create function.
        inline static IEntity* Create(class Scene* scene)
        {
            return new DemoPBRIBL(scene);
        }

        // Initial demo.
        bool Initialize();

        // Returns true if the object has been moved -> scenegraph-care.
        bool FrameMove();

        // Render with a specified pass.
        void Render(int32_t pass);

    private:
        // Render all pre-computing data we nee.
        bool LoadAllPreComputingData();

        // Render sky environment.
        void RenderSky(int32_t pass);

        // Render pbr model.
        void RenderPbrModel(int32_t pass);

    private:
        // Model resources.
        class Model* m_ModelSphere;
        class Model* m_ModelCube;
        class Model* m_ModelPBR;

        // Pre-computing texture resources.
        StringA m_EnvEquirectangularTextureName;
        Yw3dTexture* m_EnvEquirectangularTexture;
        Yw3dCubeTexture* m_EnvCubeTexture;
        Yw3dCubeTexture* m_IrrandianceCubeTexture;
        Yw3dCubeTexture* m_PrefilterReflectionCubeTexture;
		Yw3dTexture* m_PreintegrateBRDFTexture;

        // Normal texture resources.
        class Yw3dTexture* m_ModelPBRTexture;
        class Yw3dTexture* m_ModelPBRNormalTexture;
        class Yw3dTexture* m_ModelPBRSpecularTexture;

        // Model Resource handle.
        HRESOURCE m_ModelSphereHandle;
        HRESOURCE m_ModelCubeHandle;
        HRESOURCE m_ModelPBRHandle;

        // Texture Resource handle.
        HRESOURCE m_EnvEquirectangularTextureHandle;
        HRESOURCE m_ModelPBRTextureHandle;
        HRESOURCE m_ModelPBRNormalTextureHandle;
        HRESOURCE m_ModelPBRSpecularTextureHandle;

        // Pre-computing texture handle.
        HRESOURCE m_EnvCubeTextureHandle;
        HRESOURCE m_IrrandianceCubeTextureHandle;
        HRESOURCE m_PrefilterReflectionCubeTextureHandle;
        HRESOURCE m_PreintegrateBRDFTextureHandle;

        // Shader related.
        class DemoPBRIBLCubeMapVertexShader* m_SkyVertexShader;
        class DemoPBRIBLCubeMapPixelShader* m_SkyPixelShader;
        class DemoPBRIBLVertexShader* m_PbrVertexShader;
        class DemoPBRIBLPixelShader* m_PbrPixelShader;

        // Shader uniform params.
        Vector3 m_Albedo;

        // Enable debug info enabled.
        bool m_DebugInfoEnabled;
    };
}

#endif // !__YW_DEMO_PBR_IBL_H__

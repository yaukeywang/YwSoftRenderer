// Add by Yaukey at 2019-09-19.
// YW Soft Renderer demo model class.

#ifndef __YW_DEMO_MODEL_H__
#define __YW_DEMO_MODEL_H__

#include "Yw3d.h"
#include "YwEntity.h"
#include "YwResourceManager.h"

namespace yw
{
    class DemoModel : public IEntity
    {
    protected:
        // Constructor.
        DemoModel(class Scene* scene);

        // Destructor.
        ~DemoModel();

    public:
        // The instance create function.
        inline static IEntity* CreateDemoModel(class Scene* scene)
        {
            return new DemoModel(scene);
        }

        // Initial demo model.
        bool Initialize();

        // Returns true if the object has been moved -> scenegraph-care.
        bool FrameMove();

        // Render with a specified pass.
        void Render(int32_t pass);

    private:
        // Model and texture.
        class Model* m_Model;
        class Yw3dTexture* m_ModelTexture;

        // Model and texture resource handle.
        HRESOURCE m_ModelHandle;
        HRESOURCE m_ModelTextureHandle;

        // Shader related.
        class DemoModelVertexShader* m_VertexShader;
        class DemoModelWireframePixelShader* m_PixelShaderWireframe;
        class DemoModelPixelShader* m_PixelShader;
    };
}

#endif // !__YW_DEMO_MODEL_H__

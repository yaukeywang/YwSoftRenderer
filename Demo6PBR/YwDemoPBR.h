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

        // Resource handle.
        HRESOURCE m_ModelHandle;

        // Shader related.
        class DemoPBRVertexShader* m_VertexShader;
        class DemoPBRPixelShader* m_PixelShader;
    };
}

#endif // !__YW_DEMO_PBR_H__

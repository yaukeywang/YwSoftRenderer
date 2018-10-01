// Add by Yaukey at 2018-09-19.
// YW Soft Renderer the base entity class.

#ifndef __YW_ENTITY_H__
#define __YW_ENTITY_H__

#include "Yw3dBase.h"

namespace yw
{
    class YwEntity
    {
        friend class Scene;

    public:
        // Constructor.
        YwEntity(class Scene* scene) : m_Scene(scene) 
        {
        }

        // Destructor.
        virtual ~YwEntity()
        {
            m_Scene = nullptr;
        }

    public:
        // Returns true if the object has been moved -> scenegraph-care
        virtual bool FrameMove() = 0;

        // Render with a specified pass.
        virtual void Render(int32_t pass) = 0;

    public:
        // Get curent parent scene.
        inline class Scene* GetScene()
        {
            return m_Scene;
        }

    private:
        class Scene* m_Scene;
    };
}

#endif // !__YW_ENTITY_H__

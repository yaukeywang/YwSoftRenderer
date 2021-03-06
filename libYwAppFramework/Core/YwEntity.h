// Add by Yaukey at 2018-09-19.
// YW Soft Renderer the base entity class.

#ifndef __YW_ENTITY_H__
#define __YW_ENTITY_H__

#include "Yw3dBase.h"

namespace yw
{
    class IEntity
    {
        friend class Scene;

	protected:
		// Constructor.
		IEntity(class Scene* scene) : m_Scene(scene)
		{
		}

    public:
        // Destructor.
        virtual ~IEntity()
        {
            m_Scene = nullptr;
        }

    public:
        // Returns true if the object has been moved -> scenegraph-care.
        virtual bool FrameMove() = 0;

        // Render with a specified pass.
        virtual void Render(int32_t pass) = 0;

    public:
        // Get current parent scene.
        inline class Scene* GetScene() const
        {
            return m_Scene;
        }

	protected:
        class Scene* m_Scene;
    };
}

#endif // !__YW_ENTITY_H__

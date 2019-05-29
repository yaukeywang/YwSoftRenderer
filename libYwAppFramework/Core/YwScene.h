// Add by Yaukey at 2018-09-19.
// YW Soft Renderer scene class.

#ifndef __YW_SCENE_H__
#define __YW_SCENE_H__

#include "Yw3d.h"
#include <map>
#include <unordered_map>

namespace yw
{
    // Define some type.
    typedef uint32_t HENTITY;
    typedef class IEntity* (*EntityCreateFunction)(class Scene* scene);

    // Implement scene - graph.
    class Scene
    {
        friend class IApplication;
        friend class Application;

    protected:
        // Constructor.
        Scene(class IApplication* application);

        // Destructor.
        ~Scene();

    protected:
        // Initialize the scene.
        bool Initialize();

        // Each frame move.
        void FrameMove();

    public:
        // Render with a specified pass.
        void Render(uint32_t pass);

        // Registry entity by name and creation function.
        void RegisterEntityType(const String& typeName, EntityCreateFunction createFunction);

        // Create a entity.
        // If sceneProcess is false, then the scene will not automatically call framemove/render - suited for sub-models.
        HENTITY CreateEntity(const String& typeName, bool sceneProcess = true);

        // Get a entity.
        class IEntity* GetEntity(HENTITY entityHandle) const;

        // Release entity.
        void ReleaseEntity(HENTITY entityHandle);

	public:
		// Light and so on...
		// ...

		// Get scene clear color.
		inline Vector4 GetClearColor() const
		{
			return m_ClearColor;
		}

		inline void SetClearColor(Vector4& clearColor)
		{
			m_ClearColor = clearColor;
		}

    public:
        // Get parent application.
        inline class IApplication* GetApplication()
        {
            return m_Application;
        }

    private:
        // Define scene internal entity.
        struct SceneEntity
        {
            HENTITY m_Handle;
            class IEntity* m_Entity;
            bool m_SceneProcess;
        };

        // Others...

    private:
        // The parent application.
        class IApplication* m_Application;

        // Add registry entity functions.
        std::map<String, EntityCreateFunction> m_RegisteredEntityTypes;

        // All entities.
        std::unordered_map<uint32_t, SceneEntity> m_SceneEntities;

        // Total number created entities.
        uint32_t m_NumberCreatedEntities;

		// Light and so on...
		// ...

		// Clear color.
		Vector4 m_ClearColor;
    };
}

#endif // !__YW_SCENE_H__

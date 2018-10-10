// Add by Yaukey at 2018-09-19.
// YW Soft Renderer scene class.

#include "YwScene.h"
#include "YwBaseApplication.h"
#include "YwEntity.h"
#include "YwGraphics.h"

namespace yw
{
    Scene::Scene(IApplication* application) :
        m_Application(application),
        m_NumberCreatedEntities(0)
    {
        // Init Light and so on...
    }

    Scene::~Scene()
    {
        // Release all entities.
        while (m_SceneEntities.size() > 0)
        {
            ReleaseEntity(m_SceneEntities.begin()->second.m_Handle);
        }

        // Release others.
        // ...
    }

    bool Scene::Initialize()
    {
        return true;
    }

    void Scene::FrameMove()
    {
        std::unordered_map<uint32_t, SceneEntity>::iterator it = m_SceneEntities.begin();
        for (; it != m_SceneEntities.end(); ++it)
        {
            SceneEntity* sceneEntity = &(it->second);
            if (!sceneEntity->m_SceneProcess)
            {
                continue;
            }

            // If true, update scene - graph.
            sceneEntity->m_Entity->FrameMove();
        }
    }

    Vector4 Scene::GetClearColor() const
    {
        return Vector4::Zero();
    }

    void Scene::Render(uint32_t pass)
    {
        Graphics* graphics = GetApplication()->GetGraphics();
        std::unordered_map<uint32_t, SceneEntity>::iterator it = m_SceneEntities.begin();
        for (; it != m_SceneEntities.end(); ++it)
        {
            SceneEntity* sceneEntity = &(it->second);
            if (!sceneEntity->m_SceneProcess)
            {
                continue;
            }

            // If true, render scene - graph.
            graphics->PushStateBlock();
            sceneEntity->m_Entity->Render(pass);
            graphics->PopStateBlock();
        }
    }

    void Scene::RegisterEntityType(String typeName, EntityCreateFunction createFunction)
    {
        m_RegisteredEntityTypes[typeName] = createFunction;
    }

    HENTITY Scene::CreateEntity(String typeName, bool sceneProcess)
    {
        // Get create function.
        EntityCreateFunction createFunction = m_RegisteredEntityTypes[typeName];
        if (nullptr == createFunction)
        {
            return 0;
        }

        // Create a new entity.
        IEntity* newEntity = createFunction(this);
        if (nullptr == newEntity)
        {
            return 0;
        }

        // Create a internal scene entity.
        SceneEntity sceneEntity = { ++m_NumberCreatedEntities, newEntity, sceneProcess };
        m_SceneEntities[sceneEntity.m_Handle] = (sceneEntity);

        return sceneEntity.m_Handle;
    }

    IEntity* Scene::GetEntity(HENTITY entityHandle) const
    {
        std::unordered_map<uint32_t, SceneEntity>::const_iterator it = m_SceneEntities.find(entityHandle);
        if (m_SceneEntities.end() == it)
        {
            return nullptr;
        }
        else
        {
            return it->second.m_Entity;
        }
    }

    void Scene::ReleaseEntity(HENTITY entityHandle)
    {
        std::unordered_map<uint32_t, SceneEntity>::iterator it = m_SceneEntities.find(entityHandle);
        if (m_SceneEntities.end() != it)
        {
            YW_SAFE_DELETE(it->second.m_Entity);
            m_SceneEntities.erase(entityHandle);
        }
    }
}

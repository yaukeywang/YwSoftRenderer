// Add by Yaukey at 2018-09-19.
// YW Soft Renderer scene class.

#include "YwScene.h"
#include "YwBaseApplication.h"

namespace yw
{
    Scene::Scene(IApplication* application) :
        m_Application(application)
    {
    }

    Scene::~Scene()
    {
    }

    bool Scene::Initialize()
    {
        return false;
    }

    void Scene::FrameMove()
    {

    }

    Vector4 Scene::GetClearColor() const
    {
        return Vector4::Zero();
    }

    void Scene::Render(uint32_t pass)
    {

    }
}

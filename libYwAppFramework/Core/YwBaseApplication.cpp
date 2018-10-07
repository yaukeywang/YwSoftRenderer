// Add by Yaukey at 2018-07-18.
// YW Soft Renderer application framework base application interface.

#include "YwBaseApplication.h"
#include "YwGraphics.h"
#include "YwScene.h"

namespace yw
{
    IApplication::IApplication() :
        m_WindowHandle(0),
        m_Windowed(true),
        m_Active(false),
        m_WindowWidth(0),
        m_WindowHeight(0),
        m_FPS(0.0f),
        m_InvFPS(0.0f),
        m_ElapsedTime(0.0f),
        m_FrameIdent(0),
        m_AppData(nullptr),
        m_Graphics(nullptr),
        m_Scene(nullptr)
    {

    }

    IApplication::~IApplication()
    {
        YW_SAFE_DELETE(m_Graphics);
        YW_SAFE_DELETE(m_Scene);
        YW_SAFE_DELETE_ARRAY(m_AppData);
    }

    bool IApplication::CreateSubSystems(const ApplicationCreationFlags& creationFlags)
    {
        // Create graphics.
        m_Graphics = new Graphics(this);
        if ((nullptr == m_Graphics) || !m_Graphics->Initialize(creationFlags))
        {
            return false;
        }

        // Create scene.
        m_Scene = new Scene(this);
        if ((nullptr == m_Scene) || !m_Scene->Initialize())
        {
            return false;
        }

        // Last init.
        if (!CreateWorld())
        {
            DestroyWorld();
            return false;
        }

        return true;
    }
}

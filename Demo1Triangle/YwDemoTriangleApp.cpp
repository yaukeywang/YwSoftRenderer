// Add by Yaukey at 2018-10-10.
// YW Soft Renderer demo triangle application class.

#include "YwDemoTriangleApp.h"
#include "YwDemoTriangle.h"
#include "YwDemoTriangleCamera.h"
#include "YwGraphics.h"
#include "YwScene.h"

namespace yw
{
    DemoTriangleApp::DemoTriangleApp() :
        m_Camera(nullptr),
        m_DemoTriangleHandle(0),
        m_UpdateTextTime(0.0f)
    {

    }

    DemoTriangleApp::~DemoTriangleApp()
    {
    }

    bool DemoTriangleApp::CreateWorld()
    {
        // Create camera.
        m_Camera = new DemoTriangleCamera(GetGraphics());
        if (!m_Camera->CreateRenderCamera(GetWindowWidth(), GetWindowHeight()))
        {
            return false;
        }

        // Calculation projection matrix.
        m_Camera->CalculateProjection(YW_PI / 6.0f, 4.0f / 3.0f, 1.0f, 1000.0f);

        // Calculation view matrix.
        m_Camera->SetPosition(Vector3(0.0f, 0.0f, -10.0f));
        m_Camera->SetLookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
        m_Camera->CalculateView();

        GetScene()->SetClearColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));

        // Registry a demo triangle entity and create an instance.
        GetScene()->RegisterEntityType(_T("DemoTriangle"), DemoTriangle::CreateDemoTriangle);
        m_DemoTriangleHandle = GetScene()->CreateEntity(_T("DemoTriangle"));
        if (0 == m_DemoTriangleHandle)
        {
            return false;
        }

        DemoTriangle* demoTriangle = (DemoTriangle*)GetScene()->GetEntity(m_DemoTriangleHandle);
        if (!demoTriangle->Initialize(
            Vector3(-1.0f, 0.0f, 0.0f), Vector4::Red(),
            Vector3(0.0f, 1.41421f, 0.0f), Vector4::Green(),
            Vector3(1.0f, 0.0f, 0.0f), Vector4::Blue()
        ))
        {
            return false;
        }

        return true;
    }

    void DemoTriangleApp::DestroyWorld()
    {
        GetScene()->ReleaseEntity(m_DemoTriangleHandle);
        YW_SAFE_DELETE(m_Camera);
    }

    void DemoTriangleApp::FrameMove()
    {
        float elapsedTime = GetElapsedTime();
        if (elapsedTime - m_UpdateTextTime > 0.5f)
        {
            // Tag caption text update time.
            m_UpdateTextTime = elapsedTime;

            // Update window caption text.
            #if defined(_WIN32) || defined(WIN32)
                #ifdef _UNICODE
                    wchar_t szCaption[256];
                    swprintf(szCaption, L"DemoTriangle, FPS: %3.2f", GetFPS());
                #else
                    char szCaption[256];
                    sprintf(szCaption, "DemoTriangle, FPS: %3.2f", GetFPS());
                #endif

                SetWindowText(GetWindowHandle(), szCaption);
            #elif LINUX_X11
                char szCaption[256];
                sprintf(szCaption, "DemoTriangle, FPS: %3.2f", GetFPS());
                XStoreName((Display*)GetDisplay(), GetWindowHandle(), szCaption);
            #elif __amigaos4__
                static char szCaption[256];
                sprintf(szCaption, "DemoTriangle, FPS: %3.2f", GetFPS());
                IIntuition->SetWindowTitles(GetWindowHandle(), szCaption, szCaption);
            #endif
        }
    }

    void DemoTriangleApp::Render()
    {
        if (nullptr == m_Camera)
        {
            return;
        }

        m_Camera->BeginRender();
        m_Camera->ClearToSceneColor();
        m_Camera->RenderPass(-1);
        m_Camera->EndRender(true);
    }
}

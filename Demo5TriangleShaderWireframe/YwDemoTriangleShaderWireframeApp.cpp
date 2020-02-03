// Add by Yaukey at 2020-01-23.
// YW Soft Renderer demo with normal-mapping application class.

#include "YwDemoTriangleShaderWireframeApp.h"
#include "YwDemoTriangleShaderWireframe.h"
#include "YwDemoTriangleShaderWireframeCamera.h"
#include "YwInput.h"
#include "YwGraphics.h"
#include "YwScene.h"

namespace yw
{
    DemoTriangleShaderWireframeApp::DemoTriangleShaderWireframeApp() :
        m_Camera(nullptr),
        m_DemoTriangleShaderWireframeHandle(0),
        m_UpdateTextTime(0.0f),
        m_ModelRotateAngle(0.0f)
    {

    }

    DemoTriangleShaderWireframeApp::~DemoTriangleShaderWireframeApp()
    {
    }

    bool DemoTriangleShaderWireframeApp::CreateWorld()
    {
        // Create camera.
        m_Camera = new DemoTriangleShaderWireframeCamera(GetGraphics());
        if (!m_Camera->CreateRenderCamera(GetWindowWidth(), GetWindowHeight()))
        {
            return false;
        }

        // Calculation projection matrix.
        m_Camera->CalculateProjection(YW_PI / 6.0f, 4.0f / 3.0f, 1.0f, 100.0f);

        // Calculation view matrix.
        m_Camera->SetPosition(Vector3(0.0f, 2.5f, -2.0f));
        m_Camera->SetLookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
        m_Camera->CalculateView();

        Vector4 clearColor(0.0f, 0.0f, 0.0f, 0.0f);
        GetScene()->SetClearColor(clearColor);

        // Registry a demo model entity and create an instance.
        GetScene()->RegisterEntityType(_T("DemoTriangleShaderWireframe"), DemoTriangleShaderWireframe::Create);
        m_DemoTriangleShaderWireframeHandle = GetScene()->CreateEntity(_T("DemoTriangleShaderWireframe"));
        if (0 == m_DemoTriangleShaderWireframeHandle)
        {
            return false;
        }

        DemoTriangleShaderWireframe* demoTriangleShaderWireframe = (DemoTriangleShaderWireframe*)GetScene()->GetEntity(m_DemoTriangleShaderWireframeHandle);
        if (!demoTriangleShaderWireframe->Initialize())
        {
            return false;
        }

        return true;
    }

    void DemoTriangleShaderWireframeApp::DestroyWorld()
    {
        GetScene()->ReleaseEntity(m_DemoTriangleShaderWireframeHandle);
        YW_SAFE_DELETE(m_Camera);
    }

    void DemoTriangleShaderWireframeApp::FrameMove()
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
                    swprintf(szCaption, L"DemoTriangleShaderWireframe, FPS: %3.2f", GetFPS());
                #else
                    char szCaption[256];
                    sprintf(szCaption, "DemoTriangleShaderWireframe, FPS: %3.2f", GetFPS());
                #endif

                SetWindowText(GetWindowHandle(), szCaption);
            #elif defined(LINUX_X11) || defined(_LINUX)
                char szCaption[256];
                sprintf(szCaption, "DemoTriangleShaderWireframe, FPS: %3.2f", GetFPS());
                XStoreName((Display*)GetDisplay(), GetWindowHandle(), szCaption);
            #elif defined(_MAC_OSX)
                //#error "Window caption is not implemented!"
            #elif defined(__amigaos4__) || defined(_AMIGAOS4)
                static char szCaption[256];
                sprintf(szCaption, "DemoTriangleShaderWireframe, FPS: %3.2f", GetFPS());
                IIntuition->SetWindowTitles(GetWindowHandle(), szCaption, szCaption);
            #endif
        }

        // Update rotation angle.
        m_ModelRotateAngle += GetDeltaTime() * 0.5f;
        if (m_Input->MouseButtonDown(0))
        {
            int32_t deltaX = 0;
            int32_t deltaY = 0;
            m_Input->GetMouseMovement(&deltaX, &deltaY);
            //m_ModelRotateAngle -= (float)deltaX * 0.015f;
        }
    }

    void DemoTriangleShaderWireframeApp::Render()
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

    float DemoTriangleShaderWireframeApp::GetModelRotationAngle() const
    {
        return m_ModelRotateAngle;
    }
}

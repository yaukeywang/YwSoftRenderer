// Add by Yaukey at 2019-09-19.
// YW Soft Renderer demo model application class.

#include "YwDemoModelApp.h"
#include "YwDemoModel.h"
#include "YwDemoModelCamera.h"
#include "YwInput.h"
#include "YwGraphics.h"
#include "YwScene.h"

namespace yw
{
    DemoModelApp::DemoModelApp() :
        m_Camera(nullptr),
        m_DemoModelHandle(0),
        m_UpdateTextTime(0.0f),
        m_RotateAngle(0.0f)
    {

    }

    DemoModelApp::~DemoModelApp()
    {
    }

    bool DemoModelApp::CreateWorld()
    {
        // Create camera.
        m_Camera = new DemoModelCamera(GetGraphics());
        if (!m_Camera->CreateRenderCamera(GetWindowWidth(), GetWindowHeight()))
        {
            return false;
        }

        // Calculation projection matrix.
        m_Camera->CalculateProjection(YW_PI / 6.0f, 4.0f / 3.0f, 1.0f, 1000.0f);

        // Calculation view matrix.
        m_Camera->SetPosition(Vector3(0.0f, 1.0f, -1.5f));
        m_Camera->SetLookAt(Vector3(0.0f, 0.2f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
        m_Camera->CalculateView();

        Vector4 clearColor(0.0f, 0.0f, 0.0f, 0.0f);
        GetScene()->SetClearColor(clearColor);

        // Registry a demo model entity and create an instance.
        GetScene()->RegisterEntityType(_T("DemoModel"), DemoModel::CreateDemoModel);
        m_DemoModelHandle = GetScene()->CreateEntity(_T("DemoModel"));
        if (0 == m_DemoModelHandle)
        {
            return false;
        }

        DemoModel* demoModel = (DemoModel*)GetScene()->GetEntity(m_DemoModelHandle);
        if (!demoModel->Initialize())
        {
            return false;
        }

        return true;
    }

    void DemoModelApp::DestroyWorld()
    {
        GetScene()->ReleaseEntity(m_DemoModelHandle);
        YW_SAFE_DELETE(m_Camera);
    }

    void DemoModelApp::FrameMove()
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
                    swprintf(szCaption, L"DemoModel, FPS: %3.2f", GetFPS());
                #else
                    char szCaption[256];
                    sprintf(szCaption, "DemoModel, FPS: %3.2f", GetFPS());
                #endif

                SetWindowText(GetWindowHandle(), szCaption);
            #elif defined(LINUX_X11) || defined(_LINUX)
                char szCaption[256];
                sprintf(szCaption, "DemoModel, FPS: %3.2f", GetFPS());
                XStoreName((Display*)GetDisplay(), GetWindowHandle(), szCaption);
            #elif defined(_MAC_OSX)
                //#error "Window caption is not implemented!"
            #elif defined(__amigaos4__) || defined(_AMIGAOS4)
                static char szCaption[256];
                sprintf(szCaption, "DemoModel, FPS: %3.2f", GetFPS());
                IIntuition->SetWindowTitles(GetWindowHandle(), szCaption, szCaption);
            #endif
        }

        // Update rotation angle.
        if (m_Input->MouseButtonDown(0))
        {
            int32_t deltaX = 0;
            int32_t deltaY = 0;
            m_Input->GetMouseMovement(&deltaX, &deltaY);
            m_RotateAngle -= (float)deltaX * 0.015f;
        }
        else
        {
            m_RotateAngle += GetDeltaTime() * 3.0f;
        }
    }

    void DemoModelApp::Render()
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

    float DemoModelApp::GetRotationAngle() const
    {
        return m_RotateAngle;
    }
}

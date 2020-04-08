// Add by Yaukey at 2020-02-24.
// YW Soft Renderer demo with PBR application class.

#include "YwDemoPBRApp.h"
#include "YwDemoPBR.h"
#include "YwDemoPBRCamera.h"
#include "YwInput.h"
#include "YwGraphics.h"
#include "YwScene.h"

namespace yw
{
    DemoPBRApp::DemoPBRApp() :
        m_Camera(nullptr),
        m_DemoPBRHandle(0),
        m_UpdateTextTime(0.0f),
        m_LightRotateAngle(0.0f),
        m_Metallic(0.4f),
        m_Smoothness(0.5f),
        m_Drag(false)
    {
        m_ArcBall.Reset();
    }

    DemoPBRApp::~DemoPBRApp()
    {
    }

    bool DemoPBRApp::CreateWorld()
    {
        // Create camera.
        m_Camera = new DemoPBRCamera(GetGraphics());
        if (!m_Camera->Initialize(GetWindowWidth(), GetWindowHeight(), 1.0f, 1.0f, YW_PI / 6.0f, 4.0f / 3.0f, 0.1f, 100.0f, Vector3(0.0f, 0.0f, -1.5f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), 0.5f, 5.0f))
        {
            return false;
        }

        Vector4 clearColor(54.0f / 255.0f, 77.0f / 255.0f, 118.0f / 255.0f, 255.0f);
        GetScene()->SetClearColor(clearColor);

        // Registry a demo model entity and create an instance.
        GetScene()->RegisterEntityType(_T("DemoPBR"), DemoPBR::Create);
        m_DemoPBRHandle = GetScene()->CreateEntity(_T("DemoPBR"));
        if (0 == m_DemoPBRHandle)
        {
            return false;
        }

        DemoPBR* demoPBR = (DemoPBR*)GetScene()->GetEntity(m_DemoPBRHandle);
        if (!demoPBR->Initialize())
        {
            return false;
        }

        // Init the viewing arc ball.
        m_ArcBall.SetWindow(GetWindowWidth(), GetWindowHeight(), 1.0f);

        return true;
    }

    void DemoPBRApp::DestroyWorld()
    {
        GetScene()->ReleaseEntity(m_DemoPBRHandle);
        YW_SAFE_DELETE(m_Camera);
    }

    void DemoPBRApp::FrameMove()
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
                    swprintf(szCaption, L"DemoPBR, FPS: %3.2f", GetFPS());
                #else
                    char szCaption[256];
                    sprintf(szCaption, "DemoPBR, FPS: %3.2f", GetFPS());
                #endif

                SetWindowText(GetWindowHandle(), szCaption);
            #elif defined(LINUX_X11) || defined(_LINUX)
                char szCaption[256];
                sprintf(szCaption, "DemoPBR, FPS: %3.2f", GetFPS());
                XStoreName((Display*)GetDisplay(), GetWindowHandle(), szCaption);
            #elif defined(_MAC_OSX)
                //#error "Window caption is not implemented!"
            #elif defined(__amigaos4__) || defined(_AMIGAOS4)
                static char szCaption[256];
                sprintf(szCaption, "DemoPBR, FPS: %3.2f", GetFPS());
                IIntuition->SetWindowTitles(GetWindowHandle(), szCaption, szCaption);
            #endif
        }

        // Get mouse wheel.
        int32_t mouseWheel = m_Input->GetMouseWheelMovement();
        if (0 != mouseWheel)
        {
            //Vector3 cameraPos = m_Camera->GetPosition();
            //cameraPos.z = Clamp(cameraPos.z + 0.00015f * (float)mouseWheel, -3.5f, -0.5f);
            //m_Camera->SetPosition(cameraPos);
            //m_Camera->CalculateView();

            m_Camera->OnScroll(mouseWheel);
        }

        // Get mouse position.
        int32_t mouseX = 0;
        int32_t mouseY = 0;
        m_Input->GetMousePosition(&mouseX, &mouseY);

        // Check mouse down to do the operation.
        if (m_Input->MouseButtonDown(0))
        {
            //int32_t deltaX = 0;
            //int32_t deltaY = 0;
            //m_Input->GetMouseMovement(&deltaX, &deltaY);

            if (!m_Drag)
            {
                m_Drag = true;
                m_ArcBall.OnBegin(mouseX, mouseY);
                m_Camera->OnBeginViewArcBall(mouseX, mouseY);
            }
            else
            {
                m_ArcBall.OnMove(mouseX, mouseY);
                m_Camera->OnMoveViewArcBall(mouseX, mouseY);

                //Quaternion inv;
                //QuaternionInverse(inv, m_ArcBall.GetRotation());
                //m_Camera->SetRotationByArchBall(m_ArcBall.GetRotationMatrix());
                //m_Camera->CalculateView();
            }
        }
        else
        {
            if (m_Drag)
            {
                m_Drag = false;
                m_ArcBall.OnEnd();
                m_Camera->OnEndViewArcBall();

                //Quaternion inv;
                //QuaternionInverse(inv, m_ArcBall.GetRotation());
                //m_Camera->SetRotationByArchBall(m_ArcBall.GetRotationMatrix());
                //m_Camera->CalculateView();
            }
        }
    }

    void DemoPBRApp::Render()
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

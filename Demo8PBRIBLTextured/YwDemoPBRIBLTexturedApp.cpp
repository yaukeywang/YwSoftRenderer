// Add by Yaukey at 2022-06-04.
// YW Soft Renderer demo with PBR-IBL-Textured application class.

#include "YwDemoPBRIBLTexturedApp.h"
#include "YwDemoPBRIBLTextured.h"
#include "YwDemoPBRIBLTexturedCamera.h"
#include "YwInput.h"
#include "YwGraphics.h"
#include "YwScene.h"

namespace yw
{
    DemoPBRIBLTexturedApp::DemoPBRIBLTexturedApp() :
        m_Camera(nullptr),
        m_DemoPBRIBLTexturedHandle(0),
        m_UpdateTextTime(0.0f),
        m_Drag(false)
    {
        
    }

    DemoPBRIBLTexturedApp::~DemoPBRIBLTexturedApp()
    {
    }

    bool DemoPBRIBLTexturedApp::CreateWorld()
    {
        // Create camera.
        Quaternion initRotation;
        //QuaternionFromEuler(initRotation, 0.0f, 90.0f * DEG_TO_RAD, 0.0f);
        QuaternionFromEuler(initRotation, 0.0f, 0.0f, 0.0f);
        m_Camera = new DemoPBRIBLTexturedCamera(GetGraphics());
        if (!m_Camera->Initialize(
            GetWindowWidth(), GetWindowHeight(), 
            YW_PI / 4.0f, 4.0f / 3.0f, 0.1f, 100.0f, 
            Vector3(0.0f, 0.0f, -2.5f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), 
            0.5f, 5.5f, 
            false, 1.0f, ABCRA_All, initRotation,
            true, 1.0f, ABCRA_All, initRotation
        ))
        {
            return false;
        }

        Vector4 clearColor(54.0f / 255.0f, 77.0f / 255.0f, 118.0f / 255.0f, 255.0f);
        GetScene()->SetClearColor(clearColor);

        // Registry a demo model entity and create an instance.
        GetScene()->RegisterEntityType(_T("DemoPBRIBLTextured"), DemoPBRIBLTextured::Create);
        m_DemoPBRIBLTexturedHandle = GetScene()->CreateEntity(_T("DemoPBRIBLTextured"));
        if (0 == m_DemoPBRIBLTexturedHandle)
        {
            return false;
        }

        DemoPBRIBLTextured* demoPBRIBLTextured = (DemoPBRIBLTextured*)GetScene()->GetEntity(m_DemoPBRIBLTexturedHandle);
        if (!demoPBRIBLTextured->Initialize())
        {
            return false;
        }

        return true;
    }

    void DemoPBRIBLTexturedApp::DestroyWorld()
    {
        GetScene()->ReleaseEntity(m_DemoPBRIBLTexturedHandle);
        YW_SAFE_DELETE(m_Camera);
    }

    void DemoPBRIBLTexturedApp::FrameMove()
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
                    swprintf(szCaption, L"DemoPBRIBLTextured, FPS: %3.2f", GetFPS());
                #else
                    char szCaption[256];
                    sprintf(szCaption, "DemoPBRIBLTextured, FPS: %3.2f", GetFPS());
                #endif

                SetWindowText(GetWindowHandle(), szCaption);
            #elif defined(LINUX_X11) || defined(_LINUX)
                char szCaption[256];
                sprintf(szCaption, "DemoPBRIBLTextured, FPS: %3.2f", GetFPS());
                XStoreName((Display*)GetDisplay(), GetWindowHandle(), szCaption);
            #elif defined(_MAC_OSX)
                //#error "Window caption is not implemented!"
            #elif defined(__amigaos4__) || defined(_AMIGAOS4)
                static char szCaption[256];
                sprintf(szCaption, "DemoPBRIBLTextured, FPS: %3.2f", GetFPS());
                IIntuition->SetWindowTitles(GetWindowHandle(), szCaption, szCaption);
            #endif
        }

        // Get mouse wheel.
        int32_t mouseWheel = m_Input->GetMouseWheelMovement();
        if (0 != mouseWheel)
        {
            m_Camera->OnScroll(mouseWheel);
        }

        // Get mouse position.
        int32_t mouseX = 0;
        int32_t mouseY = 0;
        m_Input->GetMousePosition(&mouseX, &mouseY);

        // Check mouse down to do the operation.
        if (m_Input->MouseButtonDown(0))
        {
            if (!m_Drag)
            {
                m_Drag = true;
                m_Camera->OnBegin(mouseX, mouseY);
            }
            else
            {
                m_Camera->OnMove(mouseX, mouseY);
            }
        }
        else
        {
            if (m_Drag)
            {
                m_Drag = false;
                m_Camera->OnEnd();
            }
        }
    }

    void DemoPBRIBLTexturedApp::Render()
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

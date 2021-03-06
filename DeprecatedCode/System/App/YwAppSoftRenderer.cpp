// Add by yaukey at 2017-08-22.
// YW soft renderer app runner.

#include "stdafx.h"
#include "YwAppSoftRenderer.h"
#include "Driven/Renderer/YwRenderer.h"

namespace yw
{
	//////////////////////////////////////////////////////////////////////////
	AppSoftRenderer* AppSoftRenderer::s_pAppSoftRenderer = nullptr;

	AppSoftRenderer::AppSoftRenderer(HINSTANCE hInstance) :
		AppBase(hInstance, _T("YW Soft Renderer Application"), 800, 600, true, IRenderer::RendererType::RT_SOFT_D3D9),
		m_FrameCount(0.0f),
		m_TimeElapsed(0.0f),
		m_Fps(0.0f)
	{

	}

	AppSoftRenderer::AppSoftRenderer(HINSTANCE hInstance, LPCTSTR pCaption, int32_t nWidth, int32_t nHeight) :
		AppBase(hInstance, pCaption, nWidth, nHeight, true, IRenderer::RendererType::RT_SOFT_D3D9),
		m_FrameCount(0.0f),
		m_TimeElapsed(0.0f),
		m_Fps(0.0f)
	{
		s_pAppSoftRenderer = this;
	}

	AppSoftRenderer::~AppSoftRenderer()
	{
		// Your code below.

		s_pAppSoftRenderer = nullptr;
	}

	bool AppSoftRenderer::CreateEnvironment()
	{
		// Check device type.
		if (!CheckDeviceType())
		{
			MessageBox(nullptr, _T("AppSoftRenderer::CreateD3dEnvironment::CheckDeviceType() Failed!"), nullptr, 0);
			return false;
		}

		// Your code below.

		return true;
	}

    bool AppSoftRenderer::Initialize()
    {
        // Base initialize first.
        AppBase::Initialize();

        // Create d3d environment.
        if (!CreateEnvironment())
        {
            PostQuitMessage(0);
        }

        return true;
    }

    void AppSoftRenderer::Release()
    {
        // Base release last.
        AppBase::Release();
    }

	bool AppSoftRenderer::CheckDeviceType()
	{
		if (!AppBase::CheckDeviceType())
		{
			return false;
		}

		// Nothing to check.
		return true;
	}

	void AppSoftRenderer::OnLostDevice()
	{
		AppBase::OnLostDevice();

		// Your code below.
	}

	void AppSoftRenderer::OnResetDevice()
	{
		AppBase::OnResetDevice();

		// Your code below.
	}

	void AppSoftRenderer::UpdateScene(float fTimeDelta)
	{
		// Update frame rate.
		UpdateFrameCount(fTimeDelta);

		// Update view matrix.
		//m_pD3dDevice->SetTransform(D3DTS_VIEW, m_cCamera.GetViewMatrix());

		// Your code above.
		AppBase::UpdateScene(fTimeDelta);
	}

	void AppSoftRenderer::DrawScene()
	{
        if (nullptr == m_Renderer)
        {
            return;
        }

        m_Renderer->Clear();
        m_Renderer->BeginScene();

        // Your code below.

        m_Renderer->EndScene();
	}

	void AppSoftRenderer::OnMouseMove(DWORD dwKeyCode, int32_t nXpos, int32_t nYpos)
	{

	}

	void AppSoftRenderer::OnLButtonDown(DWORD dwKeyCode, int32_t nXpos, int32_t nYpos)
	{

	}

	void AppSoftRenderer::OnLButtonUp(DWORD dwKeyCode, int32_t nXpos, int32_t nYpos)
	{

	}

	void AppSoftRenderer::UpdateFrameCount(float fTimeDelta)
	{
		m_FrameCount += 1.0f;
		m_TimeElapsed += fTimeDelta;
		if (m_TimeElapsed >= 1.0f)
		{
			m_Fps = m_FrameCount / m_TimeElapsed;

			m_FrameCount = 0.0f;
			m_TimeElapsed = 0.0f;
		}
	}
}

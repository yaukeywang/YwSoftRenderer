// Add by yaukey at 2017-08-22.
// YW soft renderer app runner.

#include "stdafx.h"
#include "AppSoftRenderer.h"

namespace yw
{
	//////////////////////////////////////////////////////////////////////////
	AppSoftRenderer* AppSoftRenderer::s_pAppSoftRenderer = nullptr;

	AppSoftRenderer::AppSoftRenderer(HINSTANCE hInstance) :
		AppBase(hInstance, _T("YW Soft Renderer Application"), 800, 600, true, D3DDEVTYPE_HAL, D3DCREATE_SOFTWARE_VERTEXPROCESSING),
		m_FrameCount(0.0f),
		m_TimeElapsed(0.0f),
		m_Fps(0.0f)
	{

	}

	AppSoftRenderer::AppSoftRenderer(HINSTANCE hInstance, LPCTSTR pCaption, int nWidth, int nHeight, bool bWindowed, D3DDEVTYPE devType, DWORD dwRequestedV, LPCTSTR pFontName, int nFontHeight) :
		AppBase(hInstance, pCaption, nWidth, nHeight, bWindowed, devType, dwRequestedV),
		m_FrameCount(0.0f),
		m_TimeElapsed(0.0f),
		m_Fps(0.0f)
	{
		// Create d3d environment.
		if (!CreateD3dEnvironment())
		{
			PostQuitMessage(0);
		}

		s_pAppSoftRenderer = this;
	}

	AppSoftRenderer::~AppSoftRenderer()
	{
		// Your code below.

		s_pAppSoftRenderer = nullptr;
	}

	bool AppSoftRenderer::CreateD3dEnvironment()
	{
		// Check device type.
		if (!CheckDeviceType())
		{
			MessageBox(nullptr, _T("CheckDeviceCaps() Failed"), nullptr, 0);
			return false;
		}

		// Your code below.

		return true;
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
		HR(m_pD3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0));
		HR(m_pD3dDevice->BeginScene());

		// Your code below.

		HR(m_pD3dDevice->EndScene());
		HR(m_pD3dDevice->Present(NULL, NULL, NULL, NULL));
	}

	void AppSoftRenderer::OnMouseMove(DWORD dwKeyCode, int nXpos, int nYpos)
	{

	}

	void AppSoftRenderer::OnLButtonDown(DWORD dwKeyCode, int nXpos, int nYpos)
	{

	}

	void AppSoftRenderer::OnLButtonUp(DWORD dwKeyCode, int nXpos, int nYpos)
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

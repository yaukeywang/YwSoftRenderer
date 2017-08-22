// Add by yaukey at 2017-08-22.
// YW soft renderer app runner.


#ifndef __APP_SOFT_RENDERER_H__
#define __APP_SOFT_RENDERER_H__

#include "AppBase.h"

namespace yw
{
	class AppSoftRenderer : public AppBase
	{
	public:
		AppSoftRenderer(HINSTANCE hInstance);
		AppSoftRenderer(HINSTANCE hInstance, LPCTSTR pCaption, int nWidth, int nHeight, bool bWindowed, D3DDEVTYPE devType, DWORD dwRequestedV, LPCTSTR pFontName, int nFontHeight);
		~AppSoftRenderer();

	public:
		// Init all d3d settings.
		bool CreateD3dEnvironment();

	public:
		// Check d3d device type.
		virtual bool CheckDeviceType();

		// On losting device.
		virtual void OnLostDevice();

		// On Reseting device.
		virtual void OnResetDevice();

		// Update scene.
		virtual void UpdateScene(float fTimeDelta);

		// Draw stuff in the scene.
		virtual void DrawScene();

		// Mouse move event.
		virtual void OnMouseMove(DWORD dwKeyCode, int nXpos, int nYpos);

		// Mouse left button down.
		virtual void OnLButtonDown(DWORD dwKeyCode, int nXpos, int nYpos);

		// Mouse left button up.
		virtual void OnLButtonUp(DWORD dwKeyCode, int nXpos, int nYpos);

	protected:
		void UpdateFrameCount(float fTimeDelta);

	public:
		static AppSoftRenderer* s_pAppSoftRenderer;

	protected:
		// Frame count.
		float m_FrameCount;

		// How many time has elapsed.
		float m_TimeElapsed;

		// Current fps.
		float m_Fps;
	};
}

#endif // !__APP_SOFT_RENDERER_H__


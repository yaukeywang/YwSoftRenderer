/********************************************************************
created:	2010-11-01
filename: 	TrfAppBase.h
author:		Yaukey

purpose:	Application base class.
            This is a part of yw soft renderer.(2017-08-22)
*********************************************************************/

#ifndef _YW_APPBASE_H_
#define _YW_APPBASE_H_

#include "System/Support/YwBase.h"
#include "Driven/Renderer/YwRenderer.h"

namespace yw
{
	//////////////////////////////////////////////////////////////////////////
	// Application base class.
	class AppBase
	{
	public:
		AppBase(HINSTANCE hInstance, LPCTSTR pCaption, int32_t nWidth, int32_t nHeight, bool bWindowed, IRenderer::RendererType rendererType);
		virtual ~AppBase();

	public:
		//////////////////////////////////////////////////////////////////////////
		// Base method.

        // The entry of the initialize.
        virtual bool Initialize();

        // The release function.
        virtual void Release();

		// Initialize the main window.
		virtual bool InitMainWindow();

		// Initialize the renderer device.
		virtual bool InitRenderer();

		// Run the application circle.
		virtual int32_t Run();

		// Windows msg proc.
		virtual LRESULT MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		// Enable full screen mode or not.
		void EnableFullScreenMode(bool bEnable);

		// Is device lost.
		bool IsDeviceLost();

		//////////////////////////////////////////////////////////////////////////
		// Overloaded render method.

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

		//////////////////////////////////////////////////////////////////////////
		// Overloaded input method.

		// Window has been activated.
		virtual void OnActivate();

		// Window has been deactivated.
		virtual void OnDeactivate();

		// Close button is clicked event.
		virtual void OnClose();

		// Key down event.
		virtual void OnKeyDown(DWORD dwKeyCode, DWORD dwExtraParam);

		// Key up event.
		virtual void OnKeyUp(DWORD dwKeyCode, DWORD dwExtraParam);

		// Mouse move event.
		virtual void OnMouseMove(DWORD dwKeyCode, int32_t nXpos, int32_t nYpos);

		// Mouse left button down.
		virtual void OnLButtonDown(DWORD dwKeyCode, int32_t nXpos, int32_t nYpos);

		// Mouse left button up.
		virtual void OnLButtonUp(DWORD dwKeyCode, int32_t nXpos, int32_t nYpos);

		// Mouse left button double click.
		virtual void OnLButtonDBClick(DWORD dwKeyCode, int32_t nXpos, int32_t nYpos);

		// Mouse right button down.
		virtual void OnRButtonDown(DWORD dwKeyCode, int32_t nXpos, int32_t nYpos);

		// Mouse right button up.
		virtual void OnRButtonUp(DWORD dwKeyCode, int32_t nXpos, int32_t nYpos);

		// Mouse right button double click.
		virtual void OnRButtonDBClick(DWORD dwKeyCode, int32_t nXpos, int32_t nYpos);

		// Mouse middle button down.
		virtual void OnMButtonDwon(DWORD dwKeyCode, int32_t nXpos, int32_t nYpos);

		// Mouse middle button up.
		virtual void OnMButtonUp(DWORD dwKeyCode, int32_t nXpos, int32_t nYpos);

		// Mouse middle double click.
		virtual void OnMButtonDBClick(DWORD dwKeyCode, int32_t nXpos, int32_t nYpos);

		// Mouse wheel is rotated.
		virtual void OnMouseWheel(DWORD dwKeyCode, int16_t sDelta, int32_t nXpos, int32_t nYpos);

		//////////////////////////////////////////////////////////////////////////
		// Some "get" method.

		// Get the application instance.
		inline HINSTANCE GetAppInst() const
		{
			return m_hAppInst;
		}

		// Get the main window handle.
		inline HWND GetMainWnd() const
		{
			return m_hMainWnd;
		}

        inline IRenderer* GetRenderer()
        {
            return m_Renderer;
        }

    protected:
        IRenderer* GetRendererByType(IRenderer::RendererType rendererType);

	public:
		static AppBase* s_pAppBase;

	protected:
        // Windows caption.
		String m_strMainWndCaption;

        // Application instance.
		HINSTANCE m_hAppInst;

        // Main window handle.
		HWND m_hMainWnd;

        // Is application paused or not.
		bool m_bAppPaused;

        // Width of the client area.
		int32_t m_nWidth;

        // Height of the client area.
		int32_t m_nHeight;

        // Windowed or not.
        bool m_Windowed;

        // The type of the renderer.
        IRenderer::RendererType m_RendererType;

        // The renderer of this app.
        IRenderer* m_Renderer;
	};

	//////////////////////////////////////////////////////////////////////////
	// Get appbase instance.
	inline AppBase* GetAppBase()
	{
		//ASSERT(NULL!= AppBase::s_pAppBase);
		return AppBase::s_pAppBase;
	}
}

#endif // _YW_APPBASE_H_

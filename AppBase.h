/********************************************************************
created:	2010-11-01
filename: 	TrfAppBase.h
author:		Yaukey

purpose:	Application base class.
            This is a part of yw soft renderer.(2017-08-22)
*********************************************************************/

#ifndef _TRF_APPBASE_H_
#define _TRF_APPBASE_H_

#include "Base.h"

namespace yw
{
	//////////////////////////////////////////////////////////////////////////
	// Application base class.
	class AppBase
	{
	public:
		AppBase(HINSTANCE hInstance, LPCTSTR pCaption, int nWidth, int nHeight, bool bWindowed, D3DDEVTYPE devType, DWORD dwRequestedVP);
		virtual ~AppBase();

	public:
		//////////////////////////////////////////////////////////////////////////
		// Base method.

		// Initialize the main window.
		virtual bool InitMainWindow();

		// Initialize the d3d object and device.
		virtual bool InitDirect3D();

		// Run the application circle.
		virtual int Run();

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
		virtual void OnMouseMove(DWORD dwKeyCode, int nXpos, int nYpos);

		// Mouse left button down.
		virtual void OnLButtonDown(DWORD dwKeyCode, int nXpos, int nYpos);

		// Mouse left button up.
		virtual void OnLButtonUp(DWORD dwKeyCode, int nXpos, int nYpos);

		// Mouse left button double click.
		virtual void OnLButtonDBClick(DWORD dwKeyCode, int nXpos, int nYpos);

		// Mouse right button down.
		virtual void OnRButtonDown(DWORD dwKeyCode, int nXpos, int nYpos);

		// Mouse right button up.
		virtual void OnRButtonUp(DWORD dwKeyCode, int nXpos, int nYpos);

		// Mouse right button double click.
		virtual void OnRButtonDBClick(DWORD dwKeyCode, int nXpos, int nYpos);

		// Mouse middle button down.
		virtual void OnMButtonDwon(DWORD dwKeyCode, int nXpos, int nYpos);

		// Mouse middle button up.
		virtual void OnMButtonUp(DWORD dwKeyCode, int nXpos, int nYpos);

		// Mouse middle double click.
		virtual void OnMButtonDBClick(DWORD dwKeyCode, int nXpos, int nYpos);

		// Mouse wheel is rotated.
		virtual void OnMouseWheel(DWORD dwKeyCode, short sDelta, int nXpos, int nYpos);

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

		// Get the current direct3d object
		inline IDirect3D9* GetD3dObject()
		{
			return m_pD3dObject;
		};

		// Get the current direct3d device.
		inline IDirect3DDevice9* GetD3dDevice()
		{
			return m_pD3dDevice;
		}

	public:
		static AppBase* s_pAppBase;

	protected:
		String     m_strMainWndCaption;			// Windows caption.
		D3DDEVTYPE m_eDeviceType;				// D3d device type.
		DWORD      m_dwRequestVP;				// Vertex process type

		HINSTANCE m_hAppInst;					// Application instance.
		HWND      m_hMainWnd;					// Main window handle.
		bool      m_bAppPaused;					// Is application paused or not.

		int m_nWidth;							// Width of the client area.
		int m_nHeight;							// Height of the client area.

		IDirect3D9*           m_pD3dObject;		// The direct3d object.
		IDirect3DDevice9*     m_pD3dDevice;		// The direct3d device.
		D3DPRESENT_PARAMETERS m_cD3dPP;			// The direct3d present parameters.
	};

	//////////////////////////////////////////////////////////////////////////
	// Get trf appbase instance.
	inline AppBase* GetAppBase()
	{
		//ASSERT(NULL!= AppBase::s_pAppBase);
		return AppBase::s_pAppBase;
	}
}

#endif // _TRF_APPBASE_H_

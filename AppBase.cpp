/********************************************************************
	created:	2010-11-01
	filename: 	AppBase.cpp
	author:		Yaukey
	
	purpose:	Application base class.
	            This is a part of yw soft renderer.(2017-08-22)
*********************************************************************/

#include "stdafx.h"
#include "Resource.h"
#include "AppBase.h"

namespace yw
{
	//////////////////////////////////////////////////////////////////////////
	// We do not want user to change the window size and maximize the window,
	// we use WS_TRF_D3DAPP_FIXED_WINDOW instead of WS_OVERLAPPEDWINDOW.
	#define WS_TRF_D3DAPP_FIXED_WINDOW (WS_OVERLAPPED     | \
										WS_CAPTION        | \
										WS_SYSMENU        | \
										WS_MINIMIZEBOX)
										//WS_THICKFRAME   (no sizing border)
										//WS_MAXIMIZEBOX  (no maximize button)

	//////////////////////////////////////////////////////////////////////////
	// Windows callback function.
	LRESULT CALLBACK WndAppProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// Don't start processing messages until the application has been created.
		if (nullptr != GetAppBase())
		{
			return GetAppBase()->MsgProc(hwnd, msg, wParam, lParam);
		}
		else
		{
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Direct3d application base class.
	AppBase* AppBase::s_pAppBase = nullptr;

	AppBase::AppBase(HINSTANCE hInstance, LPCTSTR pCaption, int nWidth, int nHeight, bool bWindowed, D3DDEVTYPE devType, DWORD dwRequestedVP) :
		m_hAppInst(hInstance), 
		m_strMainWndCaption(pCaption),
		m_eDeviceType(devType),
		m_dwRequestVP(dwRequestedVP),
		m_hMainWnd(nullptr),
		m_bAppPaused(false),
		m_nWidth(nWidth),
		m_nHeight(nHeight),
		m_pD3dObject(nullptr),
		m_pD3dDevice(nullptr),
        m_pD3dSurface(nullptr)
	{
		ZeroMemory(&m_cD3dPP, sizeof(m_cD3dPP));

		// Init window information.
		if (!InitMainWindow())
		{
			PostQuitMessage(0);
		}

		// Init direct3d information.
		if (!InitDirect3D())
		{
			PostQuitMessage(0);
		}

		// Full screen or not.
		if (!bWindowed)
		{
			EnableFullScreenMode(true);
		}

		// Set itself pointer.
		s_pAppBase = this;
	}

	AppBase::~AppBase()
	{
        TRF_SAFE_RELEASE(m_pD3dSurface);
		TRF_SAFE_RELEASE(m_pD3dDevice);
		TRF_SAFE_RELEASE(m_pD3dObject);

		s_pAppBase = nullptr;
	}

	bool AppBase::InitMainWindow()
	{
		// Create the main application window.
		WNDCLASSEX wcex;

		wcex.cbSize        = sizeof(WNDCLASSEX);
		wcex.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wcex.lpfnWndProc   = WndAppProc;
		wcex.cbClsExtra    = 0;
		wcex.cbWndExtra    = 0;
		wcex.hInstance     = m_hAppInst;
		wcex.hIcon         = LoadIcon(m_hAppInst, MAKEINTRESOURCE(IDI_YWSOFTRENDERER));
		wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName  = nullptr;
		wcex.lpszClassName = _T("YW SoftRenderer Window Class");
		wcex.hIconSm       = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

		// Register class.
		if (!RegisterClassEx(&wcex))
		{
			MessageBox(nullptr, _T("RegisterClass() - FAILED"), nullptr, 0);
			return false;
		}

		// Default to a window with a client area rectangle of m_nWidth x m_nHeight.
		RECT R = {0, 0, m_nWidth, m_nHeight};
		//AdjustWindowRect(&R, /*WS_OVERLAPPEDWINDOW*/ WS_TRF_D3DAPP_FIXED_WINDOW, false);  // This will make MoveWindow have no effect when changing size if used "fixed" sytle.
		m_hMainWnd = CreateWindow(_T("YW SoftRenderer Window Class"), m_strMainWndCaption.c_str(), /*WS_OVERLAPPEDWINDOW*/ WS_TRF_D3DAPP_FIXED_WINDOW,
			(GetSystemMetrics(SM_CXSCREEN) - R.right) / 2, (GetSystemMetrics(SM_CYSCREEN) - R.bottom) / 2, 
			R.right, R.bottom, nullptr, nullptr, m_hAppInst, nullptr);

		if (nullptr == m_hMainWnd)
		{
			MessageBox(nullptr, _T("CreateWindow() - FAILED"), nullptr, 0);
			return false;
		}

        // Update client rect to m_nWidth x m_nHeight. (Windows client area is smaller.)
        RECT realRect;
        GetClientRect(m_hMainWnd, &realRect);

        int realWidth = realRect.right - realRect.left;
        int realHeight = realRect.bottom - realRect.top;
        int createWidth = m_nWidth + (m_nWidth - realWidth);        // old width - new width = gap
        int createHeight = m_nHeight + (m_nHeight - realHeight);
        MoveWindow(m_hMainWnd, GetSystemMetrics(SM_CXSCREEN) / 2 - createWidth / 2, GetSystemMetrics(SM_CYSCREEN) / 2 - createHeight / 2, createWidth, createHeight, FALSE);

        // Final update.
		ShowWindow(m_hMainWnd, SW_SHOW);
		UpdateWindow(m_hMainWnd);

		return true;
	}

	bool AppBase::InitDirect3D()
	{
		// Create d3d9 object.
		HRESULT hr = 0;

		// Step 1: Create the IDirect3D9 object.
		m_pD3dObject = Direct3DCreate9(D3D_SDK_VERSION);
		if (nullptr == m_pD3dObject)
		{
			MessageBox(nullptr, _T("Direct3DCreate9() - FAILED"), nullptr, 0);
			return false;
		}

		// Step 2: Verify hardware support for specified formats in windowed and full screen modes.
		D3DDISPLAYMODE mode;
		m_pD3dObject->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);
		HR(m_pD3dObject->CheckDeviceType(D3DADAPTER_DEFAULT, m_eDeviceType, mode.Format, mode.Format, true));
		HR(m_pD3dObject->CheckDeviceType(D3DADAPTER_DEFAULT, m_eDeviceType, D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, false));

		// Step 3: check for hardware vp.
		//D3DCAPS9 caps;
		//hr = m_pD3dObject->GetDeviceCaps(D3DADAPTER_DEFAULT, m_eDeviceType, &caps);
		//if (FAILED(hr))
		//{
		//	MessageBox(nullptr, _T("GetDeviceCaps() - FAILED"), nullptr, 0);
		//	return false;
		//}

		//int vp = 0;
		//if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		//{
		//	vp |= m_dwRequestVP/*D3DCREATE_HARDWARE_VERTEXPROCESSING*/;
		//}
		//else
		//{
		//	vp |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		//}

		// If pure device and HW T&L supported.
		//if ((caps.DevCaps & D3DDEVCAPS_PUREDEVICE) && (vp & D3DDEVCAPS_HWTRANSFORMANDLIGHT))
		//{
		//	vp |= D3DCREATE_PUREDEVICE;
		//}

		// Step 4: fill D3DPRESENT_PARAMETERS structures.
		m_cD3dPP.BackBufferWidth            = m_nWidth;// 0;
		m_cD3dPP.BackBufferHeight           = m_nHeight;// 0;
        m_cD3dPP.BackBufferFormat           = D3DFMT_X8R8G8B8;//(bpp 32 XRGB) // D3DFMT_UNKNOWN;
		m_cD3dPP.BackBufferCount            = 1;
		m_cD3dPP.MultiSampleType            = D3DMULTISAMPLE_NONE;
		m_cD3dPP.MultiSampleQuality         = 0;
		m_cD3dPP.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
		m_cD3dPP.hDeviceWindow              = m_hMainWnd;
		m_cD3dPP.Windowed                   = true;
        m_cD3dPP.EnableAutoDepthStencil     = false;// true;
        m_cD3dPP.AutoDepthStencilFormat     = D3DFMT_D16;// D3DFMT_D24S8;
		m_cD3dPP.Flags                      = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
		m_cD3dPP.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		m_cD3dPP.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

		// Step 5: create the device.
		hr = m_pD3dObject->CreateDevice(
			D3DADAPTER_DEFAULT,			// primary adapter
			m_eDeviceType,				// device type
			m_hMainWnd,					// window associated with the device
			m_dwRequestVP,				// vertex processing
			&m_cD3dPP,					// present parameters
			&m_pD3dDevice				// return created device
			);

		if (FAILED(hr))
		{
            TRF_SAFE_RELEASE(m_pD3dObject);
            MessageBox(nullptr, _T("CreateDevice() - FAILED"), nullptr, 0);

            return false;
		}

        // Step 6: get back buffer surface.
        hr = m_pD3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pD3dSurface);
        if (FAILED(hr))
        {
            TRF_SAFE_RELEASE(m_pD3dDevice);
            TRF_SAFE_RELEASE(m_pD3dObject);
            MessageBox(nullptr, _T("GetBackBuffer() - FAILED"), nullptr, 0);

            return false;
        }

		return true;
	}

	int AppBase::Run()
	{
		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));

		__int64 n64PerformanceFrequency = 0;
		QueryPerformanceFrequency((LARGE_INTEGER*)&n64PerformanceFrequency);
		float fSecondsPerCount = 1.0f / (float)n64PerformanceFrequency;

		__int64 n64PrePerformanceCounter = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&n64PrePerformanceCounter);

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				// If there are Window messages then process them.
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				// Otherwise, do animation/game stuff.

				// If the application is paused then free some CPU
				// cycles to other applications and then continue on
				// to the next frame.
				if (m_bAppPaused)
				{
					Sleep(20);
					continue;
				}

				if (!IsDeviceLost())
				{
					__int64 n64CurPerformanceCounter = 0;
					QueryPerformanceCounter((LARGE_INTEGER*)&n64CurPerformanceCounter);
					
					float fTimeDelta = (float)(n64CurPerformanceCounter - n64PrePerformanceCounter) * fSecondsPerCount;
					n64PrePerformanceCounter = n64CurPerformanceCounter;

					UpdateScene(fTimeDelta);
					DrawScene();
				}
			}
		}

		return (int)msg.wParam;
	}

	LRESULT AppBase::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// Is the application in a minimized or maximized state?
		static bool bMinOrMaxed = false;

		RECT clientRect = {0, 0, 0, 0};
		switch (msg)
		{
		// WM_ACTIVE is sent when the window is activated or deactivated.
		// We pause the game when the main window is deactivated and 
		// unpause it when it becomes active.
		case WM_ACTIVATE:
			if (WA_INACTIVE == LOWORD(wParam))
			{
				OnDeactivate();
			}
			else
			{
				OnActivate();
			}
			
			return 0;

/*  
		(Because we disabled the sizing bar, so we do not need WM_SIZE and WM_EXITSIZEMOVE to reset device.)
		// WM_SIZE is sent when the user resizes the window.
		case WM_SIZE:
			if (nullptr != m_pD3dDevice)
			{
				if (m_cD3dPP.Windowed)
				{
					m_cD3dPP.BackBufferWidth  = 0;
					m_cD3dPP.BackBufferHeight = 0;
				}
				else
				{
					m_cD3dPP.BackBufferWidth  = LOWORD(lParam);
					m_cD3dPP.BackBufferHeight = HIWORD(lParam);
				}

				if (SIZE_MINIMIZED == wParam)
				{
					m_bAppPaused = true;
					bMinOrMaxed  = true;
				}
				else if (SIZE_MAXIMIZED == wParam)
				{
					m_bAppPaused = false;
					bMinOrMaxed  = true;

					OnLostDevice();
					HR(m_pD3dDevice->Reset(&m_cD3dPP));
					OnResetDevice();
				}
				else if (SIZE_RESTORED == wParam)
				{
					// Restored is any resize that is not a minimize or maximize.
					// For example, restoring the window to its default size
					// after a minimize or maximize, or from dragging the resize
					// bars.
					m_bAppPaused = false;

					// Are we restoring from a mimimized or maximized state, 
					// and are in windowed mode?  Do not execute this code if 
					// we are restoring to full screen mode.
					if (bMinOrMaxed && m_cD3dPP.Windowed)
					{
						OnLostDevice();
						HR(m_pD3dDevice->Reset(&m_cD3dPP));
						OnResetDevice();
					}
					else
					{
						// No, which implies the user is resizing by dragging
						// the resize bars.  However, we do not reset the device
						// here because as the user continuously drags the resize
						// bars, a stream of WM_SIZE messages is sent to the window,
						// and it would be pointless (and slow) to reset for each
						// WM_SIZE message received from dragging the resize bars.
						// So instead, we reset after the user is done resizing the
						// window and releases the resize bars, which sends a
						// WM_EXITSIZEMOVE message.
					}

					bMinOrMaxed = false;
				}
			}

			return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
		case WM_EXITSIZEMOVE:
			if (m_cD3dPP.Windowed)
			{
				m_cD3dPP.BackBufferWidth  = 0;
				m_cD3dPP.BackBufferHeight = 0;
			}
			else
			{
				GetClientRect(m_hMainWnd, &clientRect);
				m_cD3dPP.BackBufferWidth  = clientRect.right;
				m_cD3dPP.BackBufferHeight = clientRect.bottom;
			}

			OnLostDevice();
			HR(m_pD3dDevice->Reset(&m_cD3dPP));
			OnResetDevice();

			return 0;
*/

		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);
				// TODO: 在此处添加使用 hdc 的任何绘图代码...
				EndPaint(hWnd, &ps);
			}

			return 0;

		// WM_CLOSE is sent when the user presses the 'X' button in the
		// caption bar menu.
		case WM_CLOSE:
			OnClose();
			return 0;

		// WM_DESTROY is sent when the window is being destroyed.
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_KEYDOWN:
			OnKeyDown((DWORD)wParam, (DWORD)lParam);
			return 0;

		case WM_KEYUP:
			OnKeyUp((DWORD)wParam, (DWORD)lParam);
			return 0;

		case WM_MOUSEMOVE:
			OnMouseMove((DWORD)wParam, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
			return 0;

		case WM_LBUTTONDOWN:
			OnLButtonDown((DWORD)wParam, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
			return 0;

		case WM_LBUTTONUP:
			OnLButtonUp((DWORD)wParam, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
			return 0;

		case WM_LBUTTONDBLCLK:
			OnLButtonDBClick((DWORD)wParam, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
			return 0;

		case WM_RBUTTONDOWN:
			OnRButtonDown((DWORD)wParam, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
			return 0;

		case WM_RBUTTONUP:
			OnRButtonUp((DWORD)wParam, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
			return 0;

		case WM_RBUTTONDBLCLK:
			OnRButtonDBClick((DWORD)wParam, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
			return 0;

		case WM_MBUTTONDOWN:
			OnMButtonDwon((DWORD)wParam, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
			return 0;

		case WM_MBUTTONUP:
			OnMButtonUp((DWORD)wParam, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
			return 0;

		case WM_MBUTTONDBLCLK:
			OnMButtonDBClick((DWORD)wParam, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
			return 0;

		case WM_MOUSEWHEEL:
			OnMouseWheel(LOWORD(wParam), (short)HIWORD(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
			return 0;
		}

		return DefWindowProc(m_hMainWnd, msg, wParam, lParam);
	}

	void AppBase::EnableFullScreenMode(bool bEnable)
	{
		if (bEnable)
		{
			// switch to fullscreen mode

			// we are already in fullscreen mode
			if (!m_cD3dPP.Windowed)
			{
				return;
			}

			int nWidth  = GetSystemMetrics(SM_CXSCREEN);
			int nHeight = GetSystemMetrics(SM_CYSCREEN);

			m_cD3dPP.BackBufferWidth  = nWidth;
			m_cD3dPP.BackBufferHeight = nHeight;
			m_cD3dPP.BackBufferFormat = D3DFMT_X8R8G8B8;		
			m_cD3dPP.Windowed         = false;

			// Change the window style to a more fullscreen friendly style.
			SetWindowLongPtr(m_hMainWnd, GWL_STYLE, WS_POPUP);

			// If we call SetWindowLongPtr, MSDN states that we need to call
			// SetWindowPos for the change to take effect.  In addition, we 
			// need to call this function anyway to update the window dimensions.
			SetWindowPos(m_hMainWnd, HWND_TOP, 0, 0, nWidth, nHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
		}
		else
		{
			// switch to window mode

			// we are already in window mode
			if (m_cD3dPP.Windowed)
			{
				return;
			}

			RECT R = {0, 0, m_nWidth, m_nHeight};
			AdjustWindowRect(&R, /*WS_OVERLAPPEDWINDOW*/ WS_TRF_D3DAPP_FIXED_WINDOW, false);

			m_cD3dPP.BackBufferWidth  = 0;
			m_cD3dPP.BackBufferHeight = 0;
			m_cD3dPP.BackBufferFormat = D3DFMT_UNKNOWN;
			m_cD3dPP.Windowed         = true;

			// Change the window style to a more windowed friendly style.
			SetWindowLongPtr(m_hMainWnd, GWL_STYLE, /*WS_OVERLAPPEDWINDOW*/ WS_TRF_D3DAPP_FIXED_WINDOW);

			// If we call SetWindowLongPtr, MSDN states that we need to call
			// SetWindowPos for the change to take effect.  In addition, we 
			// need to call this function anyway to update the window dimensions.
			SetWindowPos(m_hMainWnd, /*HWND_TOP*/ HWND_NOTOPMOST, (GetSystemMetrics(SM_CXSCREEN) - R.right) / 2, (GetSystemMetrics(SM_CYSCREEN) - R.bottom) / 2, R.right, R.bottom, /*SWP_NOZORDER |*/ SWP_SHOWWINDOW);
		}

		// Reset the device with the changes.
		OnLostDevice();
		HR(m_pD3dDevice->Reset(&m_cD3dPP));
		OnResetDevice();
	}

	bool AppBase::IsDeviceLost()
	{
		// Get the state of the graphics device.
		HRESULT hr = m_pD3dDevice->TestCooperativeLevel();

		if (D3DERR_DEVICELOST == hr)
		{
			// If the device is lost and cannot be reset yet then
			// sleep for a bit and we'll try again on the next 
			// message loop cycle.
			Sleep(20);
			return true;
		}
		else if (D3DERR_DRIVERINTERNALERROR == hr)
		{
			// Driver error, exit.
			MessageBox(nullptr, _T("Internal Driver Error...Exiting"), nullptr, 0);
			PostQuitMessage(0);

			return true;
		}
		else if (D3DERR_DEVICENOTRESET == hr)
		{
			// The device is lost but we can reset and restore it.
			OnLostDevice();
			HR(m_pD3dDevice->Reset(&m_cD3dPP));
			OnResetDevice();

			return true;
		}
		else
		{
			return false;
		}
	}

	bool AppBase::CheckDeviceType() 
	{
		return true;
	}

	void AppBase::OnLostDevice() 
	{
		
	}

	void AppBase::OnResetDevice() 
	{
		
	}

	void AppBase::UpdateScene(float fTimeDelta) 
	{
		
	}

	void AppBase::DrawScene() 
	{
		
	}

	void AppBase::OnActivate()
	{
		m_bAppPaused = false;
	}

	void AppBase::OnDeactivate()
	{
		m_bAppPaused = true;
	}

	void AppBase::OnClose()
	{
		DestroyWindow(m_hMainWnd);
	}

	void AppBase::OnKeyDown(DWORD dwKeyCode, DWORD dwExtraParam)
	{
		if (VK_ESCAPE == dwKeyCode)
		{
			EnableFullScreenMode(false);
		}
		else if ('F' == dwKeyCode)
		{
			EnableFullScreenMode(true);
		}
	}

	void AppBase::OnKeyUp(DWORD dwKeyCode, DWORD dwExtraParam)
	{
		
	}

	void AppBase::OnMouseMove(DWORD dwKeyCode, int nXpos, int nYpos)
	{
		
	}

	void AppBase::OnLButtonDown(DWORD dwKeyCode, int nXpos, int nYpos)
	{
		
	}

	void AppBase::OnLButtonUp(DWORD dwKeyCode, int nXpos, int nYpos)
	{
		
	}

	void AppBase::OnLButtonDBClick(DWORD dwKeyCode, int nXpos, int nYpos)
	{
		
	}

	void AppBase::OnRButtonDown(DWORD dwKeyCode, int nXpos, int nYpos)
	{
		
	}

	void AppBase::OnRButtonUp(DWORD dwKeyCode, int nXpos, int nYpos)
	{
		
	}

	void AppBase::OnRButtonDBClick(DWORD dwKeyCode, int nXpos, int nYpos)
	{
		
	}

	void AppBase::OnMButtonDwon(DWORD dwKeyCode, int nXpos, int nYpos)
	{
		
	}

	void AppBase::OnMButtonUp(DWORD dwKeyCode, int nXpos, int nYpos)
	{
		
	}

	void AppBase::OnMButtonDBClick(DWORD dwKeyCode, int nXpos, int nYpos)
	{
		
	}

	void AppBase::OnMouseWheel(DWORD dwKeyCode, short sDelta, int nXpos, int nYpos)
	{
		
	}
}

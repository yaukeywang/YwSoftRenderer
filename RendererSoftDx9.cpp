// Add by yaukey at 2017-08-28.
// Soft renderer implemented with dx9.

#include "stdafx.h"
#include "RendererSoftDx9.h"
#include "Utility.h"

namespace yw
{
    RendererSoftDx9::RendererSoftDx9() :
        m_DeviceType(D3DDEVTYPE_REF),
        m_RequestVP(D3DCREATE_SOFTWARE_VERTEXPROCESSING),
        m_Width(0),
        m_Height(0),
        m_WindowWidth(0),
        m_WindowHeight(0),
        m_FullScreenWidth(0),
        m_FullScreenHeight(0),
        m_D3dObject(nullptr),
        m_D3dDevice(nullptr),
        m_D3dSurface(nullptr)
    {
        ZeroMemory(&m_D3dPP, sizeof(m_D3dPP));
    }

    RendererSoftDx9::~RendererSoftDx9()
    {
        YW_SAFE_RELEASE(m_D3dSurface);
        YW_SAFE_RELEASE(m_D3dDevice);
        YW_SAFE_RELEASE(m_D3dObject);
    }

    bool RendererSoftDx9::Initialize(LPCTSTR name, HWND hWnd, int32_t width, int32_t height, bool windowed)
    {
        LOGI(_T("Initializing soft renderer with DirectX 9..."));

        // Init name.
        if ((nullptr == name) || (String(name).length() == 0))
        {
            m_Name = _T("DirectX 9 Soft Renderer");
        }
        else
        {
            m_Name = name;
        }

        // Init handler.
        m_hMainWnd = hWnd;

        // Init client area size.
        m_Width = width;
        m_Height = height;

        // Get the actual window create size.
        RECT windowRect;
        GetWindowRect(m_hMainWnd, &windowRect);
        m_WindowWidth = windowRect.right - windowRect.left;        // old width - new width = gap
        m_WindowHeight = windowRect.bottom - windowRect.top;

        // Create d3d9 object.
        HRESULT hr = 0;

        // Step 1: Create the IDirect3D9 object.
        m_D3dObject = Direct3DCreate9(D3D_SDK_VERSION);
        if (nullptr == m_D3dObject)
        {
            MessageBox(nullptr, _T("RendererSoftDx9::InitRenderer::Direct3DCreate9() - FAILED"), nullptr, 0);
            LOGE(_T("Initializing  soft renderer with DirectX 9 failed: RendererSoftDx9::InitRenderer::Direct3DCreate9() - FAILED!"));

            return false;
        }

        // Step 2: Verify hardware support for specified formats in windowed and full screen modes.
        D3DDISPLAYMODE mode;
        m_D3dObject->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);
        HR(m_D3dObject->CheckDeviceType(D3DADAPTER_DEFAULT, m_DeviceType, mode.Format, mode.Format, true));
        HR(m_D3dObject->CheckDeviceType(D3DADAPTER_DEFAULT, m_DeviceType, D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, false));

        // Step 3: check for hardware vp.
        //D3DCAPS9 caps;
        //hr = m_D3dObject->GetDeviceCaps(D3DADAPTER_DEFAULT, m_DeviceType, &caps);
        //if (FAILED(hr))
        //{
        //	MessageBox(nullptr, _T("RendererSoftDx9::InitRenderer::GetDeviceCaps() - FAILED"), nullptr, 0);
        //	return false;
        //}

        //int32_t vp = 0;
        //if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
        //{
        //	vp |= m_RequestVP/*D3DCREATE_HARDWARE_VERTEXPROCESSING*/;
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
        m_D3dPP.BackBufferWidth = m_Width;// 0;
        m_D3dPP.BackBufferHeight = m_Height;// 0;
        m_D3dPP.BackBufferFormat = D3DFMT_X8R8G8B8;//(bpp 32 XRGB) // D3DFMT_UNKNOWN;
        m_D3dPP.BackBufferCount = 1;
        m_D3dPP.MultiSampleType = D3DMULTISAMPLE_NONE;
        m_D3dPP.MultiSampleQuality = 0;
        m_D3dPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
        m_D3dPP.hDeviceWindow = hWnd;
        m_D3dPP.Windowed = windowed;
        m_D3dPP.EnableAutoDepthStencil = false;// true;
        m_D3dPP.AutoDepthStencilFormat = D3DFMT_D16;// D3DFMT_D24S8;
        m_D3dPP.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
        m_D3dPP.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
        m_D3dPP.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

        // Step 5: create the device.
        hr = m_D3dObject->CreateDevice(
            D3DADAPTER_DEFAULT,         // primary adapter
            m_DeviceType,				// device type
            hWnd,                       // window associated with the device
            m_RequestVP,                // vertex processing
            &m_D3dPP,                   // present parameters
            &m_D3dDevice                // return created device
        );

        if (FAILED(hr))
        {
            YW_SAFE_RELEASE(m_D3dObject);
            MessageBox(nullptr, _T("RendererSoftDx9::InitRenderer::CreateDevice() - FAILED"), nullptr, 0);
            LOGE(_T("Initializing  soft renderer with DirectX 9 failed: RendererSoftDx9::InitRenderer::CreateDevice() - FAILED!"));

            return false;
        }

        // Step 6: get back buffer surface.
        hr = m_D3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_D3dSurface);
        if (FAILED(hr))
        {
            YW_SAFE_RELEASE(m_D3dDevice);
            YW_SAFE_RELEASE(m_D3dObject);
            MessageBox(nullptr, _T("RendererSoftDx9::InitRenderer::GetBackBuffer() - FAILED"), nullptr, 0);
            LOGE(_T("Initializing  soft renderer with DirectX 9 failed: RendererSoftDx9::InitRenderer::GetBackBuffer() - FAILED!"));

            return false;
        }

        LOGI(_T("Initializing soft renderer with DirectX 9 finished."));

        return true;
    }

    bool RendererSoftDx9::Release()
    {
        YW_SAFE_RELEASE(m_D3dSurface);
        YW_SAFE_RELEASE(m_D3dDevice);
        YW_SAFE_RELEASE(m_D3dObject);

        return true;
    }

    LPCTSTR RendererSoftDx9::GetName() const
    {
        return m_Name.c_str();
    }

    bool RendererSoftDx9::Resize(int32_t width, int32_t height)
    {
        m_Width = width;
        m_Height = height;

        m_D3dPP.BackBufferWidth = m_Width;
        m_D3dPP.BackBufferHeight = m_Height;

        OnLostDevice();
        ResetDevice();
        OnResetDevice();

        return true;
    }

    int32_t RendererSoftDx9::GetWidth() const
    {
        return m_Width;
    }

    int32_t RendererSoftDx9::GetHeight() const
    {
        return m_Height;
    }

    void* RendererSoftDx9::GetBuffer()
    {
        return nullptr;
    }

    void RendererSoftDx9::EnableFullScreenMode(bool bEnable)
    {
        if (bEnable)
        {
            // switch to fullscreen mode

            // we are already in fullscreen mode
            if (!m_D3dPP.Windowed)
            {
                return;
            }

            m_FullScreenWidth = GetSystemMetrics(SM_CXSCREEN);
            m_FullScreenHeight = GetSystemMetrics(SM_CYSCREEN);

            m_D3dPP.BackBufferWidth = m_FullScreenWidth;
            m_D3dPP.BackBufferHeight = m_FullScreenHeight;
            //m_D3dPP.BackBufferFormat = D3DFMT_X8R8G8B8;
            m_D3dPP.Windowed = false;

            // Change the window style to a more fullscreen friendly style.
            SetWindowLongPtr(m_hMainWnd, GWL_STYLE, WS_POPUP);

            // If we call SetWindowLongPtr, MSDN states that we need to call
            // SetWindowPos for the change to take effect.  In addition, we 
            // need to call this function anyway to update the window dimensions.
            SetWindowPos(m_hMainWnd, HWND_TOP, 0, 0, m_FullScreenWidth, m_FullScreenHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
        }
        else
        {
            // switch to window mode

            // we are already in window mode
            if (m_D3dPP.Windowed)
            {
                return;
            }

            RECT R = { 0, 0, m_WindowWidth, m_WindowHeight };
            //AdjustWindowRect(&R, /*WS_OVERLAPPEDWINDOW*/ WS_YW_RENDERER_FIXED_WINDOW, false); // This will lead a wrong window client area size when changing size if used "fixed" sytle.

            m_D3dPP.BackBufferWidth = m_Width;
            m_D3dPP.BackBufferHeight = m_Height;
            //m_D3dPP.BackBufferFormat = D3DFMT_X8R8G8B8;
            m_D3dPP.Windowed = true;

            // Change the window style to a more windowed friendly style.
            SetWindowLongPtr(m_hMainWnd, GWL_STYLE, /*WS_OVERLAPPEDWINDOW*/ WS_YW_RENDERER_FIXED_WINDOW);

            // If we call SetWindowLongPtr, MSDN states that we need to call
            // SetWindowPos for the change to take effect.  In addition, we 
            // need to call this function anyway to update the window dimensions.
            SetWindowPos(m_hMainWnd, /*HWND_TOP*/ HWND_NOTOPMOST, (GetSystemMetrics(SM_CXSCREEN) - R.right) / 2, (GetSystemMetrics(SM_CYSCREEN) - R.bottom) / 2, R.right, R.bottom, /*SWP_NOZORDER |*/ SWP_SHOWWINDOW);
        }

        // Reset the device with the changes.
        OnLostDevice();
        ResetDevice();
        OnResetDevice();
    }

    bool RendererSoftDx9::IsDeviceLost()
    {
        // Get the state of the graphics device.
        HRESULT hr = m_D3dDevice->TestCooperativeLevel();

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
            MessageBox(nullptr, _T("RendererSoftDx9::IsDeviceLost: Internal Driver Error...Exiting!"), nullptr, 0);
            PostQuitMessage(0);

            return true;
        }
        else if (D3DERR_DEVICENOTRESET == hr)
        {
            // The device is lost but we can reset and restore it.
            OnLostDevice();
            ResetDevice();
            OnResetDevice();

            return true;
        }
        else
        {
            return false;
        }
    }

    bool RendererSoftDx9::CheckDeviceType()
    {
        return true;
    }

    void RendererSoftDx9::OnLostDevice()
    {

    }

    void RendererSoftDx9::OnResetDevice()
    {

    }

    void RendererSoftDx9::ResetDevice()
    {
        // Release old surface first.
        YW_SAFE_RELEASE(m_D3dSurface);

        // Reset device.
        HR(m_D3dDevice->Reset(&m_D3dPP));

        // Get the new surface agein.
        m_D3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_D3dSurface);
    }

    void RendererSoftDx9::Clear()
    {

    }

    bool RendererSoftDx9::BeginScene()
    {
        return true;
    }

    bool RendererSoftDx9::EndScene()
    {
        D3DLOCKED_RECT lockedRect;
        memset(&lockedRect, 0, sizeof(lockedRect));
        m_D3dSurface->LockRect(&lockedRect, nullptr, D3DLOCK_DISCARD);

        DWORD * imagedata = (DWORD*)lockedRect.pBits;

        int32_t width = m_D3dPP.Windowed ? m_Width : m_FullScreenWidth;
        int32_t height = m_D3dPP.Windowed ? m_Height : m_FullScreenHeight;
        for (int32_t i = 0; i < height; i++)
        {
            for (int32_t j = 0; j < width; j++)
            {
                // index into texture, note we use the pitch and divide by  
                // four since the pitch is given in bytes and there are  
                // 4 bytes per dword.  

                int32_t index = i * lockedRect.Pitch / 4 + j;
                imagedata[index] = 0xff06704A; // green like
            }
        }

        m_D3dSurface->UnlockRect();

        //HR(m_pD3dDevice->EndScene());
        HR(m_D3dDevice->Present(NULL, NULL, NULL, NULL));

        return true;
    }

    bool RendererSoftDx9::DrawLine(float from, float to)
    {
        return true;
    }
}

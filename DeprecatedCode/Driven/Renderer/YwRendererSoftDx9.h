// Add by yaukey at 2017-08-28.
// Soft renderer implemented with dx9.

#ifndef __YW_RENDERER_SOFT_DX9_H__
#define __YW_RENDERER_SOFT_DX9_H__

#include "Driven/Renderer/YwRenderer.h"
#include "Driven/Math/YwMathCore.h"
#include <d3d9.h>

namespace yw
{
    class RendererSoftDx9 : public IRenderer
    {
    public:
        RendererSoftDx9();
        virtual ~RendererSoftDx9();

    public:
        virtual bool Initialize(LPCTSTR name, HWND hWnd, int32_t width, int32_t height, bool windowed);
        virtual bool Release();

        virtual LPCTSTR GetName() const;
        virtual bool Resize(int32_t width, int32_t height);
        virtual int32_t GetWidth() const;
        virtual int32_t GetHeight() const;
        virtual void* GetBuffer();

        virtual void EnableFullScreenMode(bool bEnable);
        virtual bool IsDeviceLost();
        virtual bool CheckDeviceType();
        virtual void OnLostDevice();
        virtual void OnResetDevice();
        virtual void ResetDevice();

        virtual void Clear();
        virtual bool BeginScene();
        virtual bool EndScene();
        virtual bool DrawLine(float from, float to);
        virtual bool DrawLine(void* buffer, uint32_t pitch, uint32_t color, Vector2 from, Vector2 to);

    public:
        // Get the main window handle.
        inline HWND GetMainWnd() const
        {
            return m_hMainWnd;
        }

        // Get the current direct3d object
        inline IDirect3D9* GetD3dObject()
        {
            return m_D3dObject;
        };

        // Get the current direct3d device.
        inline IDirect3DDevice9* GetD3dDevice()
        {
            return m_D3dDevice;
        }

        // Get the current direct3d surface.
        inline IDirect3DSurface9* GetD3dSurface()
        {
            return m_D3dSurface;
        }

    private:
        // The name of this renderer.
        String m_Name;

        // Main window handle.
        HWND m_hMainWnd;

        // Width of the client area.
        int32_t m_Width;

        // Height of the client area.
        int32_t m_Height;

        // Width of the created window area.(Greater than client area width)
        int32_t m_WindowWidth;

        // Height of the created window area.(Greater than client area height)
        int32_t m_WindowHeight;

        // Width of full screen.
        int32_t m_FullScreenWidth;

        // Height of full screen.
        int32_t m_FullScreenHeight;

        // D3d device type.
        D3DDEVTYPE m_DeviceType;

        // Vertex process type
        uint32_t m_RequestVP;

        // The direct3d object.
        IDirect3D9* m_D3dObject;

        // The direct3d device.
        IDirect3DDevice9* m_D3dDevice;

        // The direct3d surface for backbuffer.
        IDirect3DSurface9* m_D3dSurface;

        // The direct3d present parameters.
        D3DPRESENT_PARAMETERS m_D3dPP;
    };
}

#endif // !__YW_RENDERER_SOFT_DX9_H__


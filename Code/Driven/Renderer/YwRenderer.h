// Add by yaukey at 2017-08-26.
// Base renderer class.

#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "System/Support/YwBase.h"

namespace yw
{
    class IRenderer
    {
    public:
        enum RendererType
        {
            INVALID,
            RT_SOFT_D3D9,
            END
        };

    public:
        IRenderer() {}
        virtual ~IRenderer() {}

    public:
        virtual bool Initialize(LPCTSTR name, HWND hWnd, int width, int height, bool windowed) = 0;
        virtual bool Release() = 0;

        virtual LPCTSTR GetName() const = 0;
        virtual bool Resize(int32_t width, int32_t height) = 0;
        virtual int32_t GetWidth() const = 0;
        virtual int32_t GetHeight() const = 0;
        virtual void* GetBuffer() = 0;

        virtual void EnableFullScreenMode(bool bEnable) = 0;
        virtual bool IsDeviceLost() = 0;
        virtual bool CheckDeviceType() = 0;
        virtual void OnLostDevice() = 0;
        virtual void OnResetDevice() = 0;
        virtual void ResetDevice() = 0;

        virtual void Clear() = 0;
        virtual bool BeginScene() = 0;
        virtual bool EndScene() = 0;
        virtual bool DrawLine(float from, float to) = 0;
    };
}

#endif // __RENDERER_H__

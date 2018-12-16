// Add by Yaukey at 2018-01-31.
// YW Soft Renderer render target class.

#ifndef __YW_3D_RENDER_TARGET_H__
#define __YW_3D_RENDER_TARGET_H__

#include "../Yw3dBase.h"
#include "../Yw3dTypes.h"

namespace yw
{
    // This class defines a Yw3d RenderTarget.
    class Yw3dRenderTarget : public IBase
    {
        friend class Yw3dDevice;

    protected:
        // Accessible by Yw3dDevice which is the only class that may create a render target.
        // @param[in] device a pointer to the parent Yw3dDevice-object.
        Yw3dRenderTarget(class Yw3dDevice* device);

        // Accessible by IBase. The destructor is called when the reference count reaches zero.
        ~Yw3dRenderTarget();

    public:
        // Returns a pointer to the associated device. Calling this function will increase the internal reference count of the device. 
        // Failure to call Release() when finished using the pointer will result in a memory leak.
        class Yw3dDevice* AcquireDevice();

        // Clears the colorbuffer, which is associated with this rendertarget, to a given color.
        // @param[in] color color to clear the colorbuffer to.
        // @param[in] rect rectangle to restrict clearing to.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidState if no colorbuffer has been set.
        // @return Yw3d_E_InvalidParameters if the clear-rectangle exceeds the colorbuffer's dimensions.
        Yw3dResult ClearColorBuffer(const Vector4& color, const Yw3dRect* rect);

        // Clears the depthbuffer, which is associated with this rendertarget, to a given depth-value.
        // @param[in] depth depth to clear the depthbuffer to.
        // @param[in] rect rectangle to restrict clearing to.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidState if no depthbuffer has been set.
        // @return Yw3d_E_InvalidParameters if the clear-rectangle exceeds the depthbuffer's dimensions.
        Yw3dResult ClearDepthBuffer(const float depth, const Yw3dRect* rect);

        // Clears the stencilbuffer, which is associated with this rendertarget, to a given depth-value.
        // @param[in] depth depth to clear the stencilbuffer to.
        // @param[in] rect rectangle to restrict clearing to.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidState if no stencilbuffer has been set.
        // @return Yw3d_E_InvalidParameters if the clear-rectangle exceeds the stencilbuffer's dimensions.
        Yw3dResult ClearStencilBuffer(const float stencil, const Yw3dRect* rect);

        // Associates a Yw3dSurface as colorbuffer with this rendertarget, releasing the currently set colorbuffer.
        // Calling this function will increase the internal reference count of the surface.
        // @param[in] colorBuffer new colorbuffer.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if an invalid format was encountered.
        Yw3dResult SetColorBuffer(class Yw3dSurface* colorBuffer);

        // Associates a CMuli3DSurface as depthbuffer with this rendertarget, releasing the currently set depthbuffer.
        // Calling this function will increase the internal reference count of the surface.
        // @param[in] depthBuffer new depthbuffer.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if an invalid format was encountered.
        Yw3dResult SetDepthBuffer(class Yw3dSurface* depthBuffer);

        // Associates a CMuli3DSurface as stencilbuffer with this rendertarget, releasing the currently set stencilbuffer.
        // Calling this function will increase the internal reference count of the surface.
        // @param[in] stencilbuffer new stencilbuffer.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if an invalid format was encountered.
        Yw3dResult SetStencilBuffer(class Yw3dSurface* stencilBuffer);

        // Returns a pointer to the rendertarget's colorbuffer. Calling this function will increase the internal reference count of the texture. Failure to call Release() when finished using the pointer will result in a memory leak.
        class Yw3dSurface* AcquireColorBuffer();

        // Returns a pointer to the rendertarget's depthbuffer. Calling this function will increase the internal reference count of the texture. Failure to call Release() when finished using the pointer will result in a memory leak.
        class Yw3dSurface* AcquireDepthBuffer();

        // Returns a pointer to the rendertarget's stencilbuffer. Calling this function will increase the internal reference count of the texture. Failure to call Release() when finished using the pointer will result in a memory leak.
        class Yw3dSurface* AcquireStencilBuffer();

        // Returns the rendertarget's viewport matrix.
        const Matrix44* GetViewportMatrix() const;

    private:
        // Pointer to parent.
        class Yw3dDevice* m_Device;

        // Pointer to the colorbuffer.
        class Yw3dSurface* m_ColorBuffer;

        // Pointer to the depthbuffer.
        class Yw3dSurface* m_DepthBuffer;

        // Pointer to the stencilbuffer.
        class Yw3dSurface* m_StencilBuffer;
    };
}

#endif // !__YW_3D_RENDER_TARGET_H__


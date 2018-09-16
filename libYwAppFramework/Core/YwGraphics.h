// Add by Yaukey at 2018-07-11.
// YW Soft Renderer 3d device graphics class.

#ifndef __YW_GRAPHICS_H__
#define __YW_GRAPHICS_H__

#include <stack>
#include "Yw3d.h"
#include "YwStateBlock.h"

namespace yw
{
    class Graphics
    {
        friend class IApplication;
        friend class Application;
        friend class StateBlock;
        friend class Camera;

    protected:
        // Constructor.
        Graphics(class IApplication* application);

        // Destructor.
        ~Graphics();

    protected:
        bool Initialize(const struct ApplicationCreationFlags& creationFlags);

    public:
        // Get application.
        inline IApplication* GetApplication() const
        {
            return m_Application;
        }

        // Get object.
        inline Yw3d* GetYw3d() const
        {
            return m_Yw3d;
        }

        // Get device.
        inline Yw3dDevice* GetYw3dDevice() const
        {
            return m_Yw3dDevice;
        }

        // Get current camera.
        inline class Camera* GetCurrentCamera() const
        {
            return m_Camera;
        }

        // Push current render state into stack.
        void PushStateBlock();

        // Pop current render state from stack.
        void PopStateBlock();

        // Set render states.
        Yw3dResult SetRenderState(Yw3dRenderState renderState, uint32_t value);

        // Set vertex format.
        Yw3dResult SetVertexFormat(Yw3dVertexFormat* vertexFormat);

        // Set primitive assembler.
        void SetPrimitiveAssembler(IYw3dPrimitiveAssembler* primitiveAssembler);

        // Set vertex shader.
        Yw3dResult SetVertexShader(IYw3dVertexShader* vertexShader);

        // Set triangle shader.
        void SetTriangleShader(IYw3dTriangleShader* triangleShader);

        // Set pixel shader.
        Yw3dResult SetPixelShader(IYw3dPixelShader* pixelShader);

        // Set index buffer.
        Yw3dResult SetIndexBuffer(Yw3dIndexBuffer* indexBuffer);

        // Set vertex stream.
        Yw3dResult SetVertexStream(uint32_t streamNumber, Yw3dVertexBuffer* vertexBuffer, uint32_t offset, uint32_t stride);

        // Set texture of a sampler number.
        Yw3dResult SetTexture(uint32_t samplerNumber, IYw3dBaseTexture* texture);

        // Set texture sampler state of a sampler number.
        Yw3dResult SetTextureSamplerState(uint32_t samplerNumber, Yw3dTextureSamplerState textureSamplerState, uint32_t state);

        // Set render target.
        void SetRenderTarget(Yw3dRenderTarget* renderTarget);

        // Set scissor rect.
        Yw3dResult SetScissorRect(const Yw3dRect& scissorRect);

    protected:
        // Set current camera.
        inline void SetCurrentCamera(class Camera* camera)
        {
            if (m_StateBlocks.empty())
            {
                return;
            }

            m_StateBlocks.top()->SetCurrentCamera(camera);
        }

    protected:
        // Camera instance.
        class Camera* m_Camera;

    private:
        // Yw3d instance.
        Yw3d* m_Yw3d;

        // Yw3d device instance.
        Yw3dDevice* m_Yw3dDevice;

        // Base application class.
        class IApplication* m_Application;

        // All state block stack.
        std::stack<StateBlock*> m_StateBlocks;
    };
}

#endif // !__YW_GRAPHICS_H__

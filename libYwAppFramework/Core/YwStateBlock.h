// Add by Yaukey at 2018-07-10.
// YW Soft Renderer 3d device render state control class.

#ifndef __YW_STATE_BLOCK_H__
#define __YW_STATE_BLOCK_H__

#include <map>
#include "Yw3d.h"

namespace yw
{
    // The state block class, better control of graphics states.
    class StateBlock
    {
        friend class Graphics;

    protected:
        // Constructor.
        StateBlock(class Graphics* graphics);

    public:
        // Destructor.
        ~StateBlock();

    public:
        // Initialize all the states.
        void InitializeStates(class Graphics* graphics);

        // Restore all the states.
        void RestoreStates();

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

        // Set current camera.
        void SetCurrentCamera(class Camera* camera);

    public:
        // Get parent graphics.
        inline class Graphics* GetGraphics() const
        {
            return m_Graphics;
        }

    private:
        // The parent graphics.
        class Graphics* m_Graphics;

        // All render states.
        std::map<Yw3dRenderState, uint32_t> m_RenderStates;

        // Backup variables.

        bool m_ChangedVertexFormat;
        Yw3dVertexFormat* m_VertexFormat;

        bool m_ChangedPrimitiveAssembler;
        IYw3dPrimitiveAssembler* m_PrimitiveAssembler;

        bool m_ChangedVertexShader;
        IYw3dVertexShader* m_VertexShader;

        bool m_ChangedTriangleShader;
        IYw3dTriangleShader* m_TriangleShader;

        bool m_ChangedPixelShader;
        IYw3dPixelShader* m_PixelShader;

        bool m_ChangedIndexBuffer;
        Yw3dIndexBuffer* m_IndexBuffer;

        // Internal Vertex Stream Info.
        struct VertexStreamInfo
        {
            Yw3dVertexBuffer* vertexBuffer;
            uint32_t stride;
            uint32_t offset;

            VertexStreamInfo() : vertexBuffer(nullptr), stride(0), offset(0) {}
        };

        std::map<uint32_t, VertexStreamInfo> m_VertexStreams;
        std::map<uint32_t, IYw3dBaseTexture*> m_Textures;
        std::map<uint32_t, uint32_t> m_TextureSamplerStates;

        bool m_ChangedRenderTarget;
        Yw3dRenderTarget* m_RenderTarget;

        bool m_ChangedScissorRect;
        Yw3dRect m_ScissorRect;

        bool m_ChangedCamera;
        class Camera* m_Camera;
    };
}

#endif // !__YW_STATE_BLOCK_H__

// Add by Yaukey at 2018-01-30.
// YW Soft Renderer 3d device class.

#include "Yw3dCore.h"
#include "Yw3dDevice.h"
#include "Yw3dPresentTarget.h"

namespace yw
{
    Yw3dDevice::Yw3dDevice(Yw3d* yw3d, const Yw3dDeviceParameters* deviceParameters) :
        m_Parent(yw3d),
        m_PresentTarget(nullptr),
        m_VertexFormat(nullptr),
        m_PrimitiveAssembler(nullptr),
        m_VertexShader(nullptr),
        m_TriangleShader(nullptr),
        m_PixelShader(nullptr),
        m_IndexBuffer(nullptr),
        m_RenderTarget(nullptr),
        m_NumValidCacheEntries(0),
        m_FetchedVertices(0),
        m_NextFreeClipVertex(0)
    {
        m_Parent->AddRef();

        // Copy device parameters.
        memcpy(&m_DeviceParameters, deviceParameters, sizeof(Yw3dDeviceParameters));

        // Initialize important internal variables.
        memset(m_RenderStates, 0, Yw3d_RS_NumRenderStates * sizeof(uint32_t));
        memset(m_ClipVertices, 0, YW3D_CLIP_VERTEX_CACHE_SIZE * sizeof(Yw3dVSOutput));
        memset(m_ClipVerticesStages[0], 0, YW3D_CLIP_VERTEX_CACHE_SIZE * sizeof(Yw3dVSOutput*));
        memset(m_ClipVerticesStages[1], 0, YW3D_CLIP_VERTEX_CACHE_SIZE * sizeof(Yw3dVSOutput*));

        // Set some default values.
        SetDefaultRenderStates();
        SetDefaultTextureSamplerStates();
        SetDefaultClippingPlanes();
        SetDefaultRenderTarget();
    }

    Yw3dDevice::~Yw3dDevice()
    {
        YW_SAFE_RELEASE(m_PresentTarget);
        YW_SAFE_RELEASE(m_Parent);
    }

    Yw3dResult Yw3dDevice::Create()
    {
        // Create the present-target.

        // NOTE: add support for other platforms here.
#if defined(_WIN32) || defined(WIN32)
        m_PresentTarget = new Yw3dPresentTargetWindows(this);
#endif

#ifdef LINUX_X11
        m_PresentTarget = new Yw3dPresentTargetLinuxX11(this);
#endif

#ifdef __amigaos4__
        m_PresentTarget = new Yw3dPresentTargetAmigaOS4(this);
#endif

        if (nullptr == m_PresentTarget)
        {
            LOGE(_T("Yw3dDevice::Create: out of memory, cannot create present-target.\n"));
            return Yw3d_E_OutOfMemory;
        }

        return m_PresentTarget->Create();
    }

    Yw3d* Yw3dDevice::AcquireYw3d()
    {
        if (nullptr != m_Parent)
        {
            m_Parent->AddRef();
        }

        return m_Parent;
    }

    const Yw3dDeviceParameters& Yw3dDevice::GetDeviceParameters() const
    {
        return m_DeviceParameters;
    }

    Yw3dResult Yw3dDevice::Clear(const Yw3dRect* rect, const Vector4& color, float depth, uint32_t stencil)
    {
        m_RenderTarget->ClearColorBuffer(color, rect);
        m_RenderTarget->ClearDepthBuffer(depth, rect);
        // Clear stencil buffer.

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::Present()
    {
        Yw3dRenderTarget* renderTarget = m_RenderTarget;
        if (nullptr == renderTarget)
        {
           LOGE(_T("Yw3dDevice::Present: parameter renderTarget points to null.\n"));
           return Yw3d_E_InvalidParameters;
        }

        // Get pointer to the colorbuffer of the rendertarget.
        Yw3dSurface* colorBuffer = renderTarget->AcquireColorBuffer();
        if (nullptr == colorBuffer)
        {
           LOGE(_T("Yw3dDevice::Present: rendertarget doesn't have a colorbuffer attached.\n"));
           return  Yw3d_E_InvalidState;
        }

        if ((colorBuffer->GetWidth() != m_DeviceParameters.backBufferWidth) || (colorBuffer->GetHeight() != m_DeviceParameters.backBufferHeight))
        {
           YW_SAFE_RELEASE(colorBuffer);
           LOGE(_T("Yw3dDevice::Present: colorbuffer's dimensions don't match backbuffer.\n"));
           return Yw3d_E_InvalidState;
        }

        const uint32_t floatCount = colorBuffer->GetFormatFloats();
        if (floatCount < 3)
        {
           YW_SAFE_RELEASE(colorBuffer);
           LOGE(_T("Yw3dDevice::Present: invalid colorbuffer format - only Yw3d_FMT_R32G32B32F and Yw3d_FMT_R32G32B32A32F are supported!\n"));
           return  Yw3d_E_InvalidFormat;
        }

        const float* sourceBufData = nullptr;
        if (YW3D_FAILED(colorBuffer->LockRect((void**)&sourceBufData, 0)))
        {
           YW_SAFE_RELEASE(colorBuffer);
           LOGE(_T("Yw3dDevice::Present: couldn't access colorbuffer.\n"));
           return Yw3d_E_Unknown;
        }

        Yw3dResult resPresent = m_PresentTarget->Present(sourceBufData, floatCount);

        colorBuffer->UnlockRect();
        YW_SAFE_RELEASE(colorBuffer);

        return resPresent;
    }

    Yw3dResult Yw3dDevice::DrawPrimitive(Yw3dPrimitiveType primitiveType, uint32_t startVertex, uint32_t primitiveCount)
    {
        if (primitiveCount <= 0)
        {
            LOGE(_T("Yw3dDevice::DrawPrimitive: primitive count is 0.\n"));
            return Yw3d_E_InvalidParameters;
        }

        // Process incoming vertices.
        uint32_t numVertices = 0;
        switch (primitiveType)
        {
        case Yw3d_PT_TriangleFan:
            numVertices = primitiveCount + 2;
            break;
        case Yw3d_PT_TriangleStrip:
            numVertices = primitiveCount + 2;
            break;
        case Yw3d_PT_TriangleList:
            numVertices = primitiveCount * 3;
            break;
        default:
            LOGE(_T("Yw3dDevice::DrawPrimitive: invalid primitive type specified.\n"));
            break;
        }

        Yw3dResult resCheck = PreRender();
        if (YW3D_FAILED(resCheck))
        {
            return resCheck;
        }

        uint32_t vertexIndicies[3] = { startVertex, startVertex + 1, startVertex + 2 };
        bool flip4TriStrip = false; // used when drawing triangle strips.
        while (primitiveCount-- > 0)
        {
            Yw3dVertexCacheEntry* vertices[3] = {nullptr, nullptr, nullptr};
            for (uint32_t i = 0; i < 3; i++)
            {
                // Fetch the 3 vertices of this primitive.
                Yw3dResult resFetch = FetchVertex(&vertices[i], vertexIndicies[i]);
                if (YW3D_FAILED(resFetch))
                {
                    LOGE(_T("Yw3dDevice::DrawPrimitive: couldn't fetch vertex from streams.\n"));
                    PostRender();

                    return resFetch;
                }
            }

            // Process this triangle.
            if (flip4TriStrip)
            {
                ProcessTriangle(&vertices[0]->vertexOutput, &vertices[2]->vertexOutput, &vertices[1]->vertexOutput);
            }
            else
            {
                ProcessTriangle(&vertices[0]->vertexOutput, &vertices[1]->vertexOutput, &vertices[2]->vertexOutput);
            }

            // Prepare vertex-indices for the next triangle.
            switch (primitiveType)
            {
            case Yw3d_PT_TriangleFan:
                vertexIndicies[1] = vertexIndicies[2];
                vertexIndicies[2]++;
                break;
            case Yw3d_PT_TriangleStrip:
                flip4TriStrip = !flip4TriStrip;
                vertexIndicies[0] = vertexIndicies[1];
                vertexIndicies[1] = vertexIndicies[2];
                vertexIndicies[2]++;
                break;
            case Yw3d_PT_TriangleList:
                vertexIndicies[0] += 3;
                vertexIndicies[1] += 3;
                vertexIndicies[2] += 3;
                break;
            default:
                // cannot happen.
                break;
            }
        }

        // Process post render state reset.
        PostRender();

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::DrawIndexedPrimitive(Yw3dPrimitiveType primitiveType, uint32_t baseVertexIndex, uint32_t minIndex, uint32_t numVertices, uint32_t startIndex, uint32_t primitiveCount)
    {
        if (primitiveCount <= 0)
        {
            LOGE(_T("Yw3dDevice::DrawPrimitive: primitive count is 0.\n"));
            return Yw3d_E_InvalidParameters;
        }

        if (numVertices <= 0)
        {
            LOGE(_T("Yw3dDevice::DrawIndexedPrimitive: number of vertices is 0.\n"));
            return Yw3d_E_InvalidParameters;
        }

        if ((Yw3d_PT_TriangleFan != primitiveType) && (Yw3d_PT_TriangleStrip != primitiveType) && (Yw3d_PT_TriangleList != primitiveType))
        {
            LOGE(_T("Yw3dDevice::DrawIndexedPrimitive: invalid primitive type specified.\n"));
            return Yw3d_E_InvalidParameters;
        }

        if (nullptr == m_IndexBuffer)
        {
            LOGE(_T("Yw3dDevice::DrawIndexedPrimitive: no indexbuffer has been set!\n"));
            return Yw3d_E_InvalidState;
        }

        Yw3dResult resCheck = PreRender();
        if (YW3D_FAILED(resCheck))
        {
            return resCheck;
        }

        uint32_t vertexIndicies[3] = { startIndex, startIndex + 1, startIndex + 2 };
        bool flip4TriStrip = false; // used when drawing triangle strips.
        while (primitiveCount-- > 0)
        {
            Yw3dVertexCacheEntry* vertices[3] = {nullptr, nullptr, nullptr};
            for (uint32_t i = 0; i < 3; i++)
            {
                uint32_t vertexIndex = 0;
                Yw3dResult resGetVertexIndex = m_IndexBuffer->GetVertexIndex(vertexIndicies[i], vertexIndex);
                if (YW3D_FAILED(resGetVertexIndex))
                {
                    LOGE(_T("Yw3dDevice::DrawIndexedPrimitive: couldn't read vertex index from indexbuffer.\n"));
				    PostRender();

				    return resGetVertexIndex;
                }

                // Fetch the 3 vertices of this primitive.
                Yw3dResult resFetch = FetchVertex(&vertices[i], baseVertexIndex + vertexIndex);
                if (YW3D_FAILED(resFetch))
                {
                    LOGE(_T("Yw3dDevice::DrawIndexedPrimitive: couldn't fetch vertex from streams.\n"));
                    PostRender();

                    return resFetch;
                }
            }

            // Process this triangle.
            if (flip4TriStrip)
            {
                ProcessTriangle(&vertices[0]->vertexOutput, &vertices[2]->vertexOutput, &vertices[1]->vertexOutput);
            }
            else
            {
                ProcessTriangle(&vertices[0]->vertexOutput, &vertices[1]->vertexOutput, &vertices[2]->vertexOutput);
            }

            // Prepare vertex-indices for the next triangle.
            switch (primitiveType)
            {
            case Yw3d_PT_TriangleFan:
                vertexIndicies[1] = vertexIndicies[2];
                vertexIndicies[2]++;
                break;
            case Yw3d_PT_TriangleStrip:
                flip4TriStrip = !flip4TriStrip;
                vertexIndicies[0] = vertexIndicies[1];
                vertexIndicies[1] = vertexIndicies[2];
                vertexIndicies[2]++;
                break;
            case Yw3d_PT_TriangleList:
                vertexIndicies[0] += 3;
                vertexIndicies[1] += 3;
                vertexIndicies[2] += 3;
                break;
            default:
                // cannot happen.
                break;
            }
        }

        // Process post render state reset.
        PostRender();

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::DrawDynamicPrimitive(uint32_t startVertex, uint32_t numVertices)
    {
        if (numVertices <= 0)
        {
            LOGE(_T("Yw3dDevice::DrawIndexedPrimitive: number of vertices is 0.\n"));
            return Yw3d_E_InvalidParameters;
        }

        if (nullptr == m_PrimitiveAssembler)
        {
            LOGE(_T("Yw3dDevice::DrawDynamicPrimitive: no primitive assembler has been set.\n"));
            return Yw3d_E_InvalidState;
        }

        Yw3dResult resCheck = PreRender();
        if (YW3D_FAILED(resCheck))
        {
            return resCheck;
        }

        // Execute primitive assembler.
        std::vector<uint32_t> vertexIndices;
        Yw3dPrimitiveType primitiveType = m_PrimitiveAssembler->Execute(vertexIndices, numVertices);

        // Get primitive count by type.
        uint32_t primitiveCount = 0;
        switch (primitiveType)
        {
        case Yw3d_PT_TriangleFan:
            primitiveCount = (uint32_t)vertexIndices.size() - 2;
            break;
        case Yw3d_PT_TriangleStrip:
            primitiveCount = (uint32_t)vertexIndices.size() - 2;
            break;
        case Yw3d_PT_TriangleList:
            primitiveCount = (uint32_t)vertexIndices.size() / 3;
            break;
        default:
            LOGE(_T("Yw3dDevice::DrawDynamicPrimitive: invalid primitive type specified.\n"));
            return Yw3d_E_InvalidParameters;
        }

        if (0 == primitiveCount)
        {
            return Yw3d_S_OK;
        }

        uint32_t vertexIndicies[3] = { 0, 1, 2 };
        bool flip4TriStrip = false; // used when drawing triangle strips.
        while (primitiveCount-- > 0)
        {
            Yw3dVertexCacheEntry* vertices[3] = { nullptr, nullptr, nullptr };
            for (uint32_t i = 0; i < 3; i++)
            {
                // Fetch the 3 vertices of this primitive.
                Yw3dResult resFetch = FetchVertex(&vertices[i], vertexIndices[vertexIndicies[i]]);
                if (YW3D_FAILED(resFetch))
                {
                    LOGE(_T("Yw3dDevice::DrawDynamicPrimitive: couldn't fetch vertex from streams.\n"));
                    PostRender();

                    return resFetch;
                }
            }

            // Process this triangle.
            if (flip4TriStrip)
            {
                ProcessTriangle(&vertices[0]->vertexOutput, &vertices[2]->vertexOutput, &vertices[1]->vertexOutput);
            }
            else
            {
                ProcessTriangle(&vertices[0]->vertexOutput, &vertices[1]->vertexOutput, &vertices[2]->vertexOutput);
            }

            // Prepare vertex-indices for the next triangle.
            switch (primitiveType)
            {
            case Yw3d_PT_TriangleFan:
                vertexIndicies[1] = vertexIndicies[2];
                vertexIndicies[2]++;
                break;
            case Yw3d_PT_TriangleStrip:
                flip4TriStrip = !flip4TriStrip;
                vertexIndicies[0] = vertexIndicies[1];
                vertexIndicies[1] = vertexIndicies[2];
                vertexIndicies[2]++;
                break;
            case Yw3d_PT_TriangleList:
                vertexIndicies[0] += 3;
                vertexIndicies[1] += 3;
                vertexIndicies[2] += 3;
                break;
            default:
                // cannot happen.
                break;
            }
        }

        // Process post render state reset.
        PostRender();

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::CreateVertexFormat(Yw3dVertexFormat** vertexFormat, const Yw3dVertexElement* vertexDeclaration, uint32_t vertexDeclSize)
    {
        if (nullptr == vertexFormat)
        {
            LOGE(_T("Yw3dDevice::CreateVertexFormat: parameter vertexFormat points to null.\n"));
            return Yw3d_E_InvalidParameters;
        }

        *vertexFormat = new Yw3dVertexFormat(this);
        if (nullptr == (*vertexFormat))
        {
            LOGE(_T("Yw3dDevice::CreateVertexFormat: out of memory, cannot create vertex format.\n"));
            return Yw3d_E_OutOfMemory;
        }

        Yw3dResult resCreate = (*vertexFormat)->Create(vertexDeclaration, vertexDeclSize);
        if (YW3D_FAILED(resCreate))
        {
            YW_SAFE_RELEASE(*vertexFormat);
            return resCreate;
        }

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::CreateIndexBuffer(Yw3dIndexBuffer** indexBuffer, uint32_t length, Yw3dFormat format)
    {
        if (nullptr == indexBuffer)
        {
            LOGE(_T("Yw3dDevice::CreateIndexBuffer: parameter indexBuffer points to null.\n"));
            return Yw3d_E_InvalidParameters;
        }

        *indexBuffer = new Yw3dIndexBuffer(this);
        if (nullptr == (*indexBuffer))
        {
            LOGE(_T("Yw3dDevice::CreateIndexBuffer: out of memory, cannot create indexbuffer.\n"));
            return Yw3d_E_OutOfMemory;
        }

        Yw3dResult resCreate = (*indexBuffer)->Create(length, format);
        if (YW3D_FAILED(resCreate))
        {
            YW_SAFE_RELEASE(*indexBuffer);
            return resCreate;
        }

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::CreateVertexBuffer(Yw3dVertexBuffer** vertexBuffer, uint32_t length)
    {
        if (nullptr == vertexBuffer)
        {
            LOGE(_T("Yw3dDevice::CreateVertexBuffer: parameter vertexBuffer points to null.\n"));
            return Yw3d_E_InvalidParameters;
        }

        *vertexBuffer = new Yw3dVertexBuffer(this);
        if (nullptr == (*vertexBuffer))
        {
            LOGE(_T("Yw3dDevice::CreateVertexBuffer: out of memory, cannot create vertexbuffer.\n"));
            return Yw3d_E_OutOfMemory;
        }

        Yw3dResult resCreate = (*vertexBuffer)->Create(length);
        if (YW3D_FAILED(resCreate))
        {
            YW_SAFE_RELEASE(*vertexBuffer);
            return resCreate;
        }

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::CreateSurface(Yw3dSurface** surface, uint32_t width, uint32_t height, Yw3dFormat format)
    {
        if (nullptr == surface)
        {
            LOGE(_T("Yw3dDevice::CreateSurface: parameter surface points to null.\n"));
            return Yw3d_E_InvalidParameters;
        }

        *surface = new Yw3dSurface(this);
        if (nullptr == (*surface))
        {
            LOGE(_T("Yw3dDevice::CreateSurface: out of memory, cannot create surface.\n"));
            return Yw3d_E_OutOfMemory;
        }

        Yw3dResult resCreate = (*surface)->Create(width, height, format);
        if (YW3D_FAILED(resCreate))
        {
            YW_SAFE_RELEASE(*surface);
            return resCreate;
        }

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::CreateTexture(Yw3dTexture** texture, uint32_t width, uint32_t height, uint32_t mipLevels, Yw3dFormat format)
    {
        if (nullptr == texture)
        {
            LOGE(_T("Yw3dDevice::CreateTexture: parameter texture points to null.\n"));
            return Yw3d_E_InvalidParameters;
        }

        *texture = new Yw3dTexture(this);
        if (nullptr == (*texture))
        {
            LOGE(_T("Yw3dDevice::CreateTexture: out of memory, cannot create texture.\n"));
            return  Yw3d_E_OutOfMemory;
        }

        Yw3dResult resCreate = (*texture)->Create(width, height, mipLevels, format);
        if (YW3D_FAILED(resCreate))
        {
            YW_SAFE_RELEASE(*texture);
            return resCreate;
        }

        return  Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::CreateCubeTexture(Yw3dCubeTexture** cubeTexture, uint32_t edgeLength, uint32_t mipLevels, Yw3dFormat format)
    {
        if (nullptr == cubeTexture)
        {
            LOGE(_T("Yw3dDevice::CreateCubeTexture: parameter cubeTexture points to null.\n"));
            return  Yw3d_E_InvalidParameters;
        }

        *cubeTexture = new Yw3dCubeTexture(this);
        if (nullptr == (*cubeTexture))
        {
            LOGE(_T("Yw3dDevice::CreateCubeTexture: out of memory, cannot create cube-texture.\n"));
            return  Yw3d_E_OutOfMemory;
        }

        Yw3dResult resCreate = (*cubeTexture)->Create(edgeLength, mipLevels, format);
        if (YW3D_FAILED(resCreate))
        {
            YW_SAFE_RELEASE(*cubeTexture);
            return resCreate;
        }

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::CreateVolume(Yw3dVolume** volume, uint32_t width, uint32_t height, uint32_t depth, Yw3dFormat format)
    {
        if (nullptr == volume)
        {
            LOGE(_T("Yw3dDevice::CreateVolume: parameter volume points to null.\n"));
            return  Yw3d_E_InvalidParameters;
        }

        *volume = new Yw3dVolume(this);
        if (nullptr == (*volume))
        {
            LOGE(_T("Yw3dDevice::CreateVolume: out of memory, cannot create volume.\n"));
            return  Yw3d_E_OutOfMemory;
        }

        Yw3dResult resCreate = (*volume)->Create(width, height, depth, format);
        if (YW3D_FAILED(resCreate))
        {
            YW_SAFE_RELEASE(*volume);
            return resCreate;
        }

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::CreateVolumeTexture(Yw3dVolumeTexture** volumeTexture, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevels, Yw3dFormat format)
    {
        if (nullptr == volumeTexture)
        {
            LOGE(_T("Yw3dDevice::CreateVolumeTexture: parameter volumeTexture points to null.\n"));
            return  Yw3d_E_InvalidParameters;
        }

        *volumeTexture = new Yw3dVolumeTexture(this);
        if (nullptr == (*volumeTexture))
        {
            LOGE(_T("Yw3dDevice::CreateVolumeTexture: out of memory, cannot create volume-texture.\n"));
            return  Yw3d_E_OutOfMemory;
        }

        Yw3dResult resCreate = (*volumeTexture)->Create(width, height, depth, mipLevels, format);
        if (YW3D_FAILED(resCreate))
        {
            YW_SAFE_RELEASE(*volumeTexture);
            return resCreate;
        }

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::CreateRenderTarget(Yw3dRenderTarget** renderTarget, uint32_t width, uint32_t height, Yw3dFormat colorFormat, Yw3dFormat depthFormat)
    {
        if (nullptr == renderTarget)
        {
            LOGE(_T("Yw3dDevice::CreateRenderTarget: parameter renderTarget points to null.\n"));
            return Yw3d_E_InvalidParameters;
        }

        *renderTarget = new Yw3dRenderTarget(this);
        if (nullptr == (*renderTarget))
        {
            LOGE(_T("Yw3dDevice::CreateRenderTarget: out of memory, cannot create rendertarget.\n"));
            return Yw3d_E_OutOfMemory;
        }

        // Create the color buffer.
        Yw3dSurface* colorBuffer = nullptr;
        if (YW3D_FAILED(CreateSurface(&colorBuffer, width, height, colorFormat)))
        {
            LOGE(_T("Yw3dDevice::CreateRenderTarget: can not create color buffer for render target.\n"));
            return Yw3d_E_InvalidParameters;
        }

        // Create the depth texture.
        Yw3dSurface* depthBuffer = nullptr;
        if (YW3D_FAILED(CreateSurface(&depthBuffer, width, height, depthFormat)))
        {
            YW_SAFE_RELEASE(colorBuffer);
            LOGE(_T("Yw3dDevice::CreateRenderTarget: can not create depth buffer for render target.\n"));

            return Yw3d_E_InvalidParameters;
        }

        // Set color buffer and depth buffer.
        (*renderTarget)->SetColorBuffer(colorBuffer);
        (*renderTarget)->SetDepthBuffer(depthBuffer);

        // Decrease the references of buffer.
        YW_SAFE_RELEASE(colorBuffer);
        YW_SAFE_RELEASE(depthBuffer);

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::SetTransform(Yw3dTransformState transformState, const Matrix44* transform)
    {
        if (transformState >= Yw3d_TS_NumTransformState)
        {
            LOGE(_T("Yw3dDevice::SetTransform: invalid transform state.\n"));
            return Yw3d_E_InvalidParameters;
        }

        if (nullptr == transform)
        {
            LOGE(_T("Yw3dDevice::SetTransform: invalid transform value.\n"));
            return Yw3d_E_InvalidParameters;
        }

        m_TransformState[transformState] = *transform;
        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::GetTransform(Yw3dTransformState transformState, const Matrix44*& transform)
    {
        transform = nullptr;
        if (transformState >= Yw3d_TS_NumTransformState)
        {
            LOGE(_T("Yw3dDevice::SetTransform: invalid transform state.\n"));
            return Yw3d_E_InvalidParameters;
        }

        transform = &m_TransformState[transformState];
        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::SetViewportMatrix(const Matrix44* viewportMatrix)
    {
        if (nullptr == viewportMatrix)
        {
            LOGE(_T("Yw3dDevice::SetViewportMatrix: invalid viewport matrix value.\n"));
            return Yw3d_E_InvalidParameters;
        }

        m_ViewportMatrix = *viewportMatrix;
        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::GetViewportMatrix(const Matrix44*& viewportMatrix) const
    {
        viewportMatrix = &m_ViewportMatrix;
        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::SetRenderState(Yw3dRenderState renderState, uint32_t value)
    {
        if (renderState >= Yw3d_RS_NumRenderStates)
        {
            LOGE(_T("Yw3dDevice::SetRenderState: invalid renderstate.\n"));
            return Yw3d_E_InvalidParameters;
        }

        m_RenderStates[renderState] = value;
        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::GetRenderState(Yw3dRenderState renderState, uint32_t& value)
    {
        if (renderState >= Yw3d_RS_NumRenderStates)
        {
            value = 0;
            LOGE(_T("Yw3dDevice::SetRenderState: invalid renderstate.\n"));

            return Yw3d_E_InvalidParameters;
        }

        value = m_RenderStates[renderState];
        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::SetVertexFormat(Yw3dVertexFormat* vertexFormat)
    {
        m_VertexFormat = vertexFormat;
        if (nullptr == vertexFormat)
        {
            return Yw3d_S_OK;
        }

        // Initialize part of internal render-info structure.
        for (uint32_t regIdx = 0; regIdx < YW3D_VERTEX_SHADER_REGISTERS; regIdx++)
        {
            m_RenderInfo.vsInputRegisterTypes[regIdx] = Yw3d_SRT_Unused;
        }

        const Yw3dVertexElement* curVertexElement = m_VertexFormat->GetElements();
        uint32_t elementCount = m_VertexFormat->GetNumVertexElements();
        while (elementCount--)
        {
            switch (curVertexElement->type)
            {
            case Yw3d_VET_Float:
                m_RenderInfo.vsInputRegisterTypes[curVertexElement->shaderRegister] = Yw3d_SRT_Float32;
                break;
            case Yw3d_VET_Vector2:
                m_RenderInfo.vsInputRegisterTypes[curVertexElement->shaderRegister] = Yw3d_SRT_Vector2;
                break;
            case Yw3d_VET_Vector3:
                m_RenderInfo.vsInputRegisterTypes[curVertexElement->shaderRegister] = Yw3d_SRT_Vector3;
                break;
            case Yw3d_VET_Vector4:
                m_RenderInfo.vsInputRegisterTypes[curVertexElement->shaderRegister] = Yw3d_SRT_Vector4;
                break;
            default: /* cannot happen */ 
                LOGE(_T("Yw3dDevice::SetVertexFormat: invalid vertex element type.\n"));
                return  Yw3d_E_InvalidParameters;
            }

            ++curVertexElement;
        }

        return  Yw3d_S_OK;
    }

    Yw3dVertexFormat* Yw3dDevice::AcquireVertexFormat()
    {
        if (nullptr != m_VertexFormat)
        {
            m_VertexFormat->AddRef();
        }

        return m_VertexFormat;
    }

    void Yw3dDevice::SetPrimitiveAssembler(IYw3dPrimitiveAssembler* primitiveAssembler)
    {
        m_PrimitiveAssembler = primitiveAssembler;
    }

    IYw3dPrimitiveAssembler* Yw3dDevice::AcquirePrimitiveAssembler()
    {
        if (nullptr != m_PrimitiveAssembler)
        {
            m_PrimitiveAssembler->AddRef();
        }

        return m_PrimitiveAssembler;
    }

    Yw3dResult Yw3dDevice::SetVertexShader(IYw3dVertexShader* vertexShader)
    {
        if (nullptr == vertexShader)
        {
            m_VertexShader = vertexShader;
            return  Yw3d_S_OK;
        }

        // Validate vertex shader register output types.
        for (uint32_t regIdx = 0; regIdx < YW3D_PIXEL_SHADER_REGISTERS; regIdx++)
        {
            uint32_t regType = vertexShader->GetOutputRegisters(regIdx);
            if ((regType < Yw3d_SRT_Unused) || (regType > Yw3d_SRT_Vector4))
            {
                LOGE(_T("Yw3dDevice::SetVertexShader: type of vertexshader output register is invalid.\n"));
                return Yw3d_E_InvalidState;
            }
        }

        m_VertexShader = vertexShader;
        return Yw3d_S_OK;
    }

    IYw3dVertexShader* Yw3dDevice::AcquireVertexShader()
    {
        if (nullptr != m_VertexShader)
        {
            m_VertexShader->AddRef();
        }

        return m_VertexShader;
    }

    Yw3dResult Yw3dDevice::SetTriangleShader(IYw3dTriangleShader* triangleShader)
    {
        m_TriangleShader = triangleShader;
        return Yw3d_S_OK;
    }

    IYw3dTriangleShader* Yw3dDevice::AcquireTriangleShader()
    {
        if (nullptr != m_TriangleShader)
        {
            m_TriangleShader->AddRef();
        }

        return m_TriangleShader;
    }

    Yw3dResult Yw3dDevice::SetPixelShader(IYw3dPixelShader* pixelShader)
    {
        m_PixelShader = pixelShader;
        return Yw3d_S_OK;
    }

    IYw3dPixelShader* Yw3dDevice::AcquirePixelShader()
    {
        if (nullptr != m_PixelShader)
        {
            m_PixelShader->AddRef();
        }

        return m_PixelShader;
    }

    Yw3dResult Yw3dDevice::SetIndexBuffer(Yw3dIndexBuffer* indexBuffer)
    {
        m_IndexBuffer = indexBuffer;
        return Yw3d_S_OK;
    }

    Yw3dIndexBuffer* Yw3dDevice::AcquireIndexBuffer()
    {
        if (nullptr != m_IndexBuffer)
        {
            m_IndexBuffer->AddRef();
        }

        return m_IndexBuffer;
    }

    Yw3dResult Yw3dDevice::SetVertexStream(uint32_t streamNumber, Yw3dVertexBuffer* vertexBuffer, uint32_t offset, uint32_t stride)
    {
        if (streamNumber >= YW3D_MAX_VERTEX_STREAMS)
        {
            LOGE(_T("Yw3dDevice::SetVertexStream: streamNumber exceeds number of available vertex streams.\n"));
            return Yw3d_E_InvalidParameters;
        }

        if (0 == stride)
        {
            LOGE(_T("Yw3dDevice::SetVertexStream: stride is 0.\n"));
            return Yw3d_E_InvalidParameters;
        }

        m_VertexStreams[streamNumber].vertexBuffer = vertexBuffer;
        m_VertexStreams[streamNumber].offset = offset;
        m_VertexStreams[streamNumber].stride = stride;

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::AcquireVertexStream(uint32_t streamNumber, Yw3dVertexBuffer** vertexBuffer, uint32_t* offset, uint32_t* stride)
    {
        if (streamNumber >= YW3D_MAX_VERTEX_STREAMS)
        {
            if (nullptr != vertexBuffer)
            {
                *vertexBuffer = nullptr;
            }

            if (nullptr != offset)
            {
                *offset = 0;
            }

            if (nullptr != stride)
            {
                *stride = 0;
            }

            LOGE(_T("Yw3dDevice::GetVertexStream: streamNumber exceeds number of available vertex streams.\n"));
            return Yw3d_E_InvalidParameters;
        }

        if (nullptr != vertexBuffer)
        {
            *vertexBuffer = m_VertexStreams[streamNumber].vertexBuffer;
            if (nullptr != m_VertexStreams[streamNumber].vertexBuffer)
            {
                m_VertexStreams[streamNumber].vertexBuffer->AddRef();
            }
        }

        if (nullptr != offset)
        {
            *offset = m_VertexStreams[streamNumber].offset;
        }

        if (nullptr != stride)
        {
            *stride = m_VertexStreams[streamNumber].offset;
        }

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::SetTexture(uint32_t samplerNumber, IYw3dBaseTexture* texture)
    {
        if (samplerNumber >= YW3D_MAX_TEXTURE_SAMPLERS)
        {
            LOGE(_T("Yw3dDevice::SetTexture: samplerNumber exceeds number of available texture samplers.\n"));
            return Yw3d_E_InvalidParameters;
        }

        m_TextureSamplers[samplerNumber].texture = texture;
        if (nullptr != texture)
        {
            m_TextureSamplers[samplerNumber].textureSampleInput = texture->GetTextureSampleInput();
        }

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::AcquireTexture(uint32_t samplerNumber, IYw3dBaseTexture** texture)
    {
        if (samplerNumber >= YW3D_MAX_TEXTURE_SAMPLERS)
        {
            LOGE(_T("Yw3dDevice::GetTexture: samplerNumber exceeds number of available texture samplers.\n"));
            return Yw3d_E_InvalidParameters;
        }

        if (nullptr == texture)
        {
            LOGE(_T("Yw3dDevice::GetTexture: parameter texture points to null.\n"));
            return Yw3d_E_InvalidParameters;
        }

        IYw3dBaseTexture* requestTexture = m_TextureSamplers[samplerNumber].texture;
        *texture = requestTexture;
        if (nullptr != requestTexture)
        {
            requestTexture->AddRef();
        }

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::SetTextureSamplerState(uint32_t samplerNumber, Yw3dTextureSamplerState textureSamplerState, uint32_t state)
    {
        if (samplerNumber >= YW3D_MAX_TEXTURE_SAMPLERS)
        {
            LOGE(_T("Yw3dDevice::SetTextureSamplerState: samplerNumber exceeds number of available texture samplers.\n"));
            return Yw3d_E_InvalidParameters;
        }

        if (textureSamplerState >= Yw3d_TSS_NumTextureSamplerStates)
        {
            LOGE(_T("Yw3dDevice::SetTextureSamplerState: invalid texture sampler state.\n"));
            return Yw3d_E_InvalidParameters;
        }

        m_TextureSamplers[samplerNumber].textureSamplerStates[textureSamplerState] = state;
        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::GetTextureSamplerState(uint32_t samplerNumber, Yw3dTextureSamplerState textureSamplerState, uint32_t& state)
    {
        if (samplerNumber >= YW3D_MAX_TEXTURE_SAMPLERS)
        {
            state = 0;
            LOGE(_T("Yw3dDevice::GetTextureSamplerState: samplerNumber exceeds number of available texture samplers.\n"));

            return Yw3d_E_InvalidParameters;
        }

        if (textureSamplerState >= Yw3d_TSS_NumTextureSamplerStates)
        {
            state = 0;
            LOGE(_T("Yw3dDevice::GetTextureSamplerState: invalid texture sampler state.\n"));

            return Yw3d_E_InvalidParameters;
        }

        state = m_TextureSamplers[samplerNumber].textureSamplerStates[textureSamplerState];
        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::SampleTexture(Vector4& color, uint32_t samplerNumber, float u, float v, float w, const Vector4* xGradient, const Vector4* yGradient)
    {
        if (samplerNumber >= YW3D_MAX_TEXTURE_SAMPLERS)
        {
            color = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
            LOGE(_T("Yw3dDevice::SampleTexture: samplerNumber exceeds number of available texture samplers.\n"));
            return Yw3d_E_InvalidParameters;
        }

        // Get texture sampler.
        const TextureSampler& textureSampler = m_TextureSamplers[samplerNumber];

        // Get texture.
        IYw3dBaseTexture* texture = textureSampler.texture;
        if (nullptr == texture)
        {
            color = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
            return Yw3d_S_OK;
        }

        // Correct u, v, w by input type and address type.
        switch (textureSampler.textureSampleInput)
        {
        case Yw3d_TSI_Vector:
            if ((0.0f == u) && (0.0f == v) && (0.0f == w))
            {
                color = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
                LOGE(_T("Yw3dDevice::SampleTexture: sampling vector [u,v,w] = [0,0,0].\n"));
                return Yw3d_E_InvalidParameters;
            }
            break;
        case Yw3d_TSI_3Coords:
            switch (textureSampler.textureSamplerStates[Yw3d_TSS_AddressW])
            {
            case Yw3d_TA_Wrap: w -= ftol(w);
            case Yw3d_TA_Clamp: w = Saturate(w); break;
            default: color = Vector4(0.0f, 0.0f, 0.0f, 0.0f); LOGE(_T("Yw3dDevice::SampleTexture: value of texture sampler state Yw3d_TSS_AddressW is invalid.\n")); return Yw3d_E_InvalidState;
            }
        case Yw3d_TSI_2Coords:
            switch (textureSampler.textureSamplerStates[Yw3d_TSS_AddressV])
            {
            case Yw3d_TA_Wrap: v -= ftol(v);
            case Yw3d_TA_Clamp: v = Saturate(v); break;
            default: color = Vector4(0.0f, 0.0f, 0.0f, 0.0f); LOGE(_T("Yw3dDevice::SampleTexture: value of texture sampler state Yw3d_TSS_AddressV is invalid.\n")); return Yw3d_E_InvalidState;
            }

            switch (textureSampler.textureSamplerStates[Yw3d_TSS_AddressU])
            {
            case Yw3d_TA_Wrap: u -= ftol(u);
            case Yw3d_TA_Clamp: u = Saturate(u); break;
            default: color = Vector4(0.0f, 0.0f, 0.0f, 0.0f); LOGE(_T("Yw3dDevice::SampleTexture: value of texture sampler state Yw3d_TSS_AddressU is invalid.\n")); return Yw3d_E_InvalidState;
            }
        default:
            color = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
            LOGE(_T("CMuli3DDevice::SampleTexture: invalid texture-sampling input!\n"));
            return Yw3d_E_InvalidState;
        }

        return texture->SampleTexture(color, u, v, w, xGradient, yGradient, textureSampler.textureSamplerStates);
    }

    void Yw3dDevice::SetRenderTarget(Yw3dRenderTarget* renderTarget)
    {
        m_RenderTarget = renderTarget;
    }

    Yw3dRenderTarget* Yw3dDevice::AcquireRenderTarget()
    {
        if (nullptr != m_RenderTarget)
        {
            m_RenderTarget->AddRef();
        }

        return m_RenderTarget;
    }

    Yw3dResult Yw3dDevice::SetScissorRect(const Yw3dRect& scissorRect)
    {
        if ((scissorRect.left >= scissorRect.right) || (scissorRect.top >= scissorRect.bottom))
        {
            LOGE(_T("Yw3dDevice::SetScissorRect: invalid scissor rect!\n"));
            return Yw3d_E_InvalidParameters;
        }

        m_ScissorRect = scissorRect;

        // Construct planes for clipping to the scissor rect.
        // Ax + By + Cz + D = 0.
        // Normal -> (na, nb, nc) -> (A, B, C).
        // D -> nd -> -(N dot P).
        // Take left as example: N = (1, 0, 0), P = (m_ScissorRect.left, 0, 0), so D = -(N dot P) = -[(1, 0, 0) dot (m_ScissorRect.left, 0, 0)] = -m_ScissorRect.left.
        m_RenderInfo.scissorPlanes[0] = Plane(1.0f, 0.0f, 0.0f, -(float)m_ScissorRect.left);
        m_RenderInfo.scissorPlanes[1] = Plane(-1.0f, 0.0f, 0.0f, (float)m_ScissorRect.right);
        m_RenderInfo.scissorPlanes[2] = Plane(0.0f, 1.0f, 0.0f, -(float)m_ScissorRect.top);
        m_RenderInfo.scissorPlanes[3] = Plane(0.0f, -1.0f, 0.0f, (float)m_ScissorRect.bottom);

        return Yw3d_S_OK;
    }

    Yw3dRect Yw3dDevice::GetScissorRect() const
    {
        return m_ScissorRect;
    }

    Yw3dResult Yw3dDevice::SetDepthBounds(float minZ, float maxZ)
    {
        if ((minZ < 0.0f) || (minZ > 1.0f) || (maxZ < 0.0f) || (maxZ > 1.0f) || (minZ >= maxZ))
        {
            LOGE(_T("Yw3dDevice::SetDepthBounds: invalid depth bounds!\n"));
            return Yw3d_E_InvalidParameters;
        }

        m_RenderInfo.clippingPlanes[Yw3d_CP_Near].d = minZ;
        m_RenderInfo.clippingPlanes[Yw3d_CP_Far].d = maxZ;

        return Yw3d_S_OK;
    }

    void Yw3dDevice::GetDepthBounds(float& minZ, float& maxZ) const
    {
        minZ = m_RenderInfo.clippingPlanes[Yw3d_CP_Near].d;
        maxZ = m_RenderInfo.clippingPlanes[Yw3d_CP_Far].d;
    }

    Yw3dResult Yw3dDevice::SetClippingPlane(Yw3dClippingPlanes index, const Plane* plane)
    {
        if ((index < Yw3d_CP_User0) || (index >= Yw3d_CP_NumPlanes))
        {
            return Yw3d_E_InvalidParameters;
        }

        if (nullptr != plane)
        {
            m_RenderInfo.clippingPlanes[index] = *plane;
            m_RenderInfo.clippingPlaneEnabled[index] = true;
        }
        else
        {
            m_RenderInfo.clippingPlaneEnabled[index] = false;
        }

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::GetClippingPlane(Yw3dClippingPlanes index, Plane& plane) const
    {
        if ((index < Yw3d_CP_User0) || index >= Yw3d_CP_NumPlanes)
        {
            return Yw3d_E_InvalidParameters;
        }

        if (!m_RenderInfo.clippingPlaneEnabled[index])
        {
            return Yw3d_E_InvalidState;
        }

        plane = m_RenderInfo.clippingPlanes[index];
        return Yw3d_S_OK;
    }

    uint32_t Yw3dDevice::GetRenderedPixels() const
    {
        return m_RenderInfo.renderedPixels;
    }

    void Yw3dDevice::SetDefaultRenderStates()
    {
        SetRenderState(Yw3d_RS_ZEnable, true);
        SetRenderState(Yw3d_RS_ZWriteEnable, true);
        SetRenderState(Yw3d_RS_ZFunc, Yw3d_CMP_Less);

        SetRenderState(Yw3d_RS_ColorWriteEnable, true);
        SetRenderState(Yw3d_RS_FillMode, Yw3d_Fill_Solid);

        SetRenderState(Yw3d_RS_CullMode, Yw3d_Cull_CCW);

        SetRenderState(Yw3d_RS_SubdivisionMode, Yw3d_Subdivision_None);
        SetRenderState(Yw3d_RS_SubdivisionLevels, 1);
        SetRenderState(Yw3d_RS_SubdivisionPositionRegister, 0);
        SetRenderState(Yw3d_RS_SubdivisionNormalRegister, 1);

        const float defaultSubdividingMaxScreenArea = 1.0f;
        SetRenderState(Yw3d_RS_SubdivisionMaxScreenArea, *(uint32_t*)&defaultSubdividingMaxScreenArea);
        SetRenderState(Yw3d_RS_SubdivisionMaxInnerLevels, 1);

        SetRenderState(Yw3d_RS_ScissorTestEnable, false);

        SetRenderState(Yw3d_RS_LineThickness, 1);
    }

    void Yw3dDevice::SetDefaultTextureSamplerStates()
    {
        const float mipLODBias = 0.0f;
        const float maxMipLevel = 16.0f;
        for (uint32_t textureSampler = 0; textureSampler < YW3D_MAX_TEXTURE_SAMPLERS; textureSampler++)
        {
            SetTextureSamplerState(textureSampler, Yw3d_TSS_AddressU, Yw3d_TA_Wrap);
            SetTextureSamplerState(textureSampler, Yw3d_TSS_AddressV, Yw3d_TA_Wrap);
            SetTextureSamplerState(textureSampler, Yw3d_TSS_AddressW, Yw3d_TA_Wrap);
            SetTextureSamplerState(textureSampler, Yw3d_TSS_MinFilter, Yw3d_TF_Linear);
            SetTextureSamplerState(textureSampler, Yw3d_TSS_MagFilter, Yw3d_TF_Linear);
            SetTextureSamplerState(textureSampler, Yw3d_TSS_MipFilter, Yw3d_TF_Point);
            SetTextureSamplerState(textureSampler, Yw3d_TSS_MipLodBias, *(uint32_t*)&mipLODBias);
            SetTextureSamplerState(textureSampler, Yw3d_TSS_MaxMipLevel, *(uint32_t*)&maxMipLevel);
        }
    }

    void Yw3dDevice::SetDefaultClippingPlanes()
    {
        m_RenderInfo.clippingPlanes[Yw3d_CP_Left] = Plane(1.0f, 0.0f, 0.0f, 1.0f);
        m_RenderInfo.clippingPlanes[Yw3d_CP_Right] = Plane(-1.0f, 0.0f, 0.0f, 1.0f);
        m_RenderInfo.clippingPlanes[Yw3d_CP_Top] = Plane(0.0f, -1.0f, 0.0f, 1.0f);
        m_RenderInfo.clippingPlanes[Yw3d_CP_Bottom] = Plane(0.0f, 1.0f, 0.0f, 1.0f);
        m_RenderInfo.clippingPlanes[Yw3d_CP_Near] = Plane(0.0f, 0.0f, 1.0f, 0.0f);
        m_RenderInfo.clippingPlanes[Yw3d_CP_Far] = Plane(0.0f, 0.0f, -1.0f, 1.0f);

        // Enable the default clipping planes ...
        for (uint32_t planeIdx = Yw3d_CP_Left; planeIdx <= Yw3d_CP_Far; planeIdx++)
        {
            m_RenderInfo.clippingPlaneEnabled[planeIdx] = true;
        }
    }

    void Yw3dDevice::SetDefaultRenderTarget()
    {
        // Create a default render target.
        if (YW3D_FAILED(CreateRenderTarget(&m_RenderTarget, m_DeviceParameters.backBufferWidth, m_DeviceParameters.backBufferHeight, Yw3d_FMT_R32G32B32F, Yw3d_FMT_R32F)))
        {
            LOGE(_T("Yw3dDevice::SetDefaultRenderTarget: can not create a default render target.\n"));
            return;
        }
    }

    Yw3dResult Yw3dDevice::PreRender()
    {
        if (nullptr == m_VertexFormat)
        {
            LOGE(_T("Yw3dDevice::PreRender: no vertex format has been set.\n"));
            return Yw3d_E_InvalidState;
        }

        if (nullptr == m_VertexShader)
        {
            LOGE(_T("Yw3dDevice::PreRender: no vertex shader has been set.\n"));
            return Yw3d_E_InvalidState;
        }

        if (nullptr == m_PixelShader)
        {
            LOGE(_T("Yw3dDevice::PreRender: no pixel shader has been set.\n"));
            return Yw3d_E_InvalidState;
        }

        if (nullptr == m_RenderTarget)
        {
            LOGE(_T("Yw3dDevice::PreRender: no render target has been set.\n"));
            return Yw3d_E_InvalidState;
        }

        // Set viewport for this triangle info.
        Yw3dRect& triangleViewport = m_RenderInfo.viewportRect;
        triangleViewport.left = (uint32_t)(m_ViewportMatrix._41 - m_ViewportMatrix._11);
        triangleViewport.right = (uint32_t)(m_ViewportMatrix._41 + m_ViewportMatrix._11);
        triangleViewport.top = (uint32_t)(m_ViewportMatrix._42 + m_ViewportMatrix._22);
        triangleViewport.bottom = (uint32_t)(m_ViewportMatrix._42 - m_ViewportMatrix._22);

        // Check validation of triangle viewport.
        if ((triangleViewport.left >= triangleViewport.right) || (triangleViewport.top >= triangleViewport.bottom))
        {
            LOGE(_T("Yw3dDevice::PreRender: invalid viewport matrix.\n"));
            return Yw3d_E_InvalidState;
        }

        // Get color and depth buffer.
        Yw3dSurface* colorBuffer = m_RenderTarget->AcquireColorBuffer();
        Yw3dSurface* depthBuffer = m_RenderTarget->AcquireDepthBuffer();
        if ((nullptr == colorBuffer) && (nullptr == depthBuffer))
        {
            LOGE(_T("Yw3dDevice::PreRender: render target has no associated frame buffer and depth buffer.\n"));
            return Yw3d_E_InvalidState;
        }

        // Check color buffer with viewport.
        if ((nullptr != colorBuffer) && ((colorBuffer->GetWidth() < triangleViewport.right) || (colorBuffer->GetHeight() < triangleViewport.bottom)))
        {
            LOGE(_T("Yw3dDevice::PreRender: color buffer's dimensions are smaller than set viewport.\n"));
            YW_SAFE_RELEASE(colorBuffer);
            YW_SAFE_RELEASE(depthBuffer);

            return Yw3d_E_InvalidState;
        }

        // Check depth buffer with viewport.
        if ((nullptr != depthBuffer) && ((depthBuffer->GetWidth() < triangleViewport.right) || (depthBuffer->GetHeight() < triangleViewport.bottom)))
        {
            LOGE(_T("Yw3dDevice::PreRender: depth buffer's dimensions are smaller than set viewport.\n"));
            YW_SAFE_RELEASE(colorBuffer);
            YW_SAFE_RELEASE(depthBuffer);

            return Yw3d_E_InvalidState;
        }

        YW_SAFE_RELEASE(colorBuffer);
        YW_SAFE_RELEASE(depthBuffer);

        // Check vertex stream.
        const VertexStream* curVertexStream = m_VertexStreams;
        for (uint32_t streamIdx = 0; streamIdx <= m_VertexFormat->GetHighestStream(); streamIdx++, curVertexStream++)
        {
            if (nullptr == curVertexStream->vertexBuffer)
            {
                LOGE(_T("Yw3dDevice::PreRender: vertex format references an empty vertex stream.\n"));
                return Yw3d_E_InvalidState;
            }
        }

        // Check status of scissor-testing.
        if (m_RenderStates[Yw3d_RS_ScissorTestEnable])
        {
            if (
                !((m_ScissorRect.left >= triangleViewport.left) && (m_ScissorRect.left < triangleViewport.right)) ||
                !((m_ScissorRect.right > triangleViewport.left) && (m_ScissorRect.right <= triangleViewport.right)) ||
                !((m_ScissorRect.top >= triangleViewport.top) && (m_ScissorRect.top < triangleViewport.bottom)) ||
                !((m_ScissorRect.bottom > triangleViewport.top) && (m_ScissorRect.bottom <= triangleViewport.bottom))
            )
            {
                LOGE(_T("Yw3dDevice::PreRender: scissor rect exceeds viewport's dimensions.\n"));
                return Yw3d_E_InvalidState;
            }
        }

        // Check line-thickness.
        if (0 == m_RenderStates[Yw3d_RS_LineThickness])
        {
            LOGE(_T("Yw3dDevice::PreRender: line-thickness is invalid.\n"));
            return Yw3d_E_InvalidState;
        }

        // Check if render states for subdivision-mode are valid.
        switch (m_RenderStates[Yw3d_RS_SubdivisionMode])
        {
        case Yw3d_Subdivision_None:
            break;
        case Yw3d_Subdivision_Simple:
            {
                if (0 == m_RenderStates[Yw3d_RS_SubdivisionLevels])
                {
                    LOGE(_T("Yw3dDevice::PreRender: subdivision levels for simple-subdivision are 0.\n"));
                    return Yw3d_E_InvalidState;
                }
            }
            break;
        case Yw3d_Subdivision_Smooth:
            {
                if (0 == m_RenderStates[Yw3d_RS_SubdivisionLevels])
                {
                    LOGE(_T("Yw3dDevice::PreRender: subdivision levels for smooth-subdivision are 0.\n"));
                    return Yw3d_E_InvalidState;
                }
                else if (m_RenderStates[Yw3d_RS_SubdivisionPositionRegister] >= YW3D_VERTEX_SHADER_REGISTERS)
                {
                    LOGE(_T("Yw3dDevice::PreRender: position register for smooth-subdivision exceeds number of avilable vertex shader input registers.\n"));
                    return Yw3d_E_InvalidState;
                }
                else if (m_RenderStates[Yw3d_RS_SubdivisionNormalRegister] >= YW3D_VERTEX_SHADER_REGISTERS)
                {
                    LOGE(_T("Yw3dDevice::PreRender: position register for smooth-subdivision exceeds number of avilable vertex shader input registers.\n"));
                    return Yw3d_E_InvalidState;
                }
            }
            break;
        case Yw3d_Subdivision_Adaptive:
            {
                if ((0 == m_RenderStates[Yw3d_RS_SubdivisionLevels]) && (0 == Yw3d_RS_SubdivisionMaxInnerLevels))
                {
                    LOGE(_T("Yw3dDevice::PreRender: both subdivision levels for adaptive-subdivision are 0.\n"));
                    return Yw3d_E_InvalidState;
                }
                else if (*(float*)&m_RenderStates[Yw3d_RS_SubdivisionMaxScreenArea] <= 0.0f)
                {
                    LOGE(_T("Yw3dDevice::PreRender: max screenarea for smooth-subdivision is <= 0.0f.\n"));
                    return Yw3d_E_InvalidState;
                }
            }
            break;
        default:
            LOGE(_T("Yw3dDevice::PreRender: value of render state Yw3d_RS_SubdivisionMode is invalid.\n"));
            return Yw3d_E_InvalidState;
        }

        // Check for valid device-states, which won't produce any output.
        if (m_RenderStates[Yw3d_RS_ZEnable] && (Yw3d_CMP_Never == m_RenderStates[Yw3d_RS_ZFunc]))
        {
            LOGI(_T("Yw3dDevice::PreRender: nothing will be rendered - depth buffering has been enabled and the compare-function has been set to m3dcmp_never - nothing will be rendered to the screen.\n"));
        }

        if (m_RenderStates[Yw3d_RS_ScissorTestEnable])
        {
            if ((m_ScissorRect.left == m_ScissorRect.right) || (m_ScissorRect.top == m_ScissorRect.bottom))
            {
                LOGI(_T("Yw3dDevice::PreRender: nothing will be rendered - scissor testing is enabled and scissor rect spans across an area of 0 pixels.\n"));
            }
        }

        if (!m_RenderStates[Yw3d_RS_ColorWriteEnable] && (!m_RenderStates[Yw3d_RS_ZEnable] || !m_RenderStates[Yw3d_RS_ZWriteEnable]))
        {
            LOGI(_T("Yw3dDevice::PreRender: nothing will be rendered - writing to the colorbuffer and the depthbuffer has been disabled.\n"));
        }

        // Todo: Add more checks.
        // ...

        // Initialize internal render-info structure.

        // Store output register types in the internal render-info structure.
        for (uint32_t regIdx = 0; regIdx < YW3D_PIXEL_SHADER_REGISTERS; regIdx++)
        {
            m_RenderInfo.vsOutputRegisterTypes[regIdx] = m_VertexShader->GetOutputRegisters(regIdx);
        }

        // Store color buffer related states.
        colorBuffer = m_RenderTarget->AcquireColorBuffer();
        if (nullptr != colorBuffer)
        {
            Yw3dResult resBuffer = colorBuffer->LockRect((void**)&m_RenderInfo.frameData, nullptr);
            if (YW3D_FAILED(resBuffer))
            {
                LOGI(_T("Yw3dDevice::PreRender: couldn't access color buffer.\n"));
                YW_SAFE_RELEASE(colorBuffer);

                return resBuffer;
            }

            m_RenderInfo.colorFloats = colorBuffer->GetFormatFloats();
            if (0 == m_RenderInfo.colorFloats)
            {
                colorBuffer->UnlockRect();
                YW_SAFE_RELEASE(colorBuffer);

                return Yw3d_E_Unknown;
            }

            m_RenderInfo.colorBufferPitch = colorBuffer->GetWidth() * m_RenderInfo.colorFloats;
            m_RenderInfo.colorWriteEnabled = m_RenderStates[Yw3d_RS_ColorWriteEnable] ? true : false;
        }
        else
        {
            m_RenderInfo.frameData = nullptr;
            m_RenderInfo.colorFloats = 0;
            m_RenderInfo.colorBufferPitch = 0;
            m_RenderInfo.colorWriteEnabled = false;
        }

        // Get depth buffer related states.
        depthBuffer = m_RenderStates[Yw3d_RS_ZEnable] ? m_RenderTarget->AcquireDepthBuffer() : nullptr;
        if (nullptr != depthBuffer)
        {
            Yw3dResult resBuffer = depthBuffer->LockRect((void**)&m_RenderInfo.depthData, nullptr);
            if (YW3D_FAILED(resBuffer))
            {
                LOGI(_T("Yw3dDevice::PreRender: couldn't access depth buffer.\n"));
                if (nullptr != m_RenderInfo.frameData)
                {
                    colorBuffer->UnlockRect();
                }

                YW_SAFE_RELEASE(colorBuffer);
                YW_SAFE_RELEASE(depthBuffer);

                return resBuffer;
            }

            m_RenderInfo.depthBufferPitch = depthBuffer->GetWidth();
            m_RenderInfo.depthCompare = (Yw3dCompareFunction)m_RenderStates[Yw3d_RS_ZFunc];
            m_RenderInfo.depthWriteEnabled = m_RenderStates[Yw3d_RS_ZWriteEnable] ? true : false;
        }
        else
        {
            m_RenderInfo.depthData = nullptr;
            m_RenderInfo.depthBufferPitch = 0;
            m_RenderInfo.depthCompare = Yw3d_CMP_Always;
            m_RenderInfo.depthWriteEnabled = false;
        }

        YW_SAFE_RELEASE(colorBuffer);
        YW_SAFE_RELEASE(depthBuffer);

        // Reset pixel-counter to 0.
        m_RenderInfo.renderedPixels = 0;

        // Depending on m_PixelShader->GetShaderOutput() chose the appropriate
        // RasterizeScanline-function and assign it to the function pointer.
        switch (m_PixelShader->GetShaderOutput())
        {
        case Yw3d_PSO_ColorOnly:
            m_RenderInfo.fpRasterizeScanline = m_PixelShader->MightKillPixels() ? &Yw3dDevice::RasterizeScanline_ColorOnly_MightKillPixels : &Yw3dDevice::RasterizeScanline_ColorOnly;
            m_RenderInfo.fpDrawPixel = &Yw3dDevice::DrawPixel_ColorOnly;
            break;
        case Yw3d_PSO_ColorDepth:
            m_RenderInfo.fpRasterizeScanline = &Yw3dDevice::RasterizeScanline_ColorDepth;
            m_RenderInfo.fpDrawPixel = &Yw3dDevice::DrawPixel_ColorDepth;
            break;
        default:
            LOGE(_T("Yw3dDevice::PreRender: type of pixel shader is invalid.\n"));
            return Yw3d_E_InvalidState;
        }

        // Initialize shader's pointer to the rendering device,
        // have to do this right before drawing and not at set-time, because a shader
        // may be used with different devices.
        m_VertexShader->SetDevice(this);
        m_PixelShader->SetDevice(this);
        if (nullptr != m_TriangleShader)
        {
            m_TriangleShader->SetDevice(this);
        }

        // Initialize pixel shader's pointers to info structures.
        m_PixelShader->SetInfo(m_RenderInfo.vsOutputRegisterTypes, &m_TriangleInfo);

        // Initialize vertex cache.
        m_NumValidCacheEntries = 0;
        m_FetchedVertices = 0;

        // Make ftol() returns expected integer values.
        fpuTruncate();

        return Yw3d_S_OK;
    }

    void Yw3dDevice::PostRender()
    {
        // Reset FPU to default(rounding) mode.
        fpuReset();

        // Unlock and release color buffer.
        if (nullptr != m_RenderInfo.frameData)
        {
            Yw3dSurface* colorBuffer = m_RenderTarget->AcquireColorBuffer();
            if (nullptr != colorBuffer)
            {
                colorBuffer->UnlockRect();
            }

            YW_SAFE_RELEASE(colorBuffer);
        }

        // Unlock and release depth buffer.
        if (nullptr != m_RenderInfo.depthData)
        {
            Yw3dSurface* depthBuffer = m_RenderTarget->AcquireDepthBuffer();
            if (nullptr != depthBuffer)
            {
                depthBuffer->UnlockRect();
            }

            YW_SAFE_RELEASE(depthBuffer)
        }

        // Clear current device associated with used shaders.
        m_VertexShader->SetDevice(nullptr);
        m_PixelShader->SetDevice(nullptr);
        if (nullptr != m_TriangleShader)
        {
            m_TriangleShader->SetDevice(nullptr);
        }
    }

    Yw3dResult Yw3dDevice::DecodeVertexStream(Yw3dVSInput& vertexShaderInput, uint32_t vertexIndex)
    {
        // Raw vertex data cache for each stream.
        const byte* vertexRawData[YW3D_MAX_VERTEX_STREAMS];
        memset(vertexRawData, 0, sizeof(byte) * YW3D_MAX_VERTEX_STREAMS);

        // Get start vertex index in vertex buffer of each stream.
        const VertexStream* vertexStream = m_VertexStreams;
        for (uint32_t streamIdx = 0; streamIdx <= m_VertexFormat->GetHighestStream(); streamIdx++, vertexStream++)
        {
            uint32_t vertexOffset = vertexStream->offset + vertexIndex * vertexStream->stride;
            if (vertexOffset >= vertexStream->vertexBuffer->GetLength())
            {
                LOGE(_T("Yw3dDevice::DecodeVertexStream: vertex stream offset exceeds vertex buffer length.\n"));
			    return Yw3d_E_Unknown;
            }

            // Get vertex raw data at start index in this stream.
            Yw3dResult resVertex = vertexStream->vertexBuffer->GetPointer(vertexOffset, (void**)&vertexRawData[streamIdx]);
            if (YW3D_FAILED(resVertex))
            {
                return resVertex;
            }
        }

        // Fill vertex-info structure, which can be passed to the vertex shader,
        // with data from the vertex-streams, depending on the current VertexFormat.
        const Yw3dVertexElement* vertexElement = m_VertexFormat->GetElements();
        uint32_t elementCount = m_VertexFormat->GetNumVertexElements();
        while (elementCount-- > 0)
        {
            // The result holder of this shader register.
            Yw3dShaderRegister& shaderRegister = vertexShaderInput.shaderInputs[vertexElement->shaderRegister];
            
            // Convert vertex raw data from byte into float.
            const float* vertexData = (const float*)vertexRawData[vertexElement->stream];

            // Fill this shader register result by register type, and read from vertex data.
            switch (vertexElement->type)
            {
            case Yw3d_VET_Float:
                shaderRegister = Yw3dShaderRegister(vertexData[0], 0.0f, 0.0f, 1.0f);
                vertexRawData[vertexElement->stream] += sizeof(float);
                break;
            case Yw3d_VET_Vector2:
                shaderRegister = Yw3dShaderRegister(vertexData[0], vertexData[1], 0.0f, 1.0f);
                vertexRawData[vertexElement->stream] += sizeof(float) * 2;
                break;
            case Yw3d_VET_Vector3:
                shaderRegister = Yw3dShaderRegister(vertexData[0], vertexData[1], vertexData[2], 1.0f);
                vertexRawData[vertexElement->stream] += sizeof(float) * 3;
                break;
            case Yw3d_VET_Vector4:
                shaderRegister = Yw3dShaderRegister(vertexData[0], vertexData[1], vertexData[2], vertexData[3]);
                vertexRawData[vertexElement->stream] += sizeof(float) * 4;
                break;
            default:
                // Can not happen.
                break;
            }

            // Get next vertex element format.
            vertexElement++;
        }

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::FetchVertex(Yw3dVertexCacheEntry** vertexCacheEntry, uint32_t vertexIndex)
    {
        if ((nullptr != *vertexCacheEntry) && ((*vertexCacheEntry)->vertexIndex == vertexIndex))
        {
            (*vertexCacheEntry)->fetchTime = m_FetchedVertices++;
            return Yw3d_S_OK;
        }

        // Check if we can append more vertex.
        const bool canAppend = (m_NumValidCacheEntries < YW3D_VERTEX_CACHE_SIZE);

        // Find vertex in cache.
        Yw3dVertexCacheEntry* cacheEntry = &m_VertexCache[0];
        Yw3dVertexCacheEntry* destEntry = cacheEntry;
        for (uint32_t i = 0; i < m_NumValidCacheEntries; i++, cacheEntry++)
        {
            if (cacheEntry->vertexIndex == vertexIndex)
            {
                // Vertex is already in cache, return it.
                cacheEntry->fetchTime = m_FetchedVertices++;
                *vertexCacheEntry = cacheEntry;

                return Yw3d_S_OK;
            }

            // if (!canAppend) // Profiling turned out, that leaving this away is faster because most of the time we can't append anyway.
            {
                // The vertex cache is full, so we have to look for the oldest entry and replace it in case we cannot find the desired vertex in the cache.
                if (cacheEntry->fetchTime < destEntry->fetchTime)
                {
                    destEntry = cacheEntry;
                }
            }
        }

        // The cache is not full yet, so we can append a new vertex.
        if (canAppend)
        {
            destEntry = &m_VertexCache[m_NumValidCacheEntries++];
        }

        // Update the destination cache entry and return it
        destEntry->vertexIndex = vertexIndex;
        destEntry->fetchTime = m_FetchedVertices++;

        // Decode this vertex data from stream.
        Yw3dResult resDecode = DecodeVertexStream(destEntry->vertexOutput.sourceInput, vertexIndex);
        if (YW3D_FAILED(resDecode))
        {
            return resDecode;
        }

        // Execute vertex shader.
        m_VertexShader->Execute(destEntry->vertexOutput.sourceInput.shaderInputs, destEntry->vertexOutput.position, destEntry->vertexOutput.shaderOutputs);

        // Fill result.
        *vertexCacheEntry = destEntry;

        return Yw3d_S_OK;
    }

    void Yw3dDevice::ProcessTriangle(const Yw3dVSOutput* vsOutput0, const Yw3dVSOutput* vsOutput1, const Yw3dVSOutput* vsOutput2)
    {
        switch (m_RenderStates[Yw3d_RS_SubdivisionMode])
        {
        case Yw3d_Subdivision_None:
            DrawTriangle(vsOutput0, vsOutput1, vsOutput2);
            break;
        case Yw3d_Subdivision_Simple:
            SubdivideTriangle_Simple(0, vsOutput0, vsOutput1, vsOutput2);
            break;
        case Yw3d_Subdivision_Smooth:
            SubdivideTriangle_Smooth(0, vsOutput0, vsOutput1, vsOutput2);
            break;
        case Yw3d_Subdivision_Adaptive:
            SubdivideTriangle_Adaptive(vsOutput0, vsOutput1, vsOutput2);
            break;
        default:
            break;  // This can not happen.
        }
    }

    void Yw3dDevice::InterpolateVertexShaderInput(Yw3dVSInput* vsInput, const Yw3dVSInput* vsInputA, const Yw3dVSInput* vsInputB, float interpolation)
    {
        // Interpolate registers.
        Yw3dShaderRegister* regO = vsInput->shaderInputs;
        const Yw3dShaderRegister* regA = vsInput->shaderInputs;
        const Yw3dShaderRegister* regB = vsInput->shaderInputs;

        for (uint32_t regIdx = 0; regIdx < YW3D_VERTEX_SHADER_REGISTERS; regIdx++, regO++, regA++, regB++)
        {
            switch (m_RenderInfo.vsInputRegisterTypes[regIdx])
            {
            case Yw3d_SRT_Float32:
                regO->x = Lerp(regA->x, regB->x, interpolation);
                regO->y = 0.0f;
                regO->z = 0.0f;
                regO->w = 1.0f;
                break;
            case Yw3d_SRT_Vector2:
                regO->x = Lerp(regA->x, regB->x, interpolation);
                regO->y = Lerp(regA->y, regB->y, interpolation);
                regO->z = 0.0f;
                regO->w = 1.0f;
                break;
            case Yw3d_SRT_Vector3:
                regO->x = Lerp(regA->x, regB->x, interpolation);
                regO->y = Lerp(regA->y, regB->y, interpolation);
                regO->z = Lerp(regA->z, regB->z, interpolation);
                regO->w = 1.0f;
                break;
            case Yw3d_SRT_Vector4:
                Vector4Lerp(*regO, *regA, *regB, interpolation);
                break;
            case Yw3d_SRT_Unused:
            default:    // This can not happen.
                break;
            }
        }
    }

    void Yw3dDevice::InterpolateVertexShaderOutput(Yw3dVSOutput* vsOutput, const Yw3dVSOutput* vsOutputA, const Yw3dVSOutput* vsOutputB, float interpolation)
    {
        // Interpolate vertex position.
        Vector4Lerp(vsOutput->position, vsOutputA->position, vsOutputB->position, interpolation);

        // Interpolate registers.
        Yw3dShaderRegister* regO = vsOutput->shaderOutputs;
        const Yw3dShaderRegister* regA = vsOutputA->shaderOutputs;
        const Yw3dShaderRegister* regB = vsOutputB->shaderOutputs;

        for (uint32_t regIdx = 0; regIdx < YW3D_PIXEL_SHADER_REGISTERS; regIdx++, regO++, regA++, regB++)
        {
            switch (m_RenderInfo.vsOutputRegisterTypes[regIdx])
            {
            case Yw3d_SRT_Vector4:
                regO->w = Lerp(regA->w, regB->w, interpolation);
            case Yw3d_SRT_Vector3:
                regO->z = Lerp(regA->z, regB->z, interpolation);
            case Yw3d_SRT_Vector2:
                regO->y = Lerp(regA->y, regB->y, interpolation);
            case Yw3d_SRT_Float32:
                regO->x = Lerp(regA->x, regB->x, interpolation);
            case Yw3d_SRT_Unused:
            default:    // This can not happen.
                break;
            }
        }
    }

    void Yw3dDevice::MultiplyVertexShaderOutputRegisters(Yw3dVSOutput* dest, const Yw3dVSOutput* src, float value)
    {
        // Multiply registers.
        Yw3dShaderRegister* regDest = dest->shaderOutputs;
        const Yw3dShaderRegister* regSrc = src->shaderOutputs;

        for (uint32_t regIdx = 0; regIdx < YW3D_PIXEL_SHADER_REGISTERS; regIdx++, regDest++, regSrc++)
        {
            switch (m_RenderInfo.vsOutputRegisterTypes[regIdx])
            {
            case Yw3d_SRT_Vector4:
                regDest->w = regSrc->w * value;
            case Yw3d_SRT_Vector3:
                regDest->z = regSrc->z * value;
            case Yw3d_SRT_Vector2:
                regDest->y = regSrc->y * value;
            case Yw3d_SRT_Float32:
                regDest->x = regSrc->x * value;
            case Yw3d_SRT_Unused:
            default:    // Can not happen.
                break;
            }
        }
    }

    void Yw3dDevice::SubdivideTriangle_Simple(uint32_t subdivisionLevel, const Yw3dVSOutput* vsOutput0, const Yw3dVSOutput* vsOutput1, const Yw3dVSOutput* vsOutput2)
    {
        // In case the triangle has been subdivided to the requested level, draw it ...
        if (subdivisionLevel >= m_RenderStates[Yw3d_RS_SubdivisionLevels])
        {
            DrawTriangle(vsOutput0, vsOutput1, vsOutput2);
            return;
        }

        subdivisionLevel++;

        // Generate three new vertices: in the middle of each edge
        // Interpolate inputs for the new vertices (we're splitting the triangle's edges)
        Yw3dVSOutput newVsOutputs[3];
        InterpolateVertexShaderInput(&newVsOutputs[0].sourceInput, &vsOutput0->sourceInput, &vsOutput1->sourceInput, 0.5f); // Edge between v0 and v1.
        InterpolateVertexShaderInput(&newVsOutputs[1].sourceInput, &vsOutput1->sourceInput, &vsOutput2->sourceInput, 0.5f); // Edge between v1 and v2.
        InterpolateVertexShaderInput(&newVsOutputs[2].sourceInput, &vsOutput2->sourceInput, &vsOutput0->sourceInput, 0.5f); // Edge between v2 and v0.

        // Calculate new vertex shader outputs.
        Yw3dVSOutput* curVsOutput = newVsOutputs;
        for (uint32_t i = 0; i < 3; i++, curVsOutput++)
        {
            m_VertexShader->Execute(curVsOutput->sourceInput.shaderInputs, curVsOutput->position, curVsOutput->shaderOutputs);
        }

        // Go on subdividing new triangles.
        SubdivideTriangle_Simple(subdivisionLevel, vsOutput0, &newVsOutputs[0], &newVsOutputs[2]);
        SubdivideTriangle_Simple(subdivisionLevel, vsOutput1, &newVsOutputs[1], &newVsOutputs[0]);
        SubdivideTriangle_Simple(subdivisionLevel, vsOutput2, &newVsOutputs[2], &newVsOutputs[1]);
        SubdivideTriangle_Simple(subdivisionLevel, &newVsOutputs[0], &newVsOutputs[1], &newVsOutputs[2]);
    }

    void Yw3dDevice::SubdivideTriangle_Smooth(uint32_t subdivisionLevel, const Yw3dVSOutput* vsOutput0, const Yw3dVSOutput* vsOutput1, const Yw3dVSOutput* vsOutput2)
    {
        // In case the triangle has been subdivided to the requested level, draw it ...
        if (subdivisionLevel >= m_RenderStates[Yw3d_RS_SubdivisionLevels])
        {
            DrawTriangle(vsOutput0, vsOutput1, vsOutput2);
            return;
        }

        subdivisionLevel++;

        // Generate three new vertices: in the middle of each edge
        // Interpolate inputs for the new vertices (we're splitting the triangle's edges)
        Yw3dVSOutput newVsOutputs[3];
        InterpolateVertexShaderInput(&newVsOutputs[0].sourceInput, &vsOutput0->sourceInput, &vsOutput1->sourceInput, 0.5f); // Edge between v0 and v1.
        InterpolateVertexShaderInput(&newVsOutputs[1].sourceInput, &vsOutput1->sourceInput, &vsOutput2->sourceInput, 0.5f); // Edge between v1 and v2.
        InterpolateVertexShaderInput(&newVsOutputs[2].sourceInput, &vsOutput2->sourceInput, &vsOutput0->sourceInput, 0.5f); // Edge between v2 and v0.

        // Divide offset param.
        static const float s_MultDivideBySix = 1.0f / 6.0f;

        // Offset positions using normals as a base.
        const uint32_t posIdx = m_RenderStates[Yw3d_RS_SubdivisionPositionRegister];
        const uint32_t normalIdx = m_RenderStates[Yw3d_RS_SubdivisionNormalRegister];

        // Normal-vectors should be re-normalized (they're not unit-length anymore due
        // to linear-interpolation) for best results, but because the error is very small
        // this step is skipped.

        const Yw3dShaderRegister* shaderInputs[3] = { vsOutput0->sourceInput.shaderInputs, vsOutput1->sourceInput.shaderInputs, vsOutput2->sourceInput.shaderInputs };

        // offset middle of edge between v0 and v1.
        {
            const Vector3 normalA = shaderInputs[0][normalIdx] * Vector3Dot((Vector3)shaderInputs[1][posIdx] - (Vector3)shaderInputs[0][posIdx], shaderInputs[0][normalIdx]);
            const Vector3 normalB = shaderInputs[1][normalIdx] * Vector3Dot((Vector3)shaderInputs[0][posIdx] - (Vector3)shaderInputs[1][posIdx], shaderInputs[1][normalIdx]);
            Vector4& pos = newVsOutputs[0].sourceInput.shaderInputs[posIdx];
            pos -= (normalA + normalB) * s_MultDivideBySix;
        }

        // offset middle of edge between v1 and v2
        {
            const Vector3 normalA = shaderInputs[1][normalIdx] * Vector3Dot((Vector3)shaderInputs[2][posIdx] - (Vector3)shaderInputs[1][posIdx], shaderInputs[1][normalIdx]);
            const Vector3 normalB = shaderInputs[2][normalIdx] * Vector3Dot((Vector3)shaderInputs[1][posIdx] - (Vector3)shaderInputs[2][posIdx], shaderInputs[2][normalIdx]);
            Vector4& pos = newVsOutputs[1].sourceInput.shaderInputs[posIdx];
            pos -= (normalA + normalB) * s_MultDivideBySix;
        }

        // offset middle of edge between v2 and v0.
        {
            const Vector3 normalA = shaderInputs[2][normalIdx] * Vector3Dot((Vector3)shaderInputs[0][posIdx] - (Vector3)shaderInputs[2][posIdx], shaderInputs[2][normalIdx]);
            const Vector3 normalB = shaderInputs[0][normalIdx] * Vector3Dot((Vector3)shaderInputs[2][posIdx] - (Vector3)shaderInputs[0][posIdx], shaderInputs[0][normalIdx]);
            Vector4& pos = newVsOutputs[2].sourceInput.shaderInputs[posIdx];
            pos -= (normalA + normalB) * s_MultDivideBySix;
        }

        // Calculate new vertex shader outputs.
        Yw3dVSOutput* curVsOutput = newVsOutputs;
        for (uint32_t i = 0; i < 3; i++, curVsOutput++)
        {
            m_VertexShader->Execute(curVsOutput->sourceInput.shaderInputs, curVsOutput->position, curVsOutput->shaderOutputs);
        }

        // Go on subdividing new triangles.
        SubdivideTriangle_Simple(subdivisionLevel, vsOutput0, &newVsOutputs[0], &newVsOutputs[2]);
        SubdivideTriangle_Simple(subdivisionLevel, vsOutput1, &newVsOutputs[1], &newVsOutputs[0]);
        SubdivideTriangle_Simple(subdivisionLevel, vsOutput2, &newVsOutputs[2], &newVsOutputs[1]);
        SubdivideTriangle_Simple(subdivisionLevel, &newVsOutputs[0], &newVsOutputs[1], &newVsOutputs[2]);
    }

    void Yw3dDevice::SubdivideTriangle_Adaptive(const Yw3dVSOutput* vsOutput0, const Yw3dVSOutput* vsOutput1, const Yw3dVSOutput* vsOutput2)
    {
        // Define divede param.
        static const float s_MultDivideByThree = 1.0f / 3.0f;

        // Average inputs for the center vertex.
        const Yw3dShaderRegister* shaderInputs[3] = { vsOutput0->sourceInput.shaderInputs, vsOutput1->sourceInput.shaderInputs, vsOutput2->sourceInput.shaderInputs };

        // Get the center shader input of the three vertex.
        Yw3dVSOutput vsOutputCenter;
        for (uint32_t i = 0; i < YW3D_VERTEX_SHADER_REGISTERS; i++)
        {
            vsOutputCenter.sourceInput.shaderInputs[i] = (shaderInputs[0][i] + shaderInputs[1][i] + shaderInputs[2][i]) * s_MultDivideByThree;
        }

        // Call vertex shader.
        m_VertexShader->Execute(vsOutputCenter.sourceInput.shaderInputs, vsOutputCenter.position, vsOutputCenter.shaderOutputs);

        // Split outer triangle-edges.
        SubdivideTriangle_Adaptive_SubdivideEdges(0, vsOutput0, vsOutput1, &vsOutputCenter);
        SubdivideTriangle_Adaptive_SubdivideEdges(0, vsOutput1, vsOutput2, &vsOutputCenter);
        SubdivideTriangle_Adaptive_SubdivideEdges(0, vsOutput2, vsOutput0, &vsOutputCenter);
    }

    void Yw3dDevice::SubdivideTriangle_Adaptive_SubdivideEdges(uint32_t subdivisionLevel, const Yw3dVSOutput* vsOutputEdge0, const Yw3dVSOutput* vsOutputEdge1, const Yw3dVSOutput* vsOutputCenter)
    {
        // In case the triangle-edges have been subdivided to the requested level, begin adaptive-subdivision of inner part.
        if (subdivisionLevel >= m_RenderStates[Yw3d_RS_SubdivisionLevels])
        {
            SubdivideTriangle_Adaptive_SubdivideInnerPart(0, vsOutputEdge0, vsOutputEdge1, vsOutputCenter);
            return;
        }

        subdivisionLevel++;

        // Split edge and call subdivideedges recursively.
        Yw3dVSOutput vsOutputMiddleEdge;
        InterpolateVertexShaderInput(&vsOutputMiddleEdge.sourceInput, &vsOutputEdge0->sourceInput, &vsOutputEdge1->sourceInput, 0.5f);

        // Call vertex shader.
        m_VertexShader->Execute(vsOutputMiddleEdge.sourceInput.shaderInputs, vsOutputMiddleEdge.position, vsOutputMiddleEdge.shaderOutputs);

        // Go on to subdivide.
        SubdivideTriangle_Adaptive_SubdivideEdges(subdivisionLevel, vsOutputEdge0, &vsOutputMiddleEdge, vsOutputCenter);
        SubdivideTriangle_Adaptive_SubdivideEdges(subdivisionLevel, &vsOutputMiddleEdge, vsOutputEdge1, vsOutputCenter);
    }

    void Yw3dDevice::SubdivideTriangle_Adaptive_SubdivideInnerPart(uint32_t subdivisionLevel, const Yw3dVSOutput* vsOutput0, const Yw3dVSOutput* vsOutput1, const Yw3dVSOutput* vsOutput2)
    {
        static const float s_MultDivideByThree = 1.0f / 3.0f;

        // Info about subdivisionLevel: here we are counting the maximum inner subdivisions.
        if (subdivisionLevel >= m_RenderStates[Yw3d_RS_SubdivisionMaxInnerLevels])
        {
            DrawTriangle(vsOutput0, vsOutput1, vsOutput2);
            return;
        }

        // Check area of triangle in screen space.
        {
            Vector4 pos[3] = { vsOutput0->position, vsOutput1->position, vsOutput2->position };
            for (uint32_t vertexIdx = 0; vertexIdx < 3; vertexIdx++)
            {
                // TODO: should actually be clipped to view frustum.

                // Project vertex position + scale to rendertarget's viewport.
                pos[vertexIdx].Homogenize();
                pos[vertexIdx] *= m_ViewportMatrix;
            }

            const Vector3 v0To1 = (Vector3)pos[1] - (Vector3)pos[0];
            const Vector3 v0T02 = (Vector3)pos[2] - (Vector3)pos[0];

            // Area of triangle = 0.5*|a|*|b|*sina = 0.5*|a cross b|.
            Vector3 vNormal;
            Vector3Cross(vNormal, v0To1, v0T02);

            // Get actual area of this triangle.
            const float triangleArea = 0.5f * vNormal.Length();

            if (triangleArea < *(float*)&m_RenderStates[Yw3d_RS_SubdivisionMaxScreenArea])
            {
                DrawTriangle(vsOutput0, vsOutput1, vsOutput2);
                return;
            }
        }

        // Continue splitting: find center vertex and call SubdivideInnerPart for the three new vertices.
        subdivisionLevel++;

        // Average inputs for the center vertex.
        const Yw3dShaderRegister* shaderInputs[3] = { vsOutput0->sourceInput.shaderInputs, vsOutput1->sourceInput.shaderInputs, vsOutput2->sourceInput.shaderInputs };

        // Get the center shader input of the three vertex.
        Yw3dVSOutput vsOutputCenter;
        for (uint32_t i = 0; i < YW3D_VERTEX_SHADER_REGISTERS; i++)
        {
            vsOutputCenter.sourceInput.shaderInputs[i] = (shaderInputs[0][i] + shaderInputs[1][i] + shaderInputs[2][i]) * s_MultDivideByThree;
        }

        // Call vertex shader.
        m_VertexShader->Execute(vsOutputCenter.sourceInput.shaderInputs, vsOutputCenter.position, vsOutputCenter.shaderOutputs);

        // Split outer triangle-edges.
        SubdivideTriangle_Adaptive_SubdivideInnerPart(subdivisionLevel, vsOutput0, vsOutput1, &vsOutputCenter);
        SubdivideTriangle_Adaptive_SubdivideInnerPart(subdivisionLevel, vsOutput1, vsOutput2, &vsOutputCenter);
        SubdivideTriangle_Adaptive_SubdivideInnerPart(subdivisionLevel, vsOutput2, vsOutput0, &vsOutputCenter);
    }

    void Yw3dDevice::DrawTriangle(const Yw3dVSOutput* vsOutput0, const Yw3dVSOutput* vsOutput1, const Yw3dVSOutput* vsOutput2)
    {
        // Prepare triangle for homogenous clipping.
        uint32_t numVertices = 3;
        memcpy(&m_ClipVertices[0], vsOutput0, sizeof(Yw3dVSOutput));
        memcpy(&m_ClipVertices[1], vsOutput1, sizeof(Yw3dVSOutput));
        memcpy(&m_ClipVertices[2], vsOutput2, sizeof(Yw3dVSOutput));
        m_NextFreeClipVertex = 3;

        uint32_t stage = 0;
        m_ClipVerticesStages[stage][0] = &m_ClipVertices[0];
        m_ClipVerticesStages[stage][1] = &m_ClipVertices[1];
        m_ClipVerticesStages[stage][2] = &m_ClipVertices[2];

        // Execute the triangle shader.
        if (nullptr != m_TriangleShader)
        {
            if (!m_TriangleShader->Execute(m_ClipVerticesStages[stage][0]->shaderOutputs, m_ClipVerticesStages[stage][1]->shaderOutputs, m_ClipVerticesStages[stage][2]->shaderOutputs))
            {
                // Triangle got rejected.
                return;
            }
        }

        // Perform clipping to the frustum planes.
        for (uint32_t planeIdx = 0; planeIdx < Yw3d_CP_NumPlanes; planeIdx++)
        {
            if (!m_RenderInfo.clippingPlaneEnabled[planeIdx])
            {
                continue;
            }

            numVertices = ClipToPlane(numVertices, stage, m_RenderInfo.clippingPlanes[planeIdx], true);
            if (numVertices < 3)
            {
                // Exception!
                return;
            }

            stage = (stage + 1) & 1;
        }

        // Project and rasterize the clipped triangle.
        uint32_t vertexIdx = 0;

        // Project the first three vertices for back face culling.
        Yw3dVSOutput** srcVsOutput = m_ClipVerticesStages[stage];
        for (vertexIdx = 0; vertexIdx < 3; vertexIdx++)
        {
            ProjectVertex(srcVsOutput[vertexIdx]);
        }

        // We do not have to check for back face culling for each sub-polygon of the triangle, as they
        // are all in the same plane. If the first polygon is back face culled then all other polygons
        // would be culled, too.
        if (CullTriangle(srcVsOutput[0], srcVsOutput[1], srcVsOutput[2]))
        {
            return;
        }

        // Project the remaining vertices.
        for (vertexIdx = 3; vertexIdx < numVertices; vertexIdx++)
        {
            ProjectVertex(srcVsOutput[vertexIdx]);
        }

        // Perform clipping (in screenspace) to the scissor rectangle if enabled.
        if (m_RenderStates[Yw3d_RS_ScissorTestEnable])
        {
            for (uint32_t planeIdx = 0; planeIdx < 4; planeIdx++, stage = (stage + 1) & 1)
            {
                numVertices = ClipToPlane(numVertices, stage, m_RenderInfo.scissorPlanes[planeIdx], false);
                if (numVertices < 3)
                {
                    // Exception.
                    return;
                }

                // New source for rasterization after scissoring ...
                srcVsOutput = m_ClipVerticesStages[stage];
            }
        }

        // Subdivide this polygon into triangle and rasterize them by each.
        for (vertexIdx = 1; vertexIdx < numVertices - 1; vertexIdx++)
        {
            RasterizeTriangle(srcVsOutput[0], srcVsOutput[vertexIdx], srcVsOutput[vertexIdx + 1]);
        }
    }

    uint32_t Yw3dDevice::ClipToPlane(uint32_t numVertices, uint32_t stage, const Plane &plane, bool homogenous)
    {
        Yw3dVSOutput** srcVertices = m_ClipVerticesStages[stage];
        Yw3dVSOutput** dstVertices = m_ClipVerticesStages[(stage + 1) & 1];

        uint32_t numClippedVertices = 0;
        for (uint32_t i = 0, j = 1; i < numVertices; i++, j++)
        {
            // Wrap over.
            if (j == numVertices)
            {
                j = 0;
            }

            // The signed distances of current and next vertex to clipping plane.
            float di = 0;
            float dj = 0;
            if (homogenous)
            {
                di = plane * srcVertices[i]->position;
                dj = plane * srcVertices[j]->position;
            }
            else
            {
                di = plane * (*(Vector3*)&srcVertices[i]->position);
                dj = plane * (*(Vector3*)&srcVertices[j]->position);
            }

            if (di >= 0.0f)
            {
                // First vertex inside plane, output it.
                dstVertices[numClippedVertices++] = srcVertices[i];

                if (dj < 0.0f)
                {
                    // Second vertex outside plan, calculate the intersect point, including interpolate vertex attribute and shader output.
                    InterpolateVertexShaderOutput(&m_ClipVertices[m_NextFreeClipVertex], srcVertices[i], srcVertices[j], di / (di - dj));
                    dstVertices[numClippedVertices++] = &m_ClipVertices[m_NextFreeClipVertex];
                    m_NextFreeClipVertex = (m_NextFreeClipVertex + 1) % YW3D_CLIP_VERTEX_CACHE_SIZE;

#ifdef _DEBUG
                    if (0 == m_NextFreeClipVertex)
                    {
                        LOGE(_T("Yw3dDevice::ClipToPlane: Wrap over, vertex creation during clipping! array too small ...?\n"));
                    }
#endif
                }
            }
            else
            {
                // First vertex outside plane.
                if (dj >= 0)
                {
                    // First vertex outside plane, second vertex inside plane, calculate the intersect point, including interpolate vertex attribute and shader output.
                    InterpolateVertexShaderOutput(&m_ClipVertices[m_NextFreeClipVertex], srcVertices[j], srcVertices[i], dj / (dj - di));
                    dstVertices[numClippedVertices++] = &m_ClipVertices[m_NextFreeClipVertex];
                    m_NextFreeClipVertex = (m_NextFreeClipVertex + 1) % YW3D_CLIP_VERTEX_CACHE_SIZE;

#ifdef _DEBUG
                    if (0 == m_NextFreeClipVertex)
                    {
                        LOGE(_T("Yw3dDevice::ClipToPlane: Wrap over, vertex creation during clipping! array too small ...?\n"));
                    }
#endif
                }
            }
        }

        return numClippedVertices;
    }

    bool Yw3dDevice::CullTriangle(const Yw3dVSOutput* vsOutput0, const Yw3dVSOutput* vsOutput1, const Yw3dVSOutput* vsOutput2)
    {
        // Do backface-culling.
        if (Yw3d_Cull_None == m_RenderStates[Yw3d_RS_CullMode])
        {
            return false;
        }

        // Vector3 vAB = vScreenSpacePos[1] - vScreenSpacePos[0];
        // Vector3 vAC = vScreenSpacePos[2] - vScreenSpacePos[0];
        // Vector3 vFaceNormal; Vector3Cross(vFaceNormal, vAB, vAC);
        // float dirTest = Vector3Dot(vFaceNormal, Vector3(0.0f, 0.0f, 1.0f));
        const float dirTest = (vsOutput1->position.x - vsOutput0->position.x) * (vsOutput2->position.y - vsOutput0->position.y) - (vsOutput1->position.y - vsOutput0->position.y) * (vsOutput2->position.x - vsOutput0->position.x);
        if (Yw3d_Cull_CCW == m_RenderStates[Yw3d_RS_CullMode])
        {
            // Counterclockwise vertices.
            if (dirTest <= 0.0f)
            {
                return true;
            }
        }
        else
        {   
            // Clockwise vertices.
            if (dirTest >= 0.0f)
            {
                return true;
            }
        }

        return false;
    }

    void Yw3dDevice::ProjectVertex(Yw3dVSOutput* vsOutput)
    {
        if (vsOutput->position.w < YW_FLOAT_PRECISION)
        {
            return;
        }

        // Project vertex into homogenous space.
        const float invW = 1.0f / vsOutput->position.w;
        vsOutput->position.x *= invW;
        vsOutput->position.y *= invW;
        vsOutput->position.z *= invW;
        vsOutput->position.w = 1.0f;

        // Project vertex into screen space.
        vsOutput->position *= m_ViewportMatrix;

        // Divide shader output registers by w(1/z); this way we can interpolate them linearly while rasterizing ...
        vsOutput->position.w = invW;
        MultiplyVertexShaderOutputRegisters(vsOutput, vsOutput, invW);
    }

    void Yw3dDevice::CalculateTriangleGradients(const Yw3dVSOutput* vsOutput0, const Yw3dVSOutput* vsOutput1, const Yw3dVSOutput* vsOutput2)
    {
        // References:
        // Physically Based Rendering, Third Edition : Page 158 - Compute triangle partial derivatives.
        // Partial derivatives, Directional derivatives, Gradient.
        // pi = p0 + (∂p/∂u)*ui + (∂p/∂v)*vi.

        // Position of each vertex.
        const Vector4& position0 = vsOutput0->position;
        const Vector4& position1 = vsOutput1->position;
        const Vector4& position2 = vsOutput2->position;

        // Delta x and delta y in screen-space.
        const float deltaX[2] = { position1.x - position0.x, position2.x - position0.x };
        const float deltaY[2] = { position1.y - position0.y, position2.y - position0.y };
        const float deltaZ[2] = { position1.z - position0.z, position2.z - position0.z };
        const float deltaW[2] = { position1.w - position0.w, position2.w - position0.w };

        // The "one over determinant" of matrix for calculating partial derivatives.
        const float oneOverDeterminant = 1.0f / (deltaX[0] * deltaY[1] - deltaX[1] * deltaY[0]);

        // Set base vertex.
        m_TriangleInfo.baseVertex = vsOutput0;

        // The derivatives with respect to the y-coordinate are negated, because in screen-space the y-axis is reversed.

        // Get z partial derivatives with respect to the screen-space x and y coordinate.
        m_TriangleInfo.zDdx = (deltaY[1] * deltaZ[0] - deltaY[0] * deltaZ[1]) * oneOverDeterminant;
        m_TriangleInfo.zDdy = -(deltaX[1] * deltaZ[0] - deltaX[0] * deltaZ[1]) * oneOverDeterminant;

        // Get w partial derivatives with respect to the screen-space x and y coordinate.
        m_TriangleInfo.zDdx = (deltaY[1] * deltaW[0] - deltaY[0] * deltaW[1]) * oneOverDeterminant;
        m_TriangleInfo.zDdy = -(deltaX[1] * deltaW[0] - deltaX[0] * deltaW[1]) * oneOverDeterminant;

        // Calculate shader register partial derivatives with respect to the screen-space x and y coordinate.
        Yw3dShaderRegister* destDdx = m_TriangleInfo.shaderOutputsDdx;
        Yw3dShaderRegister* destDdy = m_TriangleInfo.shaderOutputsDdy;
        for (uint32_t regIdx = 0; regIdx < YW3D_PIXEL_SHADER_REGISTERS; regIdx++, destDdx++, destDdy++)
        {
            switch (m_RenderInfo.vsOutputRegisterTypes[regIdx])
            {
            case Yw3d_SRT_Vector4:
                {
                    const float deltaRegValue[2] = { vsOutput1->shaderOutputs[regIdx].w - vsOutput0->shaderOutputs[regIdx].w, vsOutput2->shaderOutputs[regIdx].w - vsOutput0->shaderOutputs[regIdx].w };
                    destDdx->w = (deltaY[1] * deltaRegValue[0] - deltaY[0] * deltaRegValue[1]) * oneOverDeterminant;
                    destDdy->w = -(deltaX[1] * deltaRegValue[0] - deltaX[0] * deltaRegValue[1]) * oneOverDeterminant;
                }
            case Yw3d_SRT_Vector3:
                {
                    const float deltaRegValue[2] = { vsOutput1->shaderOutputs[regIdx].z - vsOutput0->shaderOutputs[regIdx].z, vsOutput2->shaderOutputs[regIdx].z - vsOutput0->shaderOutputs[regIdx].z };
                    destDdx->z = (deltaY[1] * deltaRegValue[0] - deltaY[0] * deltaRegValue[1]) * oneOverDeterminant;
                    destDdy->z = -(deltaX[1] * deltaRegValue[0] - deltaX[0] * deltaRegValue[1]) * oneOverDeterminant;
                }
            case Yw3d_SRT_Vector2:
                {
                    const float deltaRegValue[2] = { vsOutput1->shaderOutputs[regIdx].y - vsOutput0->shaderOutputs[regIdx].y, vsOutput2->shaderOutputs[regIdx].y - vsOutput0->shaderOutputs[regIdx].y };
                    destDdx->y = (deltaY[1] * deltaRegValue[0] - deltaY[0] * deltaRegValue[1]) * oneOverDeterminant;
                    destDdy->y = -(deltaX[1] * deltaRegValue[0] - deltaX[0] * deltaRegValue[1]) * oneOverDeterminant;
                }
            case Yw3d_SRT_Float32:
                {
                    const float deltaRegValue[2] = { vsOutput1->shaderOutputs[regIdx].x - vsOutput0->shaderOutputs[regIdx].x, vsOutput2->shaderOutputs[regIdx].x - vsOutput0->shaderOutputs[regIdx].x };
                    destDdx->x = (deltaY[1] * deltaRegValue[0] - deltaY[0] * deltaRegValue[1]) * oneOverDeterminant;
                    destDdy->x = -(deltaX[1] * deltaRegValue[0] - deltaX[0] * deltaRegValue[1]) * oneOverDeterminant;
                }
            case Yw3d_SRT_Unused:
            default:    // Cannot happen.
                break;
            }
        }
    }

    void Yw3dDevice::SetVSOutputFromGradient(Yw3dVSOutput* vsOutput, float x, float y)
    {
        // References:
        // pi = p0 + (∂p/∂u)*ui + (∂p/∂v)*vi.

        // Get offset x and y from base vertex in screen space.
        const float offsetX = x - m_TriangleInfo.baseVertex->position.x;
        const float offsetY = y - m_TriangleInfo.baseVertex->position.y;

        // Get position z and w value from partial derivatives by delta x and delta y.
        vsOutput->position.z = m_TriangleInfo.baseVertex->position.z + m_TriangleInfo.zDdx * offsetX + m_TriangleInfo.zDdy * offsetY;
        vsOutput->position.w = m_TriangleInfo.baseVertex->position.w + m_TriangleInfo.wDdx * offsetX + m_TriangleInfo.wDdy * offsetY;

        // Get shader register from partial derivatives by delta x and delta y.
        Yw3dShaderRegister* regDest = vsOutput->shaderOutputs;
        const Yw3dShaderRegister* regBase = m_TriangleInfo.baseVertex->shaderOutputs;
        const Yw3dShaderRegister* regDdx = m_TriangleInfo.shaderOutputsDdx;
        const Yw3dShaderRegister* regDdy = m_TriangleInfo.shaderOutputsDdy;

        for (uint32_t regIdx = 0; regIdx < YW3D_PIXEL_SHADER_REGISTERS; regIdx++, regDest++, regBase++, regDdx++, regDdy++)
        {
            switch (m_RenderInfo.vsOutputRegisterTypes[regIdx])
            {
            case Yw3d_SRT_Float32:
                *regDest = *(float*)regBase + *(float*)regDdx * offsetX + *(float*)regDdy * offsetY;
                break;
            case Yw3d_SRT_Vector2:
                *regDest = *(Vector2*)regBase + *(Vector2*)regDdx * offsetX + *(Vector2*)regDdy * offsetY;
                break;
            case Yw3d_SRT_Vector3:
                *regDest = *(Vector3*)regBase + *(Vector3*)regDdx * offsetX + *(Vector3*)regDdy * offsetY;
                break;
            case Yw3d_SRT_Vector4:
                *regDest = *(Vector4*)regBase + *(Vector4*)regDdx * offsetX + *(Vector4*)regDdy * offsetY;
                break;
            case Yw3d_SRT_Unused:
            default:    // This can not happen.
                break;
            }
        }
    }

    inline void Yw3dDevice::StepXVSOutputFromGradient(Yw3dVSOutput* vsOutput)
    {
        // Use partial derivatives with respect to the screen-space x-coordinate.
        // This is the increment of value each time when increasing 1 pixel value in x-coordinate.

        // Get the value of z and w.
        vsOutput->position.z += m_TriangleInfo.zDdx;
        vsOutput->position.w += m_TriangleInfo.wDdx;

        // Get the value of each shader register.
        Yw3dShaderRegister* regDest = vsOutput->shaderOutputs;
        Yw3dShaderRegister* regDdx = m_TriangleInfo.shaderOutputsDdx;

        for (uint32_t regIdx = 0; regIdx < YW3D_PIXEL_SHADER_REGISTERS; regIdx++)
        {
            switch (m_RenderInfo.vsOutputRegisterTypes[regIdx])
            {
            case Yw3d_SRT_Vector4:
                regDest->w += regDdx->w;
            case Yw3d_SRT_Vector3:
                regDest->z += regDdx->z;
            case Yw3d_SRT_Vector2:
                regDest->y += regDdx->y;
            case Yw3d_SRT_Float32:
                regDest->x += regDdx->x;
            case Yw3d_SRT_Unused:
            default:    // Can not happen.
                break;
            }
        }
    }

    void Yw3dDevice::RasterizeTriangle(const Yw3dVSOutput* vsOutput0, const Yw3dVSOutput* vsOutput1, const Yw3dVSOutput* vsOutput2)
    {
        // Calculate triangle gradient first.
        CalculateTriangleGradients(vsOutput0, vsOutput1, vsOutput2);

        // Check if wireframe mode.
        if (Yw3d_Fill_WireFrame == m_RenderStates[Yw3d_RS_FillMode])
        {
            RasterizeLine(vsOutput0, vsOutput1);
            RasterizeLine(vsOutput1, vsOutput2);
            RasterizeLine(vsOutput2, vsOutput0);

            return;
        }

        // ------------------------------------------------------------------
        // Rasterize this triangle by scan line.
        // We use "top-left" Fill Convention which d3d used.
        // Reference: <<Tricks of the 3D Game Programming Gurus : Advanced 3D Graphics and Rasterization>>, 9.4.2 Fill Convention, Page604(Chinese), Page934(English)

        // Sort vertices by y-coordinate.
        const Yw3dVSOutput* vertices[3] = { vsOutput0, vsOutput1, vsOutput2 };
        if (vsOutput1->position.y < vertices[0]->position.y)
        {
            vertices[1] = vertices[0];
            vertices[0] = vsOutput1;
        }

        if (vsOutput2->position.y < vertices[0]->position.y)
        {
            vertices[2] = vertices[1];
            vertices[1] = vertices[0];
            vertices[0] = vsOutput2;
        }
        else if (vsOutput2->position.y < vertices[1]->position.y)
        {
            vertices[2] = vertices[1];
            vertices[1] = vsOutput2;
        }

        // Screenspace-position references.
        const Vector4& posA = vertices[0]->position;
        const Vector4& posB = vertices[1]->position;
        const Vector4& posC = vertices[2]->position;

        // Calculate slopes for stepping.
        const float stepX[3] = 
        {
            (posB.y - posA.y > 0.0f) ? (posB.x - posA.x) / (posB.y - posA.y) : 0.0f,
            (posC.y - posA.y > 0.0f) ? (posC.x - posA.x) / (posC.y - posA.y) : 0.0f,
            (posC.y - posB.y > 0.0f) ? (posC.x - posB.x) / (posC.y - posB.y) : 0.0f
        };

        // Begin rasterization.
        float fX[2] = { posA.x, posA.x };   // x1 -> x2.
        uint32_t iY[2] = { 0, 0 };          // y1 -> y2;
        float deltaX[2] = { 0.0f, 0.0f };   // left slope -> right slope.
        for (uint32_t triPart = 0; triPart < 2; triPart++)
        {
            switch (triPart)
            {
            case 0: // Draw upper triangle-part.
                {
                    iY[0] = (uint32_t)ftol(ceilf(posA.y));
                    iY[1] = (uint32_t)ftol(ceilf(posB.y));

                    if (stepX[0] > stepX[1]) // left <-> right ?
                    {
                        deltaX[0] = stepX[1];
                        deltaX[1] = stepX[0];
                    }
                    else
                    {
                        deltaX[0] = stepX[0];
                        deltaX[1] = stepX[1];
                    }

                    const float preStepY = (float)iY[0] - posA.y;
                    fX[0] += deltaX[0] * preStepY;
                    fX[1] += deltaX[1] * preStepY;
                }
                break;
            case 1: // Draw lower triangle-part
                {
                    //iY[0] = iY[1];
                    iY[1] = (uint32_t)ftol(ceilf(posC.y));

                    const float preStepY = (float)iY[0] - posB.y;
                    if (stepX[1] > stepX[2]) // left <-> right ?
                    {
                        deltaX[0] = stepX[1];
                        deltaX[1] = stepX[2];
                        fX[1] = posB.x + deltaX[1] * preStepY;
                    }
                    else
                    {
                        deltaX[0] = stepX[2];
                        deltaX[1] = stepX[1];
                        fX[0] = posB.x + deltaX[0] * preStepY;
                    }
                }
                break;
            default:
                break;
            }

            // Rasterize a single scan line.
            for (; iY[0] < iY[1]; iY[0]++, fX[0] += deltaX[0], fX[1] += deltaX[1])
            {
                const int32_t iX[2] = { ftol(ceilf(fX[0])), ftol(ceilf(fX[1])) };
                //const float preStepX = (float)iX[0] - fX[0];

                Yw3dVSOutput psInput;
                SetVSOutputFromGradient(&psInput, (float)iX[0], (float)iY[0]);
                m_TriangleInfo.curPixelY = iY[0];
                (this->*m_RenderInfo.fpRasterizeScanline)(iY[0], iX[0], iX[1], &psInput);
            }
        }
    }

    void Yw3dDevice::RasterizeLine(const Yw3dVSOutput* vsOutput0, const Yw3dVSOutput* vsOutput1)
    {
        // Get referenced position.
        const Vector4& posA = vsOutput0->position;
        const Vector4& posB = vsOutput1->position;

        uint32_t intCoordA[2] = { (uint32_t)ftol(posA.x), (uint32_t)ftol(posA.y) };
        float deltaX = posB.x - posA.x;
        float deltaY = posB.y - posA.y;

        // Get half thickness in pixels.
        const int32_t lineThicknessHalf = -((int32_t)(m_RenderStates[Yw3d_RS_LineThickness] / 2));

        // Odd number is 1, even number is 0.
        // (m_RenderStates[Yw3d_RS_LineThickness] & 1) is 1 means odd, 0 means even.
        const int32_t posOffset = (m_RenderStates[Yw3d_RS_LineThickness] & 1) ? 0 : 1;

        if (fabsf(deltaX) > fabsf(deltaY))
        {
            // Drawing a line which is more horizontal than vertical.
            const int32_t pixelsX = ftol(deltaX);
            if (0 == pixelsX)
            {
                return;
            }

            const int32_t signX = (pixelsX > 0) ? 1 : -1;
            const float slope = deltaY / deltaX;

            float interpolation = (signX > 0) ? 0.0f : 1.0f;
            const float interpolationStep = (float)signX / (float)(abs(pixelsX) - 1);

            for (int32_t i = 0; i != pixelsX; i += signX, interpolation += interpolationStep)
            {
                const uint32_t curPixelX = intCoordA[0] + i;
                const uint32_t curPixelY = intCoordA[1] + (uint32_t)ftol(slope * i);

                // Interpolation a vertex output for pixel input.
                Yw3dVSOutput psInput;
                SetVSOutputFromGradient(&psInput, (float)curPixelX, (float)curPixelY);

                m_TriangleInfo.curPixelInvW = 1.0f / psInput.position.w;
                MultiplyVertexShaderOutputRegisters(&psInput, &psInput, m_TriangleInfo.curPixelInvW);

                if (0 == lineThicknessHalf)
                {
                    (this->*m_RenderInfo.fpDrawPixel)(curPixelX, curPixelY, &psInput);
                }
                else
                {
                    for (int32_t j = lineThicknessHalf + posOffset; j <= -lineThicknessHalf; j++)
                    {
                        const int32_t newPixelY = curPixelY + j;
                        if ((newPixelY < (int32_t)m_RenderInfo.viewportRect.top) || (newPixelY >= (int32_t)m_RenderInfo.viewportRect.bottom))
                        {
                            continue;
                        }

                        (this->*m_RenderInfo.fpDrawPixel)(curPixelX, newPixelY, &psInput);
                    }
                }
            }
        }
        else
        {
            // Drawing a line which is more vertical than horizontal or equal.
            const int32_t pixelsY = ftol(deltaY);
            if (0 == pixelsY)
            {
                return;
            }

            const int32_t signY = (pixelsY > 0) ? 1 : -1;
            const float slope = deltaX / deltaY;

            float interpolation = (signY > 0) ? 0.0f : 1.0f;
            const float interpolationStep = (float)signY / (float)(abs(pixelsY) - 1);

            for (int32_t i = 0; i != pixelsY; i += signY, interpolation += interpolationStep)
            {
                const uint32_t curPixelX = intCoordA[0] + ftol(slope * i);
                const uint32_t curPixelY = intCoordA[1] + i;

                // Interpolation a vertex output for pixel input.
                Yw3dVSOutput psInput;
                SetVSOutputFromGradient(&psInput, (float)curPixelX, (float)curPixelY);

                m_TriangleInfo.curPixelInvW = 1.0f / psInput.position.w;
                MultiplyVertexShaderOutputRegisters(&psInput, &psInput, m_TriangleInfo.curPixelInvW);

                if (0 == lineThicknessHalf)
                {
                    (this->*m_RenderInfo.fpDrawPixel)(curPixelX, curPixelY, &psInput);
                }
                else
                {
                    for (int32_t j = lineThicknessHalf + posOffset; j <= -lineThicknessHalf; j++)
                    {
                        const int32_t newPixelX = curPixelX + j;
                        if ((newPixelX < (int32_t)m_RenderInfo.viewportRect.left) || (newPixelX >= (int32_t)m_RenderInfo.viewportRect.right))
                        {
                            continue;
                        }

                        (this->*m_RenderInfo.fpDrawPixel)(newPixelX, curPixelY, &psInput);
                    }
                }
            }
        }
    }

    void Yw3dDevice::RasterizeScanline_ColorOnly(uint32_t y, uint32_t x1, uint32_t x2, Yw3dVSOutput* vsOutput)
    {
        // Get color buffer data and depth buffer data.
        float* frameData = m_RenderInfo.frameData + (y * m_RenderInfo.colorBufferPitch + x1 * m_RenderInfo.colorFloats);
        float* depthData = m_RenderInfo.depthData + (y * m_RenderInfo.depthBufferPitch + x1);

        // Start to render each pixel.
        for (; x1 < x2; x1++, frameData += m_RenderInfo.colorFloats, depthData++, StepXVSOutputFromGradient(vsOutput))
        {
            // Get depth of current pixel.
            float depth = vsOutput->position.z;

            // Perform depth test.
            switch (m_RenderInfo.depthCompare)
            {
            case Yw3d_CMP_Never: return;
            case Yw3d_CMP_Equal: if (fabsf(depth - *depthData) < YW_FLOAT_PRECISION) break; else continue;
            case Yw3d_CMP_NotEqual: if (fabsf(depth - *depthData) >= YW_FLOAT_PRECISION) break; else continue;
            case Yw3d_CMP_Less: if (depth < *depthData) break; else continue;
            case Yw3d_CMP_LessEqual: if (depth <= *depthData) break; else continue;
            case Yw3d_CMP_Greater: if (depth > *depthData) break; else continue;
            case Yw3d_CMP_GreaterEqual: if (depth >= *depthData) break; else continue;
            case Yw3d_CMP_Always: break;
            default: break; // Can not happen.
            }

            // Passed depth test - update depthbuffer!
            if (m_RenderInfo.depthWriteEnabled)
            {
                *depthData = depth;
            }

            // Update color buffer.
            if (m_RenderInfo.colorWriteEnabled)
            {
                // Get only shader register data only.
                // Note: psInput now only contains valid register data, position etc. are not initialized!
                Yw3dVSOutput psInput;
                m_TriangleInfo.curPixelInvW = 1.0f / vsOutput->position.w;
                MultiplyVertexShaderOutputRegisters(&psInput, vsOutput, m_TriangleInfo.curPixelInvW);

                // Read in current pixel's color in the colorbuffer.
                Vector4 pixelColor(0.0f, 0.0f, 0.0f, 1.0f);
                switch (m_RenderInfo.colorFloats)
                {
                case 4:
                    pixelColor.a = frameData[3];
                case 3:
                    pixelColor.b = frameData[2];
                case 2:
                    pixelColor.g = frameData[1];
                case 1:
                    pixelColor.r = frameData[0];
                default:    // Can not happen.
                    break;
                }

                // Execute the pixel shader.
                m_TriangleInfo.curPixelX = x1;
                m_PixelShader->Execute(psInput.shaderOutputs, pixelColor, depth);

                // Write the new color to the colorbuffer.
                switch (m_RenderInfo.colorFloats)
                {
                case 4:
                    frameData[3] = pixelColor.a;
                case 3:
                    frameData[2] = pixelColor.b;
                case 2:
                    frameData[1] = pixelColor.g;
                case 1:
                    frameData[0] = pixelColor.r;
                default:    // Can not happen.
                    break;
                }
            }

            m_RenderInfo.renderedPixels++;
        }
    }

    void Yw3dDevice::RasterizeScanline_ColorOnly_MightKillPixels(uint32_t y, uint32_t x1, uint32_t x2, Yw3dVSOutput* vsOutput)
    {
        // Get color buffer data and depth buffer data.
        float* frameData = m_RenderInfo.frameData + (y * m_RenderInfo.colorBufferPitch + x1 * m_RenderInfo.colorFloats);
        float* depthData = m_RenderInfo.depthData + (y * m_RenderInfo.depthBufferPitch + x1);

        // Start to render each pixel.
        for (; x1 < x2; x1++, frameData += m_RenderInfo.colorFloats, depthData++, StepXVSOutputFromGradient(vsOutput))
        {
            // Get depth of current pixel.
            float depth = vsOutput->position.z;

            // Perform depth test.
            switch (m_RenderInfo.depthCompare)
            {
            case Yw3d_CMP_Never: return;
            case Yw3d_CMP_Equal: if (fabsf(depth - *depthData) < YW_FLOAT_PRECISION) break; else continue;
            case Yw3d_CMP_NotEqual: if (fabsf(depth - *depthData) >= YW_FLOAT_PRECISION) break; else continue;
            case Yw3d_CMP_Less: if (depth < *depthData) break; else continue;
            case Yw3d_CMP_LessEqual: if (depth <= *depthData) break; else continue;
            case Yw3d_CMP_Greater: if (depth > *depthData) break; else continue;
            case Yw3d_CMP_GreaterEqual: if (depth >= *depthData) break; else continue;
            case Yw3d_CMP_Always: break;
            default: break; // Can not happen.
            }

            // Only update color and depth buffer when pixel is not killed.
            if (m_RenderInfo.colorWriteEnabled || m_RenderInfo.depthWriteEnabled)
            {
                // Get only shader register data only.
                // Note: psInput now only contains valid register data, position etc. are not initialized!
                Yw3dVSOutput psInput;
                m_TriangleInfo.curPixelInvW = 1.0f / vsOutput->position.w;
                MultiplyVertexShaderOutputRegisters(&psInput, vsOutput, m_TriangleInfo.curPixelInvW);

                // Read in current pixel's color in the colorbuffer.
                Vector4 pixelColor(0.0f, 0.0f, 0.0f, 1.0f);
                switch (m_RenderInfo.colorFloats)
                {
                case 4:
                    pixelColor.a = frameData[3];
                case 3:
                    pixelColor.b = frameData[2];
                case 2:
                    pixelColor.g = frameData[1];
                case 1:
                    pixelColor.r = frameData[0];
                default:    // Can not happen.
                    break;
                }

                // Execute the pixel shader.
                m_TriangleInfo.curPixelX = x1;
                if (!m_PixelShader->Execute(psInput.shaderOutputs, pixelColor, depth))
                {
                    // Pixel got killed.
                    continue;
                }

                // Passed depth-test and pixel was not killed, so update depthbuffer.
                if (m_RenderInfo.depthWriteEnabled)
                {
                    *depthData = depth;
                }

                // Write the new color to the colorbuffer.
                if (m_RenderInfo.colorWriteEnabled)
                {
                    switch (m_RenderInfo.colorFloats)
                    {
                    case 4:
                        frameData[3] = pixelColor.a;
                    case 3:
                        frameData[2] = pixelColor.b;
                    case 2:
                        frameData[1] = pixelColor.g;
                    case 1:
                        frameData[0] = pixelColor.r;
                    default:    // Can not happen.
                        break;
                    }
                }
            }

            m_RenderInfo.renderedPixels++;
        }
    }

    void Yw3dDevice::RasterizeScanline_ColorDepth(uint32_t y, uint32_t x1, uint32_t x2, Yw3dVSOutput* vsOutput)
    {
        // Get color buffer data and depth buffer data.
        float* frameData = m_RenderInfo.frameData + (y * m_RenderInfo.colorBufferPitch + x1 * m_RenderInfo.colorFloats);
        float* depthData = m_RenderInfo.depthData + (y * m_RenderInfo.depthBufferPitch + x1);

        // Start to render each pixel.
        for (; x1 < x2; x1++, frameData += m_RenderInfo.colorFloats, depthData++, StepXVSOutputFromGradient(vsOutput))
        {
            // Get only shader register data only.
            // Note: psInput now only contains valid register data, position etc. are not initialized!
            Yw3dVSOutput psInput;
            m_TriangleInfo.curPixelInvW = 1.0f / vsOutput->position.w;
            MultiplyVertexShaderOutputRegisters(&psInput, vsOutput, m_TriangleInfo.curPixelInvW);

            // Read in current pixel's color in the colorbuffer.
            Vector4 pixelColor(0.0f, 0.0f, 0.0f, 1.0f);
            switch (m_RenderInfo.colorFloats)
            {
            case 4:
                pixelColor.a = frameData[3];
            case 3:
                pixelColor.b = frameData[2];
            case 2:
                pixelColor.g = frameData[1];
            case 1:
                pixelColor.r = frameData[0];
            default:    // Can not happen.
                break;
            }

            // Get depth of current pixel.
            float depth = vsOutput->position.z;

            // Execute the pixel shader.
            m_TriangleInfo.curPixelX = x1;
            if (!m_PixelShader->Execute(psInput.shaderOutputs, pixelColor, depth))
            {
                // Pixel got killed.
                continue;
            }

            // Perform depth test.
            switch (m_RenderInfo.depthCompare)
            {
            case Yw3d_CMP_Never: return;
            case Yw3d_CMP_Equal: if (fabsf(depth - *depthData) < YW_FLOAT_PRECISION) break; else continue;
            case Yw3d_CMP_NotEqual: if (fabsf(depth - *depthData) >= YW_FLOAT_PRECISION) break; else continue;
            case Yw3d_CMP_Less: if (depth < *depthData) break; else continue;
            case Yw3d_CMP_LessEqual: if (depth <= *depthData) break; else continue;
            case Yw3d_CMP_Greater: if (depth > *depthData) break; else continue;
            case Yw3d_CMP_GreaterEqual: if (depth >= *depthData) break; else continue;
            case Yw3d_CMP_Always: break;
            default: break; // Can not happen.
            }

            // Passed depth test - update depthbuffer!
            if (m_RenderInfo.depthWriteEnabled)
            {
                *depthData = depth;
            }

            // Update color buffer.
            if (m_RenderInfo.colorWriteEnabled)
            {
                switch (m_RenderInfo.colorFloats)
                {
                case 4:
                    frameData[3] = pixelColor.a;
                case 3:
                    frameData[2] = pixelColor.b;
                case 2:
                    frameData[1] = pixelColor.g;
                case 1:
                    frameData[0] = pixelColor.r;
                default:    // Can not happen.
                    break;
                }
            }

            m_RenderInfo.renderedPixels++;
        }
    }

    void Yw3dDevice::DrawPixel_ColorOnly(uint32_t x, uint32_t y, const Yw3dVSOutput* vsOutput)
    {
        // Get color buffer data and depth buffer data.
        float* frameData = m_RenderInfo.frameData + (y * m_RenderInfo.colorBufferPitch + x * m_RenderInfo.colorFloats);
        float* depthData = m_RenderInfo.depthData + (y * m_RenderInfo.depthBufferPitch + x);

        // Get depth of current pixel.
        float depth = vsOutput->position.z;

        // Perform depth test.
        switch (m_RenderInfo.depthCompare)
        {
        case Yw3d_CMP_Never: return;
        case Yw3d_CMP_Equal: if (fabsf(depth - *depthData) < YW_FLOAT_PRECISION) break; else return;
        case Yw3d_CMP_NotEqual: if (fabsf(depth - *depthData) >= YW_FLOAT_PRECISION) break; else return;
        case Yw3d_CMP_Less: if (depth < *depthData) break; else return;
        case Yw3d_CMP_LessEqual: if (depth <= *depthData) break; else return;
        case Yw3d_CMP_Greater: if (depth > *depthData) break; else return;
        case Yw3d_CMP_GreaterEqual: if (depth >= *depthData) break; else return;
        case Yw3d_CMP_Always: break;
        default: break; // Can not happen.
        }

        // Passed depth test - update depthbuffer!
        if (m_RenderInfo.depthWriteEnabled)
        {
            *depthData = depth;
        }

        // Update color buffer.
        if (m_RenderInfo.colorWriteEnabled)
        {
            // Read in current pixel's color in the colorbuffer.
            Vector4 pixelColor(0.0f, 0.0f, 0.0f, 1.0f);
            switch (m_RenderInfo.colorFloats)
            {
            case 4:
                pixelColor.a = frameData[3];
            case 3:
                pixelColor.b = frameData[2];
            case 2:
                pixelColor.g = frameData[1];
            case 1:
                pixelColor.r = frameData[0];
            default:    // Can not happen.
                break;
            }

            // Execute the pixel shader.
            m_TriangleInfo.curPixelX = x;
            m_TriangleInfo.curPixelY = y;
            m_PixelShader->Execute(vsOutput->shaderOutputs, pixelColor, depth);

            // Write the new color to the colorbuffer.
            switch (m_RenderInfo.colorFloats)
            {
            case 4:
                frameData[3] = pixelColor.a;
            case 3:
                frameData[2] = pixelColor.b;
            case 2:
                frameData[1] = pixelColor.g;
            case 1:
                frameData[0] = pixelColor.r;
            default:    // Can not happen.
                break;
            }
        }

        m_RenderInfo.renderedPixels++;
    }

    void Yw3dDevice::DrawPixel_ColorDepth(uint32_t x, uint32_t y, const Yw3dVSOutput* vsOutput)
    {
        // Get color buffer data and depth buffer data.
        float* frameData = m_RenderInfo.frameData + (y * m_RenderInfo.colorBufferPitch + x * m_RenderInfo.colorFloats);
        float* depthData = m_RenderInfo.depthData + (y * m_RenderInfo.depthBufferPitch + x);

        // Read in current pixel's color in the colorbuffer.
        Vector4 pixelColor(0.0f, 0.0f, 0.0f, 1.0f);
        switch (m_RenderInfo.colorFloats)
        {
        case 4:
            pixelColor.a = frameData[3];
        case 3:
            pixelColor.b = frameData[2];
        case 2:
            pixelColor.g = frameData[1];
        case 1:
            pixelColor.r = frameData[0];
        default:    // Can not happen.
            break;
        }

        // Get depth of current pixel.
        float depth = vsOutput->position.z;

        // Execute the pixel shader.
        m_TriangleInfo.curPixelX = x;
        m_TriangleInfo.curPixelY = y;
        if (!m_PixelShader->Execute(vsOutput->shaderOutputs, pixelColor, depth))
        {
            // Pixel got killed.
            return;
        }

        // Perform depth test.
        switch (m_RenderInfo.depthCompare)
        {
        case Yw3d_CMP_Never: return;
        case Yw3d_CMP_Equal: if (fabsf(depth - *depthData) < YW_FLOAT_PRECISION) break; else return;
        case Yw3d_CMP_NotEqual: if (fabsf(depth - *depthData) >= YW_FLOAT_PRECISION) break; else return;
        case Yw3d_CMP_Less: if (depth < *depthData) break; else return;
        case Yw3d_CMP_LessEqual: if (depth <= *depthData) break; else return;
        case Yw3d_CMP_Greater: if (depth > *depthData) break; else return;
        case Yw3d_CMP_GreaterEqual: if (depth >= *depthData) break; else return;
        case Yw3d_CMP_Always: break;
        default: break; // Can not happen.
        }

        // Passed depth test - update depthbuffer!
        if (m_RenderInfo.depthWriteEnabled)
        {
            *depthData = depth;
        }

        // Update color buffer.
        if (m_RenderInfo.colorWriteEnabled)
        {
            switch (m_RenderInfo.colorFloats)
            {
            case 4:
                frameData[3] = pixelColor.a;
            case 3:
                frameData[2] = pixelColor.b;
            case 2:
                frameData[1] = pixelColor.g;
            case 1:
                frameData[0] = pixelColor.r;
            default:    // Can not happen.
                break;
            }
        }

        m_RenderInfo.renderedPixels++;
    }
}

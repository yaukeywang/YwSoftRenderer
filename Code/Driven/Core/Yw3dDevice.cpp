// Add by Yaukey at 2018-01-30.
// YW Soft Renderer 3d device class.

#include "stdafx.h"
#include "System/Support/YwUtility.h"
#include "Yw3dDevice.h"
#include "Yw3d.h"
#include "Yw3dIndexBuffer.h"
#include "Yw3dPrimitiveAssembler.h"
#include "Yw3dRenderTarget.h"
#include "Yw3dSurface.h"
#include "Yw3dVertexBuffer.h"
#include "Yw3dVertexFormat.h"

namespace yw
{
    Yw3dDevice::Yw3dDevice(Yw3d* yw3d, const Yw3dDeviceParameters* deviceParameters) :
        m_pVertexFormat(nullptr),
        m_PrimitiveAssembler(nullptr),
        m_VertexShader(nullptr),
        m_TriangleShader(nullptr),
        m_PixelShader(nullptr),
        m_IndexBuffer(nullptr)
    {
    }

    Yw3dDevice::~Yw3dDevice()
    {
    }

    Yw3dResult Yw3dDevice::Create()
    {
        return Yw3d_E_Unknown;
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
            primitiveType = primitiveCount + 2;
            break;
        case Yw3d_PT_TriangleStrip:
            primitiveType = primitiveCount + 2;
            break;
        case Yw3d_PT_TriangleList:
            primitiveType = primitiveCount * 3;
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

        uint32_t vertexIndicies = {startVertex, startVertex + 1, startVertex + 2};
        bool flip4TriStrip = false; // used when drawing triangle strips.
        while (primitiveCount-- > 0)
        {
            Yw3dVertexCacheEntry* vertices = {nullptr, nullptr, nullptr};
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
                ProcessTriangle(vertices[0]->vertexOutput, vertices[2]->vertexOutput, vertices[1]->vertexOutput);
            }
            else
            {
                ProcessTriangle(vertices[0]->vertexOutput, vertices[1]->vertexOutput, vertices[2]->vertexOutput);
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
            return e_invalidparameters;
        }

        if ((Yw3d_PT_TriangleFan != primitiveType) || (Yw3d_PT_TriangleStrip != primitiveType) || (Yw3d_PT_TriangleList != primitiveType))
        {
            LOGE( "Yw3dDevice::DrawIndexedPrimitive: invalid primitive type specified.\n" );
            return e_invalidparameters;
        }

        if (nullptr == m_IndexBuffer)
        {
            LOGE( "Yw3dDevice::DrawIndexedPrimitive: no indexbuffer has been set!\n" );
            return e_invalidstate;
        }

        Yw3dResult resCheck = PreRender();
        if (YW3D_FAILED(resCheck))
        {
            return resCheck;
        }

        uint32_t vertexIndicies = {startVertex, startVertex + 1, startVertex + 2};
        bool flip4TriStrip = false; // used when drawing triangle strips.
        while (primitiveCount-- > 0)
        {
            Yw3dVertexCacheEntry* vertices = {nullptr, nullptr, nullptr};
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
                ProcessTriangle(vertices[0]->vertexOutput, vertices[2]->vertexOutput, vertices[1]->vertexOutput);
            }
            else
            {
                ProcessTriangle(vertices[0]->vertexOutput, vertices[1]->vertexOutput, vertices[2]->vertexOutput);
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

    Yw3dResult Yw3dDevice::PreRender()
    {
        return Yw3d_E_Unknown;
    }

    void Yw3dDevice::PostRender()
    {
        return Yw3d_E_Unknown;
    }

    Yw3dResult Yw3dDevice::DecodeVertexStream(Yw3dVSInput& vertexShaderInput, uint32_t vertexIndex)
    {
        // Raw vertex data cache for each stream.
        const byte* vertexRawData[YW3D_MAX_VERTEX_STREAMS];
        memset(vertexRawData, 0, sizeof(byte) * YW3D_MAX_VERTEX_STREAMS);

        // Get start vertex index in vertex buffer of each stream.
        const VertexStream* vertexStream = m_VertexStreams;
        for (int streamIdx = 0; streamIdx < m_VertexFormat->GetHighestStream(); streamIdx++; vertexStream++)
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
            Yw3dShaderRegister& shaderRegister = vertexShaderInput->shaderInputs[vertexElement->register];
            
            // Convert vertex raw data from byte into float.
            const float* vertexData = (const float*)vertexRawData[vertexElement->stream];

            // Fill this shader register result by register type, and read from vertex data.
            switch (vertexElement->type)
            {
            case Yw3d_VET_Float:
                shaderRegister = Yw3dShaderRegister(vertexData[0], 0.0f, 0.0f, 0.0f);
                vertexRawData[vertexElement->stream] += sizeof(float);
                break;
            case Yw3d_VET_Vector2;
                shaderRegister = Yw3dShaderRegister(vertexData[0], vertexData[1], 0.0f, 0.0f);
                vertexRawData[vertexElement->stream] += sizeof(float) * 2;
                break;
            case Yw3d_VET_Vector3:
                shaderRegister = Yw3dShaderRegister(vertexData[0], vertexData[1], vertexData[2], 0.0f);
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
        }

        // Get next vertex element format.
        vertexElement++;
    }

    Yw3dResult Yw3dDevice::FetchVertex(Yw3dVertexCacheEntry** vertexCacheEntry, uint32_t vertexIndex)
    {
        if ((nullptr != vertexCacheEntry) && ((*vertexCacheEntry)->vertexIndex == vertexIndex))
        {
            (*vertexCacheEntry)->fetchTime = m_FetchedVertices++;
            return Yw3d_S_OK;
        }

        // Check if we can append more vertex.
        const bool canAppend = (m_NumValidCacheEntries < YW3D_VERTEX_CACHE_SIZE);

        // Find vertex in cache.
        Yw3dVertexCacheEntry* cacheEntry = m_VertexCache[0];
        Yw3dVertexCacheEntry* destEntry = cacheEntry;
        for (int i = 0; i < m_NumValidCacheEntries; i++, cacheEntry++)
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
            destEntry = m_VertexCache[m_NumValidCacheEntries++];
        }

        // Update the destination cache entry and return it
        destEntry->vertexIndex = vertexIndex;
        destEntry->fetchTime = m_FetchedVertices++;

        // Decode this vertex data from stream.
        Yw3dResult resDecode = DecodeVertexStream(destEntry->vertexOutput->sourceInput, vertexIndex);
        if (YW3D_FAILED(resDecode))
        {
            return resDecode;
        }

        // Execute vertex shader.
        //m_VertexShader->Execute();

        // Fill result.
        *vertexCacheEntry = destEntry;

        return Yw3d_S_OK;
    }
}

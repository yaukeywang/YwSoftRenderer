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
            VertexCacheEntry* vertices = {nullptr, nullptr, nullptr};
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
            VertexCacheEntry* vertices = {nullptr, nullptr, nullptr};
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
}

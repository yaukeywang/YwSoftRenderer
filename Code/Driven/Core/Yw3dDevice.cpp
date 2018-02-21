// Add by Yaukey at 2018-01-30.
// YW Soft Renderer 3d device class.

#include "stdafx.h"
#include "Yw3dDevice.h"
#include "Yw3d.h"
#include "System/Support/YwUtility.h"

namespace yw
{
    Yw3dDevice::Yw3dDevice(Yw3d* yw3d, const Yw3dDeviceParameters* deviceParameters)
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
        }

        // Process post render state.
        PostRender();

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dDevice::DrawIndexedPrimitive(Yw3dPrimitiveType primitiveType, int32_t baseVertexIndex, uint32_t minIndex, uint32_t numVertices, uint32_t startIndex, uint32_t primitiveCount)
    {
        return Yw3d_E_Unknown;
    }

    Yw3dResult Yw3dDevice::PreRender()
    {

    }

    void Yw3dDevice::PostRender()
    {

    }
}

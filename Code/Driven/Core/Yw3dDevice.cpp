// Add by Yaukey at 2018-01-30.
// YW Soft Renderer 3d device class.

#include "stdafx.h"
#include "System/Support/YwUtility.h"
#include "Yw3dDevice.h"
#include "Yw3d.h"
#include "Yw3dIndexBuffer.h"
#include "Yw3dPrimitiveAssembler.h"
#include "Yw3dRenderTarget.h"
#include "Yw3dShader.h"
#include "Yw3dSurface.h"
#include "Yw3dVertexBuffer.h"
#include "Yw3dVertexFormat.h"

namespace yw
{
    Yw3dDevice::Yw3dDevice(Yw3d* yw3d, const Yw3dDeviceParameters* deviceParameters) :
        m_VertexFormat(nullptr),
        m_PrimitiveAssembler(nullptr),
        m_VertexShader(nullptr),
        m_TriangleShader(nullptr),
        m_PixelShader(nullptr),
        m_IndexBuffer(nullptr),
        m_RenderTarget(nullptr),
        m_NumValidCacheEntries(0),
        m_FetchedVertices(0)
    {
        memset(m_RenderStates, 0, Yw3d_RS_NumRenderStates * sizeof(uint32_t));
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

        if ((Yw3d_PT_TriangleFan != primitiveType) || (Yw3d_PT_TriangleStrip != primitiveType) || (Yw3d_PT_TriangleList != primitiveType))
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

    Yw3dResult Yw3dDevice::SampleTexture(Vector4& color, uint32_t samplerNumber, float u, float v, float w, const Vector4* xGradient, const Vector4* yGradient)
    {
        return Yw3d_E_Unknown;
    }

    Yw3dResult Yw3dDevice::PreRender()
    {
        return Yw3d_E_Unknown;
    }

    void Yw3dDevice::PostRender()
    {
        return;
    }

    Yw3dResult Yw3dDevice::DecodeVertexStream(Yw3dVSInput& vertexShaderInput, uint32_t vertexIndex)
    {
        // Raw vertex data cache for each stream.
        const byte* vertexRawData[YW3D_MAX_VERTEX_STREAMS];
        memset(vertexRawData, 0, sizeof(byte) * YW3D_MAX_VERTEX_STREAMS);

        // Get start vertex index in vertex buffer of each stream.
        const VertexStream* vertexStream = m_VertexStreams;
        for (uint32_t streamIdx = 0; streamIdx < m_VertexFormat->GetHighestStream(); streamIdx++, vertexStream++)
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
                shaderRegister = Yw3dShaderRegister(vertexData[0], 0.0f, 0.0f, 0.0f);
                vertexRawData[vertexElement->stream] += sizeof(float);
                break;
            case Yw3d_VET_Vector2:
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

            // Get next vertex element format.
            vertexElement++;
        }

        return Yw3d_S_OK;
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

    void Yw3dDevice::ProjectVertex(Yw3dVSOutput* vsOutput)
    {
        if (vsOutput->position.w < YW_FLOAT_PRECISION)
        {
            return;
        }

        const float invW = 1.0f / vsOutput->position.w;
        vsOutput->position.x *= invW;
        vsOutput->position.y *= invW;
        vsOutput->position.z *= invW;
        vsOutput->position.w = 1.0f;

        vsOutput->position *= m_RenderTarget->GetViewportMatrix();

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
        const float deltaY[2] = { position1.y - position0.y, position2.y - position0.x };
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
        float fX[2] = { posA.x, posA.x };
        for (uint32_t triPart = 0; triPart < 2; triPart++)
        {
            uint32_t iY[2] = { 0, 0 };
            float deltaX[2] = { 0.0f, 0.0f };

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
            for (; iY[0] < iY[1]; fX[0] += deltaX[0], fX[1] += deltaX[1])
            {
                const int32_t iX[2] = { ftol(ceilf(fX[0])), ftol(ceilf(fX[1])) };
                //const float preStepX = (float)iX[0] - fX[0];

                Yw3dVSOutput psInput;
                SetVSOutputFromGradient(&psInput, (float)iX[0], (float)iY[0]);
                m_TriangleInfo.curPixelY = iY[0];
                (this->*m_RenderInfo.RasterizeScanline)(iY[0], iX[0], iX[1], &psInput);
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
                    (this->*m_RenderInfo.DrawPixel)(curPixelX, curPixelY, &psInput);
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

                        (this->*m_RenderInfo.DrawPixel)(curPixelX, newPixelY, &psInput);
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
                    (this->*m_RenderInfo.DrawPixel)(curPixelX, curPixelY, &psInput);
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

                        (this->*m_RenderInfo.DrawPixel)(newPixelX, curPixelY, &psInput);
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

            // passed depth test - update depthbuffer!
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

                // Read in current pixel's color in the colorbuffer
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
}

// Add by Yaukey at 2020-02-02.
// YW Soft Renderer demo with triangle shader wireframe shader class.

#include "YwDemoTriangleShaderWireframeShader.h"

namespace yw
{
    // ------------------------------------------------------------------
    // Wireframe vertex shader.

    // Vertex input format:
    // 0 - Vector3 position;
    // 1 - Vector3 normal;
    // 2 - Vector4 tangent;
    // 3 - Vector4 color;
    // 4 - Vector2 texcoord;
    // 5 - Vector2 texcoord2;
    void DemoTriangleShaderWireframeVertexShader::Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput)
    {
        // Get current position.
        float4 curPos = vsShaderInput[0];
        curPos.w = 1.0f;

        // The projection vertex position.
        position = curPos * (*GetWVPMatrix());

        // The projection pos.
        vsShaderOutput[0] = position;
    }

    Yw3dShaderRegisterType DemoTriangleShaderWireframeVertexShader::GetOutputRegisters(uint32_t shaderRegister)
    {
        switch (shaderRegister)
        {
        case 0:
            return Yw3d_SRT_Vector4; // Projective pos.
        case 1:
            return Yw3d_SRT_Vector4; // EdgeA.
        case 2:
            return Yw3d_SRT_Vector4; // EdgeB.
        case 3:
            return Yw3d_SRT_Float32; // Case.
        default:
            return Yw3d_SRT_Unused;
        }
    }

    // ------------------------------------------------------------------
    // Wireframe triangle shader.

    bool DemoTriangleShaderWireframeTriangleShader::Execute(Yw3dShaderRegister* shaderRegister0, Yw3dShaderRegister* shaderRegister1, Yw3dShaderRegister* shaderRegister2)
    {
        // Constants.
        const uint32_t infoA[] = { 0, 0, 0, 0, 1, 1, 2 };
        const uint32_t infoB[] = { 1, 1, 2, 0, 2, 1, 2 };
        const uint32_t infoAd[] = { 2, 2, 1, 1, 0, 0, 0 };
        const uint32_t infoBd[] = { 2, 2, 1, 2, 0, 2, 1 };
        const uint32_t infoEdge0[] = { 0, 2, 0, 0, 0, 0, 2 };

        // Get input pos.
        float4 inputPos0 = shaderRegister0[0];
        float4 inputPos1 = shaderRegister1[0];
        float4 inputPos2 = shaderRegister2[0];

        // Compute the case from the positions of point in space.
        int32_t curCase = (inputPos0.z < 0) * 4 + (inputPos1.z < 0) * 2 + (inputPos2.z < 0);

        // Set output case.
        shaderRegister0[3].x = (float)curCase;
        shaderRegister1[3].x = (float)curCase;
        shaderRegister2[3].x = (float)curCase;

        if (7 == curCase)
        {
            return true;
        }

        // Transform point to viewport space.
        const float44& viewportMatrix = GetMatrix(0);
        float2 points[3];
        points[0] = inputPos0 * viewportMatrix;
        points[1] = inputPos1 * viewportMatrix;
        points[2] = inputPos2 * viewportMatrix;

        // If Case is 0, all projected points are defined, do the
        // general case computation
        if (curCase == 0)
        {
            // Compute the edges vectors of the transformed triangle
            float2 edges[3];
            edges[0] = points[1] - points[0];
            edges[1] = points[2] - points[1];
            edges[2] = points[0] - points[2];

            // Store the length of the edges
            float lengths[3];
            lengths[0] = edges[0].Length();
            lengths[1] = edges[1].Length();
            lengths[2] = edges[2].Length();

            // Compute the cos angle of each vertices
            float cosAngles[3];
            cosAngles[0] = dot(-edges[2], edges[0]) / (lengths[2] * lengths[0]);
            cosAngles[1] = dot(-edges[0], edges[1]) / (lengths[0] * lengths[1]);
            cosAngles[2] = dot(-edges[1], edges[2]) / (lengths[1] * lengths[2]);

            // The height for each vertices of the triangle
            float heights[3];
            heights[1] = lengths[0] * sqrt(1 - cosAngles[0] * cosAngles[0]);
            heights[2] = lengths[1] * sqrt(1 - cosAngles[1] * cosAngles[1]);
            heights[0] = lengths[2] * sqrt(1 - cosAngles[2] * cosAngles[2]);

            float edgeSigns[3];
            edgeSigns[0] = (edges[0].x > 0 ? 1.0f : -1.0f);
            edgeSigns[1] = (edges[1].x > 0 ? 1.0f : -1.0f);
            edgeSigns[2] = (edges[2].x > 0 ? 1.0f : -1.0f);

            float edgeOffsets[3];
            edgeOffsets[0] = lengths[0] * (0.5f - 0.5f * edgeSigns[0]);
            edgeOffsets[1] = lengths[1] * (0.5f - 0.5f * edgeSigns[1]);
            edgeOffsets[2] = lengths[2] * (0.5f - 0.5f * edgeSigns[2]);

            // Vertex 0 Projective pos.
            // Vertex 0 EdgeA.
            float4& output0EdgeA = shaderRegister0[1];
            output0EdgeA.x = 0;
            output0EdgeA.y = heights[0];
            output0EdgeA.z = 0;

            // Vertex 0 EdgeB.
            float4& output0EdgeB = shaderRegister0[2];
            output0EdgeB.x = edgeOffsets[0];
            output0EdgeB.y = edgeOffsets[1] + edgeSigns[1] * cosAngles[1] * lengths[0];
            output0EdgeB.z = edgeOffsets[2] + edgeSigns[2] * lengths[2];

            // Vertex 1 Projective pos.
            // Vertex 1 EdgeA.
            float4& output1EdgeA = shaderRegister1[1];
            output1EdgeA.x = 0;
            output1EdgeA.y = 0;
            output1EdgeA.z = heights[1];

            // Vertex 1 EdgeB.
            float4& output1EdgeB = shaderRegister1[2];
            output1EdgeB.x = edgeOffsets[0] + edgeSigns[0] * lengths[0];
            output1EdgeB.y = edgeOffsets[1];
            output1EdgeB.z = edgeOffsets[2] + edgeSigns[2] * cosAngles[2] * lengths[1];

            // Vertex 2 Projective pos.
            // Vertex 2 EdgeA.
            float4& output2EdgeA = shaderRegister2[1];
            output2EdgeA.x = heights[2];
            output2EdgeA.y = 0;
            output2EdgeA.z = 0;

            // Vertex 2 EdgeB.
            float4& output2EdgeB = shaderRegister2[2];
            output2EdgeB.x = edgeOffsets[0] + edgeSigns[0] * cosAngles[0] * lengths[2];
            output2EdgeB.y = edgeOffsets[1] + edgeSigns[1] * lengths[1];
            output2EdgeB.z = edgeOffsets[2];
        }
        // Else need some tricky computations
        else
        {
            // Then compute and pass the edge definitions from the case
            // Vertex EdgeA x-y.
            //output.EdgeA.xy = points[infoA[output.Case]];
            float2 tmpEdgeAXY = points[infoA[curCase]];
            shaderRegister0[1].x = tmpEdgeAXY.x;
            shaderRegister0[1].y = tmpEdgeAXY.y;
            shaderRegister1[1].x = tmpEdgeAXY.x;
            shaderRegister1[1].y = tmpEdgeAXY.y;
            shaderRegister2[1].x = tmpEdgeAXY.x;
            shaderRegister2[1].y = tmpEdgeAXY.y;

            // Vertex EdgeB x-y.
            //output.EdgeB.xy = points[infoB[output.Case]];
            float2 tmpEdgeBXY = points[infoB[curCase]];
            shaderRegister0[2].x = tmpEdgeBXY.x;
            shaderRegister0[2].y = tmpEdgeBXY.y;
            shaderRegister1[2].x = tmpEdgeBXY.x;
            shaderRegister1[2].y = tmpEdgeBXY.y;
            shaderRegister2[2].x = tmpEdgeBXY.x;
            shaderRegister2[2].y = tmpEdgeBXY.y;

            // Vertex EdgeA z-w.
            //output.EdgeA.zw = normalize(output.EdgeA.xy - points[infoAd[output.Case]]);
            float2 edgeAZW = normalize(tmpEdgeAXY - points[infoAd[curCase]]);
            shaderRegister0[1].z = edgeAZW.x;
            shaderRegister0[1].w = edgeAZW.y;
            shaderRegister1[1].z = edgeAZW.x;
            shaderRegister1[1].w = edgeAZW.y;
            shaderRegister2[1].z = edgeAZW.x;
            shaderRegister2[1].w = edgeAZW.y;

            // Vertex EdgeB z-w.
            //output.EdgeB.zw = normalize(output.EdgeB.xy - points[infoBd[output.Case]]);
            float2 edgeBZW = normalize(tmpEdgeBXY - points[infoBd[curCase]]);
            shaderRegister0[2].z = edgeBZW.x;
            shaderRegister0[2].w = edgeBZW.y;
            shaderRegister1[2].z = edgeBZW.x;
            shaderRegister1[2].w = edgeBZW.y;
            shaderRegister2[2].z = edgeBZW.x;
            shaderRegister2[2].w = edgeBZW.y;
        }

        return true;
    }

    // ------------------------------------------------------------------
    // Wireframe default pixel shader.

    bool DemoTriangleShaderWireframeDefaultPixelShader::MightKillPixels()
    {
        //return false; // Can use alpha blend.
        return true; // Discard pixel if pixel shader return false.
    }

    float DemoTriangleShaderWireframeDefaultPixelShader::EvalMinDistanceToEdges(const Yw3dShaderRegister* input)
    {
        // Get input value.
        const float4& inputPos = input[0];
        const float4& inputEdgeA = input[1];
        const float4& inputEdgeB = input[2];
        const int32_t inputCase = (int32_t)input[3].x;

        // Result distant.
        float dist = 0;

        // The easy case, the 3 distances of the fragment to the 3 edges is already
        // computed, get the min.
        if (0 == inputCase)
        {
            dist = min(min(inputEdgeA.x, inputEdgeA.y), inputEdgeA.z);
        }
        // The tricky case, compute the distances and get the min from the 2D lines
        // given from the geometry shader.
        else
        {
            // Compute and compare the sqDist, do one sqrt in the end.

            float2 AF = float2(inputPos.x, inputPos.y) - float2(inputEdgeA.x, inputEdgeA.y);
            float sqAF = dot(AF, AF);
            float AFcosA = dot(AF, float2(inputEdgeA.z, inputEdgeA.w));
            dist = abs(sqAF - AFcosA * AFcosA);

            float2 BF = float2(inputPos.x, inputPos.y) - float2(inputEdgeB.x, inputEdgeB.y);
            float sqBF = dot(BF, BF);
            float BFcosB = dot(BF, float2(inputEdgeB.z, inputEdgeB.w));
            dist = min(dist, abs(sqBF - BFcosB * BFcosB));

            // Only need to care about the 3rd edge for some cases.
            if (1 == inputCase || 2 == inputCase || 4 == inputCase)
            {
                float AFcosA0 = dot(AF, normalize(float2(inputEdgeB.x, inputEdgeB.y) - float2(inputEdgeA.x, inputEdgeA.y)));
                dist = min(dist, abs(sqAF - AFcosA0 * AFcosA0));
            }

            dist = sqrt(dist);
        }

        return dist;
    }

    // Shader main entry.
    bool DemoTriangleShaderWireframeDefaultPixelShader::Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth)
    {
        const float lineWidth = 2.0f;
        const float fadeDistance = 50.0f;
        const float patternPeriod = 1.5f;

        const float4 fillColor = float4(0.1f, 0.2f, 0.4f, 1.0f);
        const float4 wireColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
        const float4 patternColor = float4(1.0f, 1.0f, 0.5f, 1.0f);

        // Compute the shortest distance between the fragment and the edges.
        float dist = EvalMinDistanceToEdges(input);

        // Cull fragments too far from the edge.
        if (dist > 0.5f * lineWidth + 1.0f)
        {
            return false;
        }

        // Map the computed distance to the [0,2] range on the border of the line.
        dist = Clamp((dist - (0.5f * lineWidth - 1.0f)), 0.0f, 2.0f);

        // Alpha is computed from the function exp2(-2(x)^2).
        dist *= dist;
        float alpha = exp2(-2.0f * dist);

        // Standard wire color
        color = wireColor;
        color.a *= alpha;

        if (color.a <= 0.0f)
        {
            return false;
        }

        return true;
    }

    // ------------------------------------------------------------------
    // Wireframe pattern pixel shader.

    bool DemoTriangleShaderWireframePatternPixelShader::MightKillPixels()
    {
        //return false; // Can use alpha blend.
        return true; // Discard pixel if pixel shader return false.
    }

    float DemoTriangleShaderWireframePatternPixelShader::EvalMinDistanceToEdgesExt(const Yw3dShaderRegister* input, float3& edgeSqDists, float3& edgeCoords, uint32_t& edgeOrder0, uint32_t& edgeOrder1, uint32_t& edgeOrder2)
    {
        // Get input value.
        const float4& inputPos = input[0];
        const float4& inputEdgeA = input[1];
        const float4& inputEdgeB = input[2];
        const int32_t inputCase = (int32_t)input[3].x;

        // The easy case, the 3 distances of the fragment to the 3 edges is already
        // computed, get the min.
        if (0 == inputCase)
        {
            edgeSqDists = float3(inputEdgeA) * float3(inputEdgeA);
            edgeCoords = float3(inputEdgeB);
            edgeOrder0 = 0;
            edgeOrder1 = 1;
            edgeOrder2 = 2;

            float inoutEdgeSqDists[3] = { edgeSqDists.x, edgeSqDists.y, edgeSqDists.z };

            if (inoutEdgeSqDists[1] < inoutEdgeSqDists[0])
            {
                std::swap(edgeOrder0, edgeOrder1);
            }

            if (inoutEdgeSqDists[2] < inoutEdgeSqDists[edgeOrder1])
            {
                std::swap(edgeOrder1, edgeOrder2);
            }

            if (inoutEdgeSqDists[2] < inoutEdgeSqDists[edgeOrder0])
            {
                std::swap(edgeOrder0, edgeOrder1);
            }

            edgeSqDists.x = inoutEdgeSqDists[0];
            edgeSqDists.y = inoutEdgeSqDists[1];
            edgeSqDists.z = inoutEdgeSqDists[2];
        }
        // The tricky case, compute the distances and get the min from the 2D lines
        // given from the geometry shader.
        else
        {
            float2 AF = float2(inputPos) - float2(inputEdgeA);
            float sqAF = dot(AF, AF);
            float AFcosA = dot(AF, float2(inputEdgeA.z, inputEdgeA.w));
            edgeSqDists.x = abs(sqAF - AFcosA * AFcosA);
            edgeOrder0 = 0;
            edgeOrder1 = 1;
            edgeOrder2 = 2;
            edgeCoords.x = abs(AFcosA);

            float2 BF = float2(inputPos) - float2(inputEdgeB);
            float sqBF = dot(BF, BF);
            float BFcosB = dot(BF, float2(inputEdgeB.z, inputEdgeB.w));
            edgeSqDists.y = abs(sqBF - BFcosB * BFcosB);
            edgeCoords.y = abs(BFcosB);

            float inoutEdgeSqDists[3] = { edgeSqDists.x, edgeSqDists.y, edgeSqDists.z };

            if (inoutEdgeSqDists[1] < inoutEdgeSqDists[0])
            {
                std::swap(edgeOrder0, edgeOrder1);
            }

            if (1 == inputCase || 2 == inputCase || 4 == inputCase)
            {
                float AFcosA0 = dot(AF, normalize(float2(inputEdgeB) - float2(inputEdgeA)));
                edgeSqDists.z = abs(sqAF - AFcosA0 * AFcosA0);
                edgeCoords.z = abs(AFcosA0);

                inoutEdgeSqDists[0] = edgeSqDists.x;
                inoutEdgeSqDists[1] = edgeSqDists.y;
                inoutEdgeSqDists[2] = edgeSqDists.z;

                if (inoutEdgeSqDists[2] < inoutEdgeSqDists[edgeOrder1])
                {
                    std::swap(edgeOrder1, edgeOrder2);
                }

                if (inoutEdgeSqDists[2] < inoutEdgeSqDists[edgeOrder0])
                {
                    std::swap(edgeOrder0, edgeOrder1);
                }
            }
            else
            {
                inoutEdgeSqDists[2] = 0;
                edgeCoords.z = 0;
            }

            edgeSqDists.x = inoutEdgeSqDists[0];
            edgeSqDists.y = inoutEdgeSqDists[1];
            edgeSqDists.z = inoutEdgeSqDists[2];
        }

        float edgeSqDistsArray[3] = { edgeSqDists.x, edgeSqDists.y, edgeSqDists.z };
        return sqrt(edgeSqDistsArray[edgeOrder0]);
    }

    // Shader main entry.
    bool DemoTriangleShaderWireframePatternPixelShader::Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth)
    {
        const float lineWidth = 4.0f;
        const float fadeDistance = 50;
        const float patternPeriod = 1.5;

        const float4 fillColor = float4(0.1f, 0.2f, 0.4f, 1.0f);
        const float4 wireColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
        const float4 patternColor = float4(1.0f, 1.0f, 0.5f, 1.0f);

        // Constants.
        const uint32_t infoA[] = { 0, 0, 0, 0, 1, 1, 2 };
        const uint32_t infoB[] = { 1, 1, 2, 0, 2, 1, 2 };
        const uint32_t infoAd[] = { 2, 2, 1, 1, 0, 0, 0 };
        const uint32_t infoBd[] = { 2, 2, 1, 2, 0, 2, 1 };
        const uint32_t infoEdge0[] = { 0, 2, 0, 0, 0, 0, 2 };

        // Get input value.
        const float4& inputPos = input[0];
        const float4& inputEdgeA = input[1];
        const float4& inputEdgeB = input[2];
        const int32_t inputCase = (int32_t)input[3].x;

        // Compute the shortest square distance between the fragment and the edges.
        float3 edgeSqDists;
        float3 edgeCoords;
        uint32_t edgeOrder0 = 0xffffffff;
        uint32_t edgeOrder1 = 0xffffffff;
        uint32_t edgeOrder2 = 0xffffffff;
        float dist = EvalMinDistanceToEdgesExt(input, edgeSqDists, edgeCoords, edgeOrder0, edgeOrder1, edgeOrder2);

        float outputEdgeSqDists[3] = { edgeSqDists.x, edgeSqDists.y, edgeSqDists.z };
        float outputEdgeCoords[3] = { edgeCoords.x, edgeCoords.y, edgeCoords.z };

        // Standard wire color
        color = wireColor;
        float realLineWidth = 0.5f * lineWidth;

        // Except if on the diagonal edge
        if (infoEdge0[inputCase] == edgeOrder0)
        {
            if (dist > lineWidth + 1.0f)
            {
                color.a = 0.0f;
                return false;
            }

            float patternPos = ((int32_t)abs(outputEdgeCoords[edgeOrder0]) % (int32_t)(patternPeriod * 2.0f * lineWidth)) - lineWidth;
            dist = (patternPos * patternPos + dist * dist);

            color = patternColor;
            realLineWidth = lineWidth;

            // Filling the corners near the vertices with the WireColor
            if (outputEdgeSqDists[edgeOrder1] < pow(0.5f * lineWidth + 1.0f, 2.0f))
            {
                dist = outputEdgeSqDists[edgeOrder1];
                color = wireColor;
                realLineWidth = 0.5f * lineWidth;
            }

            dist = sqrt(dist);
        }
        // Cull fragments too far from the edge.
        else if (dist > 0.5f * lineWidth + 1.0f)
        {
            color.a = 0.0f;
            return false;
        }

        // Map the computed distance to the [0,2] range on the border of the line.
        dist = Clamp((dist - (realLineWidth - 1.0f)), 0.0f, 2.0f);

        // Alpha is computed from the function exp2(-2(x)^2).
        dist *= dist;
        float alpha = exp2(-2.0f * dist);

        color.a *= alpha;

        return true;
    }
}

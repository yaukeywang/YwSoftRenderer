// Add by Yaukey at 2020-02-02.
// YW Soft Renderer demo with triangle shader wireframe shader class.

#include "YwDemoTriangleShaderWireframeShader.h"

namespace yw
{
    // ------------------------------------------------------------------
    // Wireframe vertex shader.

    void DemoTriangleShaderWireframeVertexShader::Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput)
    {
        // Get current position.
        Vector4 curPos = vsShaderInput[0];
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
        Vector4 inputPos0 = shaderRegister0[0];
        Vector4 inputPos1 = shaderRegister1[0];
        Vector4 inputPos2 = shaderRegister2[0];

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
        const Matrix44& viewportMatrix = GetMatrix(0);
        Vector2 points[3];
        points[0] = inputPos0 * viewportMatrix;
        points[1] = inputPos1 * viewportMatrix;
        points[2] = inputPos2 * viewportMatrix;

        // If Case is 0, all projected points are defined, do the
        // general case computation
        if (curCase == 0)
        {
            // Compute the edges vectors of the transformed triangle
            Vector2 edges[3];
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
            cosAngles[0] = Vector2Dot(-edges[2], edges[0]) / (lengths[2] * lengths[0]);
            cosAngles[1] = Vector2Dot(-edges[0], edges[1]) / (lengths[0] * lengths[1]);
            cosAngles[2] = Vector2Dot(-edges[1], edges[2]) / (lengths[1] * lengths[2]);

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
            Vector4& output0EdgeA = shaderRegister0[1];
            output0EdgeA.x = 0;
            output0EdgeA.y = heights[0];
            output0EdgeA.z = 0;

            // Vertex 0 EdgeB.
            Vector4& output0EdgeB = shaderRegister0[2];
            output0EdgeB.x = edgeOffsets[0];
            output0EdgeB.y = edgeOffsets[1] + edgeSigns[1] * cosAngles[1] * lengths[0];
            output0EdgeB.z = edgeOffsets[2] + edgeSigns[2] * lengths[2];

            // Vertex 1 Projective pos.
            // Vertex 1 EdgeA.
            Vector4& output1EdgeA = shaderRegister1[1];
            output1EdgeA.x = 0;
            output1EdgeA.y = 0;
            output1EdgeA.z = heights[1];

            // Vertex 1 EdgeB.
            Vector4& output1EdgeB = shaderRegister1[2];
            output0EdgeB.x = edgeOffsets[0] + edgeSigns[0] * lengths[0];
            output0EdgeB.y = edgeOffsets[1];
            output0EdgeB.z = edgeOffsets[2] + edgeSigns[2] * cosAngles[2] * lengths[1];

            // Vertex 2 Projective pos.
            // Vertex 2 EdgeA.
            Vector4& output2EdgeA = shaderRegister2[1];
            output2EdgeA.x = heights[2];
            output2EdgeA.y = 0;
            output2EdgeA.z = 0;

            // Vertex 2 EdgeB.
            Vector4& output2EdgeB = shaderRegister2[2];
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
            Vector2 tmpEdgeAXY = points[infoA[curCase]];
            shaderRegister0[1].x = tmpEdgeAXY.x;
            shaderRegister0[1].y = tmpEdgeAXY.y;
            shaderRegister1[1].x = tmpEdgeAXY.x;
            shaderRegister1[1].y = tmpEdgeAXY.y;
            shaderRegister2[1].x = tmpEdgeAXY.x;
            shaderRegister2[1].y = tmpEdgeAXY.y;

            // Vertex EdgeB x-y.
            //output.EdgeB.xy = points[infoB[output.Case]];
            Vector2 tmpEdgeBXY = points[infoB[curCase]];
            shaderRegister0[2].x = tmpEdgeBXY.x;
            shaderRegister0[2].y = tmpEdgeBXY.y;
            shaderRegister1[2].x = tmpEdgeBXY.x;
            shaderRegister1[2].y = tmpEdgeBXY.y;
            shaderRegister2[2].x = tmpEdgeBXY.x;
            shaderRegister2[2].y = tmpEdgeBXY.y;

            // Vertex EdgeA z-w.
            //output.EdgeA.zw = normalize(output.EdgeA.xy - points[infoAd[output.Case]]);
            Vector2 edgeAZW = (tmpEdgeAXY - points[infoAd[curCase]]).Normalize();
            shaderRegister0[1].z = edgeAZW.x;
            shaderRegister0[1].w = edgeAZW.y;
            shaderRegister1[1].z = edgeAZW.x;
            shaderRegister1[1].w = edgeAZW.y;
            shaderRegister2[1].z = edgeAZW.x;
            shaderRegister2[1].w = edgeAZW.y;

            // Vertex EdgeB z-w.
            //output.EdgeB.zw = normalize(output.EdgeB.xy - points[infoBd[output.Case]]);
            Vector2 edgeBZW = (tmpEdgeBXY - points[infoBd[curCase]]).Normalize();
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
        return false;
    }

    float DemoTriangleShaderWireframeDefaultPixelShader::EvalMinDistanceToEdges(const Yw3dShaderRegister* input)
    {
        // Get input value.
        const Vector4& inputPos = input[0];
        const Vector4& inputEdgeA = input[1];
        const Vector4& inputEdgeB = input[2];
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

            Vector2 AF = Vector2(inputPos.x, inputPos.y) - Vector2(inputEdgeA.x, inputEdgeA.y);
            float sqAF = Vector2Dot(AF, AF);
            float AFcosA = Vector2Dot(AF, Vector2(inputEdgeA.z, inputEdgeA.w));
            dist = abs(sqAF - AFcosA * AFcosA);

            Vector2 BF = Vector2(inputPos.x, inputPos.y) - Vector2(inputEdgeB.x, inputEdgeB.y);
            float sqBF = Vector2Dot(BF, BF);
            float BFcosB = Vector2Dot(BF, Vector2(inputEdgeB.z, inputEdgeB.w));
            dist = min(dist, abs(sqBF - BFcosB * BFcosB));

            // Only need to care about the 3rd edge for some cases.
            if (1 == inputCase || 2 == inputCase || 4 == inputCase)
            {
                float AFcosA0 = Vector2Dot(AF, (Vector2(inputEdgeB.x, inputEdgeB.y) - Vector2(inputEdgeA.x, inputEdgeA.y)).Normalize());
                dist = min(dist, abs(sqAF - AFcosA0 * AFcosA0));
            }

            dist = sqrt(dist);
        }

        return dist;
    }

    // Shader main entry.
    bool DemoTriangleShaderWireframeDefaultPixelShader::Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth)
    {
        const float lineWidth = 4.0f;
        const float fadeDistance = 50;
        const float patternPeriod = 1.5;

        const Vector4 fillColor = Vector4(0.1f, 0.2f, 0.4f, 1.0f);
        const Vector4 wireColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        const Vector4 patternColor = Vector4(1.0f, 1.0f, 0.5f, 1.0f);

        // Compute the shortest distance between the fragment and the edges.
        float dist = EvalMinDistanceToEdges(input);

        // Cull fragments too far from the edge.
        if (dist > 0.5f * lineWidth + 1)
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
        return false;
    }

    float DemoTriangleShaderWireframePatternPixelShader::EvalMinDistanceToEdgesExt(const Yw3dShaderRegister* input, Vector3& edgeSqDists, Vector3& edgeCoords, uint32_t& edgeOrder0, uint32_t& edgeOrder1, uint32_t& edgeOrder2)
    {
        // Get input value.
        const Vector4& inputPos = input[0];
        const Vector4& inputEdgeA = input[1];
        const Vector4& inputEdgeB = input[2];
        const int32_t inputCase = (int32_t)input[3].x;

        // The easy case, the 3 distances of the fragment to the 3 edges is already
        // computed, get the min.
        if (0 == inputCase)
        {
            edgeSqDists = Vector3(inputEdgeA) * Vector3(inputEdgeA);
            edgeCoords = Vector3(inputEdgeB);
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
            float2 AF = Vector2(inputPos) - Vector2(inputEdgeA);
            float sqAF = Vector2Dot(AF, AF);
            float AFcosA = Vector2Dot(AF, Vector2(inputEdgeA.z, inputEdgeA.w));
            edgeSqDists.x = abs(sqAF - AFcosA * AFcosA);
            edgeOrder0 = 0;
            edgeOrder1 = 1;
            edgeOrder2 = 2;
            edgeCoords.x = abs(AFcosA);

            float2 BF = Vector2(inputPos) - Vector2(inputEdgeB);
            float sqBF = Vector2Dot(BF, BF);
            float BFcosB = Vector2Dot(BF, Vector2(inputEdgeB.z, inputEdgeB.w));
            edgeSqDists.y = abs(sqBF - BFcosB * BFcosB);
            edgeCoords.y = abs(BFcosB);

            float inoutEdgeSqDists[3] = { edgeSqDists.x, edgeSqDists.y, edgeSqDists.z };

            if (inoutEdgeSqDists[1] < inoutEdgeSqDists[0])
            {
                std::swap(edgeOrder0, edgeOrder1);
            }

            if (1 == inputCase || 2 == inputCase || 4 == inputCase)
            {
                float AFcosA0 = Vector2Dot(AF, (Vector2(inputEdgeB) - Vector2(inputEdgeA)).Normalize());
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

        const Vector4 fillColor = Vector4(0.1f, 0.2f, 0.4f, 1.0f);
        const Vector4 wireColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        const Vector4 patternColor = Vector4(1.0f, 1.0f, 0.5f, 1.0f);

        // Constants.
        const uint32_t infoA[] = { 0, 0, 0, 0, 1, 1, 2 };
        const uint32_t infoB[] = { 1, 1, 2, 0, 2, 1, 2 };
        const uint32_t infoAd[] = { 2, 2, 1, 1, 0, 0, 0 };
        const uint32_t infoBd[] = { 2, 2, 1, 2, 0, 2, 1 };
        const uint32_t infoEdge0[] = { 0, 2, 0, 0, 0, 0, 2 };

        // Get input value.
        const Vector4& inputPos = input[0];
        const Vector4& inputEdgeA = input[1];
        const Vector4& inputEdgeB = input[2];
        const int32_t inputCase = (int32_t)input[3].x;

        // Compute the shortest square distance between the fragment and the edges.
        Vector3 edgeSqDists;
        Vector3 edgeCoords;
        uint32_t edgeOrder0;
        uint32_t edgeOrder1;
        uint32_t edgeOrder2;
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
            dist = (patternPos*patternPos + dist * dist);

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

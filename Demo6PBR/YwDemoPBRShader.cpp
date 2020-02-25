// Add by Yaukey at 2020-02-24.
// YW Soft Renderer demo with PBR shader class.

#include "YwDemoPBRShader.h"

namespace yw
{
    // ------------------------------------------------------------------
    // PBR vertex shader.

    void DemoPBRVertexShader::Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput)
    {
        // The projection vertex position.
        position = vsShaderInput[0] * (*GetWVPMatrix());

        // Model normal.
        //vsShaderOutput[0] = vsShaderInput[1];

        // Get light and view direction.
        const Vector4& lightDir = GetVector(0);
        const Vector4& viewDir = GetVector(1);

        // Get world inverse matrix.
        const Matrix44& worldInverse = GetMatrix(0);

        // Transform light and view direction into model space.
        Vector4 modelLightDir = lightDir * worldInverse;
        Vector4 modelViewDir = viewDir * worldInverse;

        //// Other vertex attribute.
        vsShaderOutput[0] = -modelLightDir;
        vsShaderOutput[1] = -modelViewDir;

        // Other vertex attribute.
        vsShaderOutput[2] = vsShaderInput[4];
        vsShaderOutput[3] = vsShaderInput[5];

        const Vector3 normal = vsShaderInput[1];
        const Vector4 tangent = vsShaderInput[2];
        const Vector3 binormal = Vector3Cross(Vector3(), normal, Vector3(tangent.x, tangent.y, tangent.z)) * tangent.w;
        vsShaderOutput[4] = Vector3(tangent);
        vsShaderOutput[5] = binormal;
        vsShaderOutput[6] = normal;
    }

    Yw3dShaderRegisterType DemoPBRVertexShader::GetOutputRegisters(uint32_t shaderRegister)
    {
        switch (shaderRegister)
        {
        case 0:
            return Yw3d_SRT_Vector3;    // Vertex normal.
        case 1:
            return Yw3d_SRT_Vector4;    // Vertex tangent.
        case 2:
            return Yw3d_SRT_Vector4;    // Vertex color.
        case 3:
            return Yw3d_SRT_Vector2;    // Vertex texcoord.
        case 4:
            return Yw3d_SRT_Vector2;    // Vertex texcoord2.
        case 5:
            return Yw3d_SRT_Vector3;
        case 6:
            return Yw3d_SRT_Vector3;
        case 7:
            return Yw3d_SRT_Vector3;
        default:
            return Yw3d_SRT_Unused;
        }
    }

    // ------------------------------------------------------------------
    // PBR default pixel shader.

    bool DemoPBRPixelShader::MightKillPixels()
    {
        return false;
    }

    // Shader main entry.
    bool DemoPBRPixelShader::Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth)
    {
        // Form TBN matrix from tangent space to model space.
        const Vector3 tangent = Vector3(input[4]).Normalize();
        const Vector3 binormal = Vector3(input[5]).Normalize();
        const Vector3 normal = Vector3(input[6]).Normalize();
        Matrix33 TBN = Matrix33(
            tangent.x, tangent.y, tangent.z,
            binormal.x, binormal.y, binormal.z,
            normal.x, normal.y, normal.z
        );

        // Sample main texture.
        Vector4 vDdx, vDdy;
        GetPartialDerivatives(2, vDdx, vDdy);
        Vector2 texCoord = input[2];
        Vector4 texColor;
        SampleTexture(texColor, 0, texCoord.x, texCoord.y, 0.0f, &vDdx, &vDdy);

        // Sample normal texture.
        Vector4 normalTexColor;
        SampleTexture(normalTexColor, 1, texCoord.x, texCoord.y, 0.0f, &vDdx, &vDdy);

        Vector3 normalTangent = normalTexColor * 2 - Vector4(1.0f, 1.0f, 1.0f, 0.0f);
        Vector3 normalModel = Vector3Normalize(Vector3(), normalTangent * TBN);

        Vector3 modelLightDir = Vector3(input[0]).Normalize();
        Vector3 modelViewDir = Vector3(input[1]).Normalize();

        // Get half vector.
        Vector3 h = (modelLightDir + modelViewDir);

        // Get diffuse.
        float diff = max(0.0f, Vector3Dot(normalModel, modelLightDir));

        // Get N dot H.
        float nh = max(0.0f, Vector3Dot(normalModel, h));

        // Get specular.
        float specular = GetFloat(0);
        float gloss = GetFloat(1);
        float spec = pow(nh, specular * 128.0f) * gloss;

        // Get light color.
        Vector4 lightColor = GetVector(0);
        Vector4 albedo = GetVector(1);
        Vector4 specColor = GetVector(2);

        Vector4 c = albedo * texColor + lightColor * diff * texColor * 1.3f + lightColor * specColor * spec * 1.8f;
        c.a = 1.0f;

        color = c;
        return true;
    }
}

// Add by Yaukey at 2018-02-22.
// YW Soft Renderer 3d shader class (vertex-shader, triangle-shader and pixel shader).

#ifndef __YW_3D_SHADER_H__
#define __YW_3D_SHADER_H__

#include "Yw3dBase.h"
#include "Yw3dTypes.h"
#include "Yw3dBaseShader.h"

namespace yw
{
    // Defines the vertex shader interface.
    class IYw3dVertexShader : public IYw3dBaseShader
    {
        friend class Yw3dDevice;

    protected:
        // Accessible by Yw3dDevice.
        // This is the core function of a vertex shader: It transforms vertex positions to homogeneous clipping space and sets up registers for the pixel shader.
        // @param[in] vsShaderInput vertex shader input registers, data is loaded from the active vertex streams.
        // @param[out] position vertex position transformed to homogeneous clipping space.
        // @param[out] vsShaderOutput vertex shader output registers which will be interpolated and passed to the pixel shader.
        virtual void Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput) = 0;

        // Returns the type of a particular output register. Member of the enumeration Yw3dShaderRegType; if a given register is not used, return Yw3d_SRT_Unused.
        // @param[in] shaderRegister index of register, e [0,YW3D_PIXEL_SHADER_REGISTERS].
        virtual Yw3dShaderRegisterType GetOutputRegisters(uint32_t shaderRegister) = 0;
    };

    // Defines the triangle shader interface.
    class IYw3dTriangleShader : public IYw3dBaseShader
    {
        friend class Yw3dDevice;

    protected:
        // Accessible by Yw3dDevice.
        // This is the core function of a triangle shader: It performs per triangle operations on vertices.
        // @param[in,out] shaderRegister0 shader registers outputted from vertex shader for the first triangle vertex.
        // @param[in,out] shaderRegister1 shader registers outputted from vertex shader for the second triangle vertex.
        // @param[in,out] shaderRegister2 shader registers outputted from vertex shader for the third triangle vertex.
        // @note A triangle shader may operate on the registers outputted by the vertex shader.
        // @return false if triangle should not be rendered.
        virtual bool Execute(Yw3dShaderRegister* shaderRegister0, Yw3dShaderRegister* shaderRegister1, Yw3dShaderRegister* shaderRegister2) = 0;
    };

    // Defines the pixel shader interface.
    class IYw3dPixelShader : public IYw3dBaseShader
    {
        friend class Yw3dDevice;

    protected:
        // Accessible by Yw3dDevice which is the only class that may create.
        IYw3dPixelShader();

    protected:
        // Accessible by Yw3dDevice. Returns the type of the pixel shader; member of the enumeration Yw3dPixelShaderOutput. Default: Yw3d_PSO_ColorOnly.
        virtual Yw3dPixelShaderOutput GetShaderOutput() { return Yw3d_PSO_ColorOnly; }

        // Returns true in case support for pixel-killing for Yw3d_PSO_ColorOnly-shader-types shall be enabled.
        virtual bool MightKillPixels() { return true; }

        // Accessible by Yw3dDevice.
        // This is the core function of a pixel shader: It receives interpolated register data from the vertex shader and can output a new color and depth value for the pixel currently being drawn.
        // @param[in] input pixel shader input registers, which have been set up in the vertex shader and interpolated during rasterization.
        // @param[in,out] color contains the value of the pixel in the rendertarget when Execute() is called. The pixel shader may perform blending with this value and setting it to a new color. Make sure to override GetShaderOutput() to the correct shader type.
        // @param[in,out] depth contains the depth of the pixel in the rendertarget when Execute() is called. The pixel shader may set this to a new value. Make sure to override GetShaderOutput() to the correct shader type.
        // @return true if the pixel shall be written to the rendertarget, false in case it shall be killed.
        virtual bool Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth) = 0;

    protected:
        // Accessible by Yw3dDevice - Sets the triangle info.
        // @param[in] vsOutputs pointer to the pixel shader input register-types.
        // @param[in] triangleInfo pointer to the triangle info structure.
        void SetInfo(const Yw3dShaderRegisterType* vsOutputs, const struct Yw3dTriangleInfo* triangleInfo);

        // This functions computes the partial derivatives of a shader register with respect to the screen space coordinates.
        // @param[in] register index of the source shader register.
        // @param[out] ddx partial derivative with respect to the x-screen space coordinate.
        // @param[out] ddy partial derivative with respect to the y-screen space coordinate.
        void GetPartialDerivatives(uint32_t register, Vector4& ddx, Vector4& ddy) const;

    protected:
        // Sample texture color.
        // @param[in] shaderRegister texture binded shader register index.
        // @param[in] samplerNumber texture binded texture sampler index.
        // @param[in] s texture sample address.
        // @param[in] texelOff Offset to be added to obtain the final texel.
        // @return sampled texture color, return Pure-Black(0,0,0,0) if no texture found.
        inline Vector4 tex2D(uint32_t shaderRegister, uint32_t samplerNumber, const Vector2& s, int32_t texelOff = 0)
        {
            // Get ddx and ddy for mipmap.
            Vector4 vDdx, vDdy;
            GetPartialDerivatives(shaderRegister, vDdx, vDdy);

            // Sample the texture.
            Vector4 texColor;
            SampleTexture(texColor, samplerNumber, s.x, s.y, 0.0f, -1.0f, &vDdx, &vDdy);

            // Return sampled texture color.
            return texColor;
        }

        // Sample texture color.
        // @param[in] shaderRegister texture binded shader register index.
        // @param[in] samplerNumber texture binded texture sampler index.
        // @param[in] s texture sample address.
        //            s.xy Coordinates to perform the lookup. 
        //            s.w Level of detail.
        // @param[in] texelOff Offset to be added to obtain the final texel.
        // @return sampled texture color, return Pure-Black(0,0,0,0) if no texture found.
        inline Vector4 tex2Dlod(uint32_t shaderRegister, uint32_t samplerNumber, const Vector4& s, int32_t texelOff = 0)
        {
            // Get ddx and ddy for mipmap only when lod is greater than 0.
            Vector4* pDdx = nullptr;
            Vector4* pDdy = nullptr;

            // Prepare to get ddx and ddy.
            Vector4 vDdx, vDdy;
            if (s.w < 0.0f)
            {
                GetPartialDerivatives(shaderRegister, vDdx, vDdy);
                pDdx = &vDdx;
                pDdy = &vDdy;
            }

            // Sample the texture.
            Vector4 texColor;
            SampleTexture(texColor, samplerNumber, s.x, s.y, 0.0f, s.w, pDdx, pDdy);

            // Return sampled texture color.
            return texColor;
        }

        // Sample cube texture color.
        // @param[in] shaderRegister texture binded shader register index.
        // @param[in] samplerNumber texture binded texture sampler index.
        // @param[in] s Coordinates to perform the lookup.
        // @return sampled texture color, return Pure-Black(0,0,0,0) if no texture found.
        inline Vector4 texCUBE(uint32_t shaderRegister, uint32_t samplerNumber, const Vector3& s)
        {
            // Get ddx and ddy for mipmap.
            Vector4 vDdx, vDdy;
            GetPartialDerivatives(shaderRegister, vDdx, vDdy);

            // Sample the texture.
            Vector4 texColor;
            SampleTexture(texColor, samplerNumber, s.x, s.y, s.z, -1.0f, &vDdx, &vDdy);

            // Return sampled texture color.
            return texColor;
        }

        // Sample cube texture color with specified level of detail.
        // @param[in] shaderRegister texture binded shader register index.
        // @param[in] samplerNumber texture binded texture sampler index.
        // @param[in] s Coordinates to perform the lookup.
        //            s.xyz Coordinates to perform the lookup. 
        //            s.w Level of detail.
        // @return sampled texture color, return Pure-Black(0,0,0,0) if no texture found.
        inline Vector4 texCUBElod(uint32_t shaderRegister, uint32_t samplerNumber, const Vector4& s)
        {
            // Get ddx and ddy for mipmap.
            Vector4 vDdx, vDdy;
            GetPartialDerivatives(shaderRegister, vDdx, vDdy);

            // Sample the texture.
            Vector4 texColor;
            SampleTexture(texColor, samplerNumber, s.x, s.y, s.z, s.w, &vDdx, &vDdy);

            // Return sampled texture color.
            return texColor;
        }

        // Sample texture color and check the state.
        // @param[out] color receives the color of the pixel to be looked up.
        // @param[in] shaderRegister texture binded shader register index.
        // @param[in] samplerNumber texture binded texture sampler index.
        // @param[in] uv texture sample address.
        // @return true if sampling texture succeed, otherwise false.
        inline bool tex2DSafe(Vector4& color, uint32_t shaderRegister, uint32_t samplerNumber, const Vector2& uv, int32_t texelOff = 0)
        {
            // Get ddx and ddy for mipmap.
            Vector4 vDdx, vDdy;
            GetPartialDerivatives(shaderRegister, vDdx, vDdy);

            // Sample the texture.
            Yw3dResult result = SampleTexture(color, samplerNumber, uv.x, uv.y, 0.0f, -1.0f, &vDdx, &vDdy);

            // Return sample state, success or not.
            return YW3D_SUCCESSFUL(result);
        }

    private:
        // Register type info.
        const Yw3dShaderRegisterType* m_VsOutputs;

        // Gradient info about the triangle that is currently being drawn.
	    const struct Yw3dTriangleInfo* m_TriangleInfo;
    };
}

#endif // !__YW_3D_SHADER_H__

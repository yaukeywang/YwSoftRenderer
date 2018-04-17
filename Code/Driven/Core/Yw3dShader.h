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
        // @param[in] register index of register, e [0,YW3D_PIXEL_SHADER_REGISTERS].
        virtual Yw3dShaderRegisterType GetOutputRegisters(uint32_t register) = 0;
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

        // Returns true incase support for pixel-killing for Yw3d_PSO_ColorOnly-shader-types shall be enabled.
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

    private:
        // Register type info.
        const Yw3dShaderRegisterType* m_VsOutputs;

        // Gradient info about the triangle that is currently being drawn.
	    const struct Yw3dTriangleInfo* m_TriangleInfo;
    };
}

#endif // !__YW_3D_SHADER_H__

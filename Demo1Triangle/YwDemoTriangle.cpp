// Add by Yaukey at 2018-10-10.
// YW Soft Renderer demo triangle class.

#include "YwDemoTriangle.h"
#include "YwScene.h"

namespace yw
{
	// ------------------------------------------------------------------
	// Triangle shader.

	// Triangle vertex shader.
	class DemoTriangleVertexShader : public IYw3dVertexShader
	{
	protected:
		void Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput)
		{
			// The projection vertex position.
			position = vsShaderInput[0] * GetMatrix(Yw3d_SC_Matrix_WVP);

			// Vertex color.
			vsShaderOutput[0] = vsShaderInput[1];
		}

		Yw3dShaderRegisterType GetOutputRegisters(uint32_t shaderRegister)
		{
			switch (shaderRegister)
			{
			case 0:
				// Vertex color.
				return Yw3d_SRT_Vector4;
			default:
				return Yw3d_SRT_Unused;
			}
		}
	};

	// Triangle pixel shader.
	class DemoTrianglePixelShader : public IYw3dPixelShader
	{
	protected:
		bool MightKillPixels()
		{
			return false;
		}

		bool Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth)
		{
			color = input[0];
			return true;
		}
	};

	// ------------------------------------------------------------------
	// Triangle entity.

	DemoTriangle::DemoTriangle(Scene* scene) : 
		IEntity(scene),
		m_VertexFormat(nullptr),
		m_VertexBuffer(nullptr),
		m_IndexBuffer(nullptr),
		m_VertexShader(nullptr),
		m_PixelShader(nullptr),
		m_NumVertices(0),
		m_NumPrimitives(0)
	{
	}

	DemoTriangle::~DemoTriangle()
	{

	}
}

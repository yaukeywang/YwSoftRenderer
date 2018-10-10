// Add by Yaukey at 2018-10-10.
// YW Soft Renderer demo triangle class.

#ifndef __YW_DEMO_TRIANGLE_H__
#define __YW_DEMO_TRIANGLE_H__

#include "Yw3d.h"
#include "YwEntity.h"

namespace yw
{
	class DemoTriangle : public IEntity
	{
	public:
		// Define vertex format.
		struct Vertexformat
		{
			Vector3 m_Position;
			Vector4 m_Color;
		};

	protected:
		// Constructor.
		DemoTriangle(class Scene* scene);

		// Destructor.
		~DemoTriangle();

	public:
		// The instance create function.
		inline static IEntity* CreateDemoTriangle(class Scene* scene)
		{
			return new DemoTriangle(scene);
		}

		// Initial demo triangle.
		bool Initialize(const Vector3& pointA, const Vector3& pointB, const Vector3& pointC);

		// Returns true if the object has been moved -> scenegraph-care.
		bool FrameMove();

		// Render with a specified pass.
		void Render(int32_t pass);

	private:
		// Vertex related.
		Yw3dVertexFormat* m_VertexFormat;
		Yw3dVertexBuffer* m_VertexBuffer;
		Yw3dIndexBuffer* m_IndexBuffer;

		// Shader related.
		class DemoTriangleVertexShader* m_VertexShader;
		class DemoTrianglePixelShader* m_PixelShader;

		// Primitive info.
		uint32_t m_NumVertices;
		uint32_t m_NumPrimitives;
	};
}

#endif // !__YW_DEMO_TRIANGLE_H__

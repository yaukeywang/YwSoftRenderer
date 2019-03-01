// Add by Yaukey at 2019-03-01.
// YW mesh loader base class.

#ifndef __YW_MESH_LOADER_H__
#define __YW_MESH_LOADER_H__

#include "YwBase.h"
#include "YwMath.h"

namespace yw
{
    // Base mesh loader class.
    class IMeshLoader
    {
    public:
        // Constructor.
        IMeshLoader() : m_Scale(1.0f), m_VertexCount(0), m_TriangleCount(0) {}
        
        // Destructor.
        virtual ~IMeshLoader() {}

    public:
        // Load mesh data from a file, classes devired from this need to implement their own.
        // @param[in] fileName the full path of the mesh file.
        // @return true if the function succeeds.
        virtual bool Load(const String& fileName) = 0;

    public:
        // Get scale of mesh.
        // @return the scale parameter.
        inline float GetScale() const
        {
            return m_Scale;
        }

        // Get all vertices.
        // @return the address of the vertex buffer.
        inline const Vector3* GetVertices() const
        {
            return &(m_Vertices[0]);
        }

        // Get all normal of vertices.
        // @return the address of the normal buffer.
        inline const Vector3* GetNormals() const
        {
            return &(m_Normals[0]);
        }

        // Get all triangle indices.
        // @return the address of the triangle index buffer.
        inline const int32_t* GetTriangles() const
        {
            return &(m_Triangles[0]);
        }

        // Get first uv coordinates.
        // @return uv coordinates of the first layer.
        inline const Vector2* GetUV() const
        {
            return &(m_UV[0]);
        }

        // Get second uv coordinates.
        // @return uv coordinates of the second layer.
        inline const Vector2* GetUV2() const
        {
            return &(m_UV2[0]);
        }

    private:
        // The mesh file name.
        String m_Filename;

        // The mesh scale parameters.
        float m_Scale;

        // All mesh vertices.
        std::vector<Vector3> m_Vertices;

        // All mesh normals.
        std::vector<Vector3> m_Normals;

        // All mesh triangles.
        std::vector<int32_t> m_Triangles;

        // All 1st uv coordinates.
        std::vector<Vector2> m_UV;

        // All 2nd uv coordinates.
        std::vector<Vector2> m_UV2;

        // The total vertex count.
        int m_VertexCount;

        // The total triangle count.
        int m_TriangleCount;
    };
}

#endif //!__YW_MESH_LOADER_H__

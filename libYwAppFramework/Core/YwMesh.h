// Add by Yaukey at 2019-03-05.
// YW Soft Renderer mesh struct.

#ifndef __YW_MESH_H__
#define __YW_MESH_H__

#include "YwBase.h"
#include "YwMath.h"

namespace yw
{
    // Mesh triangle.
    struct MeshTriangle
    {
        // Vertex index array.
        std::vector<int32_t> m_VertexIndices;
        
        // Normal index array.
        std::vector<int32_t> m_NormalIndices;
        
        // First layer uv array.
        std::vector<int32_t> m_UVIndices;
        
        // Second layer uv array.
        std::vector<int32_t> m_UV2Indices;
        
        // Constructor.
        MeshTriangle() {}
    };
    
    // The group object in a mesh.
    struct MeshGroup
    {
        // Name of this group.
        StringA m_Name;
        
        // All triangles.
        std::vector<MeshTriangle*> m_Triangles;
        
        // Used material. (Not Implemented Yet!)
        void* m_Material;
        
        // Constructor.
        MeshGroup(StringA groupName) : m_Name(groupName), m_Material(nullptr) {}

        // Destructor.
        ~MeshGroup()
        {
            for (int i = 0; i < m_Triangles.size(); i++)
            {
                MeshTriangle* meshTriangle = m_Triangles[i];
                YW_SAFE_DELETE(meshTriangle);
            }

            m_Triangles.clear();
        }
    };
    
    // The mesh info.
    class Mesh
    {
    public:
        // Constructor.
        Mesh();
        
        // Destructor.
        ~Mesh();

    public:
        MeshGroup* AddGroup(StringA& groupName, MeshGroup* meshGroup);

    public:
        // The mesh name.
        StringA m_MeshName;
        
        // Material name.
        StringA m_MaterialName;
        
        // All mesh vertices.
        std::vector<Vector3> m_Vertices;
        
        // All mesh normals.
        std::vector<Vector3> m_Normals;
        
        // All 1st uv coordinates.
        std::vector<Vector2> m_Texcoords;
        
        // All 2nd uv coordinates.
        std::vector<Vector2> m_SecondaryTexcoords;
        
        // All mesh groups.
        std::vector<MeshGroup*> m_MeshGroups;
        
        // The position of the mesh.
        Vector3 m_Position;
    };
}

#endif // !__YW_MESH_H__


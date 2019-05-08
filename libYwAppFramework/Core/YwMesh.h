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
        int32_t m_VertexIndices[3];
        
        // Normal index array.
        int32_t m_NormalIndices[3];
        
        // First layer uv array.
        int32_t m_TexcoordsIndices[3];
        
        // Second layer uv array.
        int32_t m_Texcoords2Indices[3];

        // index of triangle facet normal.
        
        // Constructor.
        MeshTriangle()
        {
            for (int32_t i = 0; i < 3; i++)
            {
                m_VertexIndices[i] = -1;
                m_NormalIndices[i] = -1;
                m_TexcoordsIndices[i] = -1;
                m_Texcoords2Indices[i] = -1;
            }
        }

        // Destructor.
        ~MeshTriangle()
        {
        }
    };
    
    // The group object in a mesh.
    struct MeshGroup
    {
        // Name of this group.
        StringA m_Name;
        
        // All triangle indices.
        std::vector<uint32_t> m_Triangles;
        
        // Used material. (Not Implemented Yet!)
        void* m_Material;
        
        // Constructor.
        MeshGroup(StringA groupName) : m_Name(groupName), m_Material(nullptr) {}

        // Destructor.
        ~MeshGroup()
        {
            m_Name.clear();
            m_Triangles.clear();
            YW_SAFE_DELETE(m_Material);
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
        MeshGroup* AddGroup(const StringA& groupName, MeshGroup* meshGroup);
        MeshGroup* FindGroup(const StringA& groupName);

    public:
        // The mesh name.
        StringA m_MeshName;
        
        // Material name.
        StringA m_MaterialName;
        
        // All mesh vertices.
        std::vector<Vector3> m_Vertices;

        // All mesh normals.
        std::vector<Vector3> m_FacetNormals;
        
        // All mesh normals.
        std::vector<Vector3> m_Normals;
        
        // All 1st uv coordinates.
        std::vector<Vector2> m_Texcoords;
        
        // All 2nd uv coordinates.
        std::vector<Vector2> m_Texcoord2s;

        // All triangles.
        std::vector<MeshTriangle*> m_Triangles;
        
        // All mesh groups.
        std::vector<MeshGroup*> m_MeshGroups;
        
        // The position of the mesh.
        Vector3 m_Position;
    };
}

#endif // !__YW_MESH_H__


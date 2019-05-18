// Add by Yaukey at 2019-03-05.
// YW Soft Renderer mesh struct.

#ifndef __YW_MESH_H__
#define __YW_MESH_H__

#include "YwBase.h"
#include "YwMath.h"

namespace yw
{
    // Mesh triangle.
    struct ModelTriangle
    {
        // Vertex index array.
        uint32_t m_VertexIndices[3];
        
        // Normal index array.
        uint32_t m_NormalIndices[3];
        
        // First layer uv array.
        uint32_t m_TexcoordsIndices[3];
        
        // Second layer uv array.
        uint32_t m_Texcoords2Indices[3];

        // Facet normal index of triangle.
        uint32_t m_FacetNormalIndex;
        
        // Constructor.
        ModelTriangle()
        {
            for (int32_t i = 0; i < 3; i++)
            {
                m_VertexIndices[i] = 0;
                m_NormalIndices[i] = 0;
                m_TexcoordsIndices[i] = 0;
                m_Texcoords2Indices[i] = 0;
                m_FacetNormalIndex = 0;
            }
        }

        // Destructor.
        ~ModelTriangle()
        {
        }
    };
    
    // The group object in a mesh.
    struct ModelGroup
    {
        // Name of this group.
        StringA m_Name;
        
        // All triangle indices.
        std::vector<uint32_t> m_Triangles;
        
        // Used material. (Not Implemented Yet!)
        void* m_Material;
        
        // Constructor.
        ModelGroup(StringA groupName) : m_Name(groupName), m_Material(nullptr) {}

        // Destructor.
        ~ModelGroup()
        {
            m_Name.clear();
            m_Triangles.clear();
            YW_SAFE_DELETE(m_Material);
        }
    };
    
    // The mesh info.
    class Model
    {
    public:
        // Constructor.
        Model();
        
        // Destructor.
        ~Model();

    public:
        ModelGroup* AddGroup(const StringA& groupName, ModelGroup* meshGroup);
        ModelGroup* FindGroup(const StringA& groupName);

    public:
        // The model name.
        StringA m_Name;
        
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

        // All vertex tangents.
        std::vector<Vector3> m_Tangents;

        // All triangles.
        std::vector<ModelTriangle*> m_Triangles;
        
        // All mesh groups.
        std::vector<ModelGroup*> m_Groups;
        
        // The position of the mesh.
        //Vector3 m_Position;
    };
}

#endif // !__YW_MESH_H__

// Add by Yaukey at 2019-03-05.
// YW Soft Renderer model struct.

#ifndef __YW_MESH_H__
#define __YW_MESH_H__

#include "YwBase.h"
#include "YwMath.h"
#include "Yw3d.h"

namespace yw
{
    // Mesh triangle.
    struct ModelTriangle
    {
        // Vertex index array.
        uint32_t m_PositionIndices[3];
        
        // Normal index array.
        uint32_t m_NormalIndices[3];
        
        // First layer uv array.
        uint32_t m_TexcoordsIndices[3];
        
        // Second layer uv array.
        uint32_t m_Texcoords2Indices[3];

        // Vertex attribute index array.
        uint32_t m_VertexAttributeIndices[3];

        // Facet normal index of triangle.
        uint32_t m_FacetNormalIndex;
        
        // Constructor.
        ModelTriangle()
        {
            for (int32_t i = 0; i < 3; i++)
            {
                m_PositionIndices[i] = 0;
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
    
    // The group object in a model.
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

    // Define model vertex attribute index.
    // $Note: Normal must be exist, default will be position index.
    struct ModelVertexAttributeIndex
    {
        int32_t positionIndex;
        int32_t normalIndex;
        int32_t tangentIndex;
        int32_t colorIndex;
        int32_t texcoordIndex;
        int32_t texcoord2Index;

        ModelVertexAttributeIndex() : positionIndex(-1), normalIndex(-1), tangentIndex(-1), colorIndex(-1), texcoordIndex(-1), texcoord2Index(-1) {}
        ModelVertexAttributeIndex(int32_t position) : positionIndex(position), normalIndex(position), tangentIndex(position), colorIndex(-1), texcoordIndex(-1), texcoord2Index(-1) {}
        ModelVertexAttributeIndex(int32_t position, int32_t normal) : positionIndex(position), normalIndex(normal), tangentIndex(normal), colorIndex(-1), texcoordIndex(-1), texcoord2Index(-1) {}
        ModelVertexAttributeIndex(int32_t position, int32_t normal, int32_t tangent, int32_t color, int32_t texcoord, int32_t texcoord2) : positionIndex(position), normalIndex(normal), tangentIndex(tangent), colorIndex(color), texcoordIndex(texcoord), texcoord2Index(texcoord2) {}
    };

    // Define vertex format.
    struct ModelVertexFormat
    {
        Vector3 position;
        Vector3 normal;
        Vector3 tangent;
        Vector4 color;
        Vector2 texcoord;
        Vector2 texcoord2;

        ModelVertexFormat() {}
    };

    // Model index buffer element.
    struct ModelIndexBufferElement
    {
        Yw3dIndexBuffer* indexBuffer;
        int32_t primitiveCount;

        ModelIndexBufferElement() : indexBuffer(nullptr), primitiveCount(0) {}
        ModelIndexBufferElement(Yw3dIndexBuffer* buffer, int32_t count) : indexBuffer(buffer), primitiveCount(count) {}
    };
    
    // The model info.
    class Model
    {
    public:
        // Constructor.
        Model();

        // Constructor.
        Model(const StringA& modelName, bool readOnly);
        
        // Destructor.
        ~Model();

    public:
        // Add a group.
        ModelGroup* AddGroup(const StringA& groupName);

        // Find a group.
        ModelGroup* FindGroup(const StringA& groupName);

    public:
        // Create all vertex related data.
        bool CreateVertexData(Yw3dDevice* device);

        // Render this model directly with device.
        // Return: How many groups rendered.
        int Render(Yw3dDevice* device) const;

        // Render this model with graphics management.
        // Return: How many groups rendered.
        int Render(class Graphics* graphics) const;

        // If this model data is read-only.
        inline bool ReadOnly() const
        {
            return m_ReadOnly;
        }

    private:
        // Clear all base model data, graphics data can not be re-created after clearing.
        void ClearBaseModelData();

    private:
        // If this model data is read-only.
        bool m_ReadOnly;

    public:
        // ------------------------------------------------------------------
        // Basic model data.

        // The model name.
        StringA m_Name;
        
        // Material name.
        StringA m_MaterialName;
        
        // All model positions.
        std::vector<Vector3> m_Positions;

        // All model normals.
        std::vector<Vector3> m_FacetNormals;
        
        // All model normals.
        std::vector<Vector3> m_Normals;
        
        // All 1st uv coordinates.
        std::vector<Vector2> m_Texcoords;
        
        // All 2nd uv coordinates.
        std::vector<Vector2> m_Texcoord2s;

        // All vertex tangents.
        std::vector<Vector3> m_Tangents;

        // Add vertex colors.
        std::vector<Vector4> m_Colors;

        // All model vertices.
        std::vector<ModelVertexAttributeIndex> m_Vertices;

        // All triangles.
        std::vector<ModelTriangle*> m_Triangles;
        
        // All model groups.
        std::vector<ModelGroup*> m_Groups;

        // The position of the model.
        //Vector3 m_Position;

    public:
        // ------------------------------------------------------------------
        // Data for graphics device.

        // Vertex element declaration.
        static Yw3dVertexElement s_VertexDeclaration[6];

        // The vertex format.
        Yw3dVertexFormat* m_VertexFormat;

        // The vertex buffer.
        Yw3dVertexBuffer* m_VertexBuffer;

        // The index buffer for each group.
        std::vector<ModelIndexBufferElement> m_IndexBuffers;

        // Total vertex count.
        int32_t m_TotalVertexCount;
    };
}

#endif // !__YW_MESH_H__

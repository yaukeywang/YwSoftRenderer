// Add by Yaukey at 2024-06-11.
// YW Soft Renderer mesh struct.

#ifndef __YW_MESH_H__
#define __YW_MESH_H__

#include "YwBase.h"
#include "YwMath.h"
#include "Yw3d.h"

namespace yw
{
    // ------------------------------------------------------------------
    // Base mesh element info.

    // Define vertex format.
    struct MeshVertex
    {
        Vector3 position;
        Vector3 normal;
        Vector4 tangent;
        Vector4 color;
        Vector2 texcoord;
        Vector2 texcoord2;

        MeshVertex() { Reset(); }
        void Reset() { memset(this, 0, sizeof(MeshVertex)); }
    };

    // Index info of model vertex in cache.
    struct MeshVertexIndex
    {
        uint32_t index;
        MeshVertexIndex* next;

        MeshVertexIndex() : index(0), next(nullptr) {}
        MeshVertexIndex(uint32_t vertexIndex, MeshVertexIndex* nextNode) : index(vertexIndex), next(nextNode) {}
    };

    // Model index buffer element.
    struct MeshIndexBufferElement
    {
        Yw3dIndexBuffer* indexBuffer;
        int32_t primitiveCount;

        MeshIndexBufferElement() : indexBuffer(nullptr), primitiveCount(0) {}
        MeshIndexBufferElement(Yw3dIndexBuffer* buffer, int32_t count) : indexBuffer(buffer), primitiveCount(count) {}
    };

    // Mesh triangle.
    struct MeshTriangle
    {
        // Vertex index array.
        uint32_t positionIndices[3];

        // Normal index array.
        uint32_t normalIndices[3];

        // First layer uv array.
        uint32_t texcoordsIndices[3];

        // Second layer uv array.
        uint32_t texcoords2Indices[3];

        // Vertex attribute index array.
        uint32_t vertexIndices[3];

        // Facet normal index of triangle.
        uint32_t facetNormalIndex;

        // Constructor.
        MeshTriangle()
        {
            for (int32_t i = 0; i < 3; i++)
            {
                positionIndices[i] = 0;
                normalIndices[i] = 0;
                texcoordsIndices[i] = 0;
                texcoords2Indices[i] = 0;
                vertexIndices[i] = 0;
                facetNormalIndex = 0;
            }
        }

        // Destructor.
        ~MeshTriangle()
        {
        }
    };

    // The sub-mesh object in a mesh.
    struct SubMesh
    {
        // Name of this sub-mesh.
        StringA name;

        // All triangle indices.
        std::vector<uint32_t> triangles;

        // All triangle vertex indices.
        std::vector<uint32_t> triangleIndices;

        // Used material. (Not Implemented Yet!)
        void* material;

        // Constructor.
        SubMesh(StringA subMeshName) : name(subMeshName), material(nullptr) {}

        // Destructor.
        ~SubMesh()
        {
            name.clear();
            triangles.clear();
            triangleIndices.clear();
            YW_SAFE_DELETE(material);
        }
    };

    // Mesh VBO class.
    class MeshVBO
    {
    public:
        MeshVBO(Yw3dDevice* device, const std::vector<MeshVertex>& vertices, const std::vector<SubMesh*>& allSubMeshes);
        ~MeshVBO();

    public:
        // Get total vertex count.
        inline int32_t GetTotalVertexCount() const
        {
            return m_TotalVertexCount;
        }

        // Render this mesh directly with device.
        // Return: How many sub-meshes rendered.
        int32_t Render(Yw3dDevice* device) const;

        // Render this mesh with graphics management.
        // Return: How many sub-meshes rendered.
        int32_t Render(class Graphics* graphics) const;

    private:
        // Create all VBO data.
        bool Create(Yw3dDevice* device, const std::vector<MeshVertex>& vertices, const std::vector<SubMesh*>& allSubMeshes);

        // Clear all VBO data.
        void Clear();

    private:
        // Vertex element declaration.
        static Yw3dVertexElement s_VertexDeclaration[6];

        // The vertex format.
        Yw3dVertexFormat* m_VertexFormat;

        // The vertex buffer.
        Yw3dVertexBuffer* m_VertexBuffer;

        // The index buffer for each sub-mesh.
        std::vector<MeshIndexBufferElement> m_IndexBuffers;

        // Total vertex count.
        int32_t m_TotalVertexCount;
    };

    // ------------------------------------------------------------------
    // The mesh class.
    class Mesh
    {
    public:
        // Constructor.
        Mesh(const StringA& modelName);

        // Destructor.
        ~Mesh();

    public:
        // Get mesh name.
        inline const StringA& GetName() const
        {
            return m_Name;
        }

        // Add a sub-mesh.
        SubMesh* AddSubMesh(const StringA& subMeshName);

        // Find a sub-mesh.
        SubMesh* FindSubMesh(const StringA& subMeshName);

    public:
        // Create all vertex related data.
        bool CreateVBOData(Yw3dDevice* device);

        // Render this mesh directly with device.
        // Return: How many sub-meshes rendered.
        int32_t Render(Yw3dDevice* device) const;

        // Render this mesh with graphics management.
        // Return: How many sub-meshes rendered.
        int32_t Render(class Graphics* graphics) const;

        // Clear all base mesh data, graphics data can not be re-created after clearing.
        void Clear();

        // Clear base geometry data.
        void ClearBaseData();

        // Clear VBO data.
        void ClearVBOData();

    private:
        // Basic mesh data.

        // The mesh name.
        StringA m_Name;

    public:
        // All raw mesh data, e.g. materials, geometry and so on.

        // Material name.
        StringA m_MaterialName;

        // The root position of the mesh.
        //Vector3 m_RootPosition;

        // All mesh vertex raw positions.
        std::vector<Vector3> m_Positions;

        // All mesh vertex raw facet normals.
        std::vector<Vector3> m_FacetNormals;

        // All mesh vertex raw normals.
        std::vector<Vector3> m_Normals;

        // All mesh vertex raw 1st uv coordinates.
        std::vector<Vector2> m_Texcoords;

        // All mesh vertex raw 2nd uv coordinates.
        std::vector<Vector2> m_Texcoord2s;

        // All mesh vertex raw tangents.
        std::vector<Vector4> m_Tangents;

        // Add mesh vertex raw colors.
        std::vector<Vector4> m_Colors;

    public:
        // All internal geometry and cache data after well organized.

        // All mesh vertices used for creating gfx buffer.
        std::vector<MeshVertex> m_Vertices;

        // $TODO: Remove this.
        // All mesh vertex indices cache.(Used for accelerating data parsing only.)
        std::vector<MeshVertexIndex*> m_VertexIndexCache;

        // All triangles used for creating gfx buffer.
        std::vector<MeshTriangle*> m_Triangles;

        // All sub-meshes.
        std::vector<SubMesh*> m_AllSubMeshes;

    private:
        MeshVBO* m_VBO;
    };
}

#endif // !__YW_MESH_H__

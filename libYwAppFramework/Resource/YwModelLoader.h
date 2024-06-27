// Add by Yaukey at 2019-03-01.
// YW model loader base class.

#ifndef __YW_MODEL_LOADER_H__
#define __YW_MODEL_LOADER_H__

#include "YwBase.h"
#include "YwMath.h"

namespace yw
{
    // ------------------------------------------------------------------
    // Model importer info.
    namespace MeshImporter
    {
        // Bone weight of vertex.
        struct VertexBoneWeight
        {
            int32_t boneIndex;
            float weight;

            VertexBoneWeight();
            ~VertexBoneWeight();
        };

        // Defines properties of each vertex to import.
        struct Vertex
        {
            Vector3 position;
            Vector3 normal;
            Vector4 tangent;
            Vector4 color;
            Vector2 texcoord;
            Vector2 texcoord2;
            Vector2 texcoord3;
            Vector2 texcoord4;
            Vector2 texcoord5;
            Vector2 texcoord6;
            Vector2 texcoord7;
            Vector2 texcoord8;
            std::vector<VertexBoneWeight> boneWeights;

            Vertex();
            ~Vertex();
        };

        // Index info of mesh vertex in cache.
        struct VertexIndex
        {
            uint32_t index;
            VertexIndex* next;

            // Constructors/Destructors.
            VertexIndex();
            VertexIndex(uint32_t vertexIndex, VertexIndex* nextNode);
        };

        typedef std::vector<VertexIndex*> VertexIndexCache;

        // Mesh triangle.
        struct Triangle
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

            // Constructors/Destructors.
            Triangle();
            ~Triangle();
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

            // Constructors/Destructors.
            SubMesh(StringA subMeshName);
            ~SubMesh();
        };

        // The mesh class.
        struct Mesh
        {
            // Raw data read from file.

            // Material name.
            StringA m_MaterialName;

            // The root position of the mesh.
            Vector3 m_RootPosition;

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

            // All internal geometry and cache data after well organized.

            // All mesh vertices.
            std::vector<Vertex> m_Vertices;

            // All mesh vertex bone weights.
            std::vector<VertexBoneWeight> m_BoneWeights;

            // All triangles used for creating gfx buffer.
            std::vector<Triangle*> m_Triangles;

            // All sub-meshes.
            std::vector<SubMesh*> m_AllSubMeshes;

            // Internal use only.
            // All mesh vertex indices cache.(Used for accelerating data parsing only.)
            std::vector<VertexIndexCache*> m_VertexIndexCache;

            // Constructors/Destructors.
            Mesh();
            ~Mesh();
        };
    }

    // ------------------------------------------------------------------
    // Base model loader class.
    class IModelLoader
    {
    public:
        // Constructor.
        IModelLoader() {}
        
        // Destructor.
        virtual ~IModelLoader() {}

    public:
        // Load model data from a file, classes derived from this need to implement their own.
        // @param[in] fileName the full path of the model file.
        // @param[in] device used to create model data.
        // @param[out] model the loaded data to fill.
        // @param[in] modelReadOnly if model is read only, if true, the original model data will be cleared after the GRAPHICS-MODEL-DATA(vertex/index buffer) is created, you can not re-create graphics model data again from souce data.
        // @param[in] calculateNormals always re-calculate model normals or not.
        // @param[in] calculateNormalAngle the angle used to re-calculate normals.
        // @param[in] modelName the name of this model, optional. 
        // @return true if the model loading ok, false if loading failed.
        bool Load(const StringA& fileName, class Yw3dDevice* device, class Model** model, bool modelReadOnly = true, bool calculateNormals = false, float calculateNormalAngle = 90.0f, const StringA* modelName = nullptr);

    private:
        // Load model from kinds of data.
        // @param[in] fileName the full path of the model file.
        // @param[in] data model raw data.
        // @param[in] calculateNormals always re-calculate model normals or not.
        // @param[in] calculateNormalAngle the angle used to re-calculate normals.
        // @param[out] model the loaded data to fill.
        // @return true if the model loading ok, false if loading failed.
        virtual bool LoadFormData(const StringA& fileName, const uint8_t* data, bool calculateNormals, float calculateNormalAngle, class Model* model) = 0;
    };
}

#endif // !__YW_MODEL_LOADER_H__

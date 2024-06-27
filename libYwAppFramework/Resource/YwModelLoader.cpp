// Add by Yaukey at 2019-08-19.
// YW model loader base class.

#include "YwModelLoader.h"
#include "YwModel.h"
#include "YwFileIO.h"

namespace yw
{
    // ------------------------------------------------------------------
    // Model importer info.

    namespace MeshImporter
    {
        // Bone weight of vertex.

        VertexBoneWeight::VertexBoneWeight() :
            boneIndex(-1),
            weight(0.0f)
        {
        }

        VertexBoneWeight::~VertexBoneWeight()
        {
        }

        // Properties of each vertex to import.
        Vertex::Vertex()
        {
        }

        Vertex::~Vertex()
        {
        }

        // Index info of mesh vertex in cache.

        VertexIndex::VertexIndex() :
            index(0),
            next(nullptr)
        {
        }

        VertexIndex::VertexIndex(uint32_t vertexIndex, VertexIndex* nextNode) :
            index(vertexIndex),
            next(nextNode)
        {
        }

        // Mesh triangle.

        Triangle::Triangle()
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

        Triangle::~Triangle()
        {
        }

        // The sub-mesh object in a mesh.

        SubMesh::SubMesh(StringA subMeshName) :
            name(subMeshName),
            material(nullptr)
        {
        }

        // Destructor.
        SubMesh::~SubMesh()
        {
            name.clear();
            triangles.clear();
            triangleIndices.clear();
            YW_SAFE_DELETE(material);
        }

        // The mesh.

        Mesh::Mesh()
        {
        }

        Mesh::~Mesh()
        {
        }
    }

    // ------------------------------------------------------------------
    // Base model loader class.

    bool IModelLoader::Load(const StringA& fileName, Yw3dDevice* device, Model** model, bool modelReadOnly, bool calculateNormals, float calculateNormalAngle, const StringA* modelName)
    {
        if ((fileName.length() <= 0) || (nullptr == model))
        {
            return false;
        }

        if (nullptr == device)
        {
            return false;
        }

        // Create model data.
        YW_SAFE_DELETE(*model);
        (*model) = new Model((nullptr == modelName) ? "" : *modelName, modelReadOnly);

        // Load base model data from file.
        FileIO file;
        uint8_t* modelData = nullptr;
        uint32_t fileSize = file.ReadFile(fileName, &modelData, true);
        if ((0 == fileSize) || (nullptr == modelData))
        {
            return false;
        }

        const uint8_t* objData = modelData;
        if (nullptr == objData)
        {
            return false;
        }

        // Try to load obj model from data.
        Model* objModel = *model;
        bool res = LoadFormData(fileName, objData, calculateNormals, calculateNormalAngle, objModel);
        if (!res)
        {
            YW_SAFE_DELETE_ARRAY(modelData);
            return false;
        }

        // Release file data.
        YW_SAFE_DELETE_ARRAY(modelData);

        // Create mesh VBO data.
        if (!objModel->GetMesh()->CreateVBOData(device))
        {
            YW_SAFE_DELETE(model);
            return nullptr;
        }

        return true;
    }
}

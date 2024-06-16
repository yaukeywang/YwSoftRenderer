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

            VertexBoneWeight() : 
                boneIndex(-1),
                weight(0.0f)
            {
            }
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

            Vertex()
            {
            }
        };

        // Index info of mesh vertex in cache.
        struct VertexIndex
        {
            uint32_t index;
            VertexIndex* next;

            VertexIndex() :
                index(0),
                next(nullptr)
            {
            }

            VertexIndex(uint32_t vertexIndex, VertexIndex* nextNode) : 
                index(vertexIndex),
                next(nextNode)
            {
            }
        };

        typedef std::vector<VertexIndex*> VertexIndexCache;
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

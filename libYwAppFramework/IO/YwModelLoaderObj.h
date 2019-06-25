// Add by Yaukey at 2019-03-01.
// YW mesh loader for obj file class.
// Modified version of glm.h
// Nate Robins, 1997, 2000
// nate@pobox.com, http://www.pobox.com/~nate
// Wavefront OBJ model file format reader/writer/manipulator.
// YW model loader for obj file class.

#ifndef __YW_MODEL_LOADER_OBJ_H__
#define __YW_MODEL_LOADER_OBJ_H__

#include <stdio.h>
#include "YwModelLoader.h"

namespace yw
{
    // Base model loader class.
    class ModelLoaderObj : public IModelLoader
    {
    public:
        // Constructor.
        ModelLoaderObj();

        // Constructor with params.
        ModelLoaderObj(bool calculateNormals, float calculateNormalAngle);
        
        // Destructor.
        ~ModelLoaderObj();
        
    public:
        // Load model data from a file, classes derived from this need to implement their own.
        // @param[in] fileName the full path of the model file.
        // @return Model pointer of the loaded model, null if failed, NOT create vertex/index buffer.
        bool Load(const StringA& fileName, class Model** model);

        // Load model data from a file, classes derived from this need to implement their own.
        // @param[in] fileName the full path of the model file.
        // @return Model pointer of the loaded model, null if failed, CREATE vertex/index buffer.
        bool Load(const StringA& fileName, class Model** model, class Yw3dDevice* device);
        
    private:
        // $TEMP: Read data from obj file.
        const char* ReadDataFromFile(const StringA& fileName);

        // Load Wavefront-Obj form data.
        void LoadWavefrontObjFormData(Model* model, const char* objData, bool calculateNormals, float calculateNormalAngle);

        // First pass of process.
        void FirstPass(Model* model, const char* objData);
        
        // Second pass of process.
        void SecondPass(class Model* model, const char* objData);

        // Normalize model mesh indices from 1-based to 0-based.
        void NormalizeIndices(class Model* model);

        // Calculate facet normals.
        void CalculateFacetNormals(class Model* model);

        // Calculate vertex normals.
        void CalculateVertexNormals(class Model* model, float angle);

        // Calculate vertex tangent.
        void CalculateVertexTangent(class Model* model);

        // Process secondary texture coordinates.
        void ProcessOtherData(class Model* model);

        // Read material.
        // $Implement.
        void ReadMTL(class Model* model, StringA name);

    private:
        /* Node: general purpose node. */
        struct Node
        {
            uint32_t m_Index;
            bool m_Averaged;
            Node* m_Next;

            Node() : m_Index(0), m_Averaged(false), m_Next(nullptr) {}
        };

    private:
        // Calculate normal or not.
        bool m_CalculateNormals;

        // The angle of calculating normal.
        float m_CalculateNormalAngle;
    };
}

#endif // !__YW_MODEL_LOADER_OBJ_H__

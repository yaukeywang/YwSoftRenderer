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
        ModelLoaderObj(bool translateVertex, bool calculateNormals, float calculateNormalAngle);
        
        // Destructor.
        ~ModelLoaderObj();
        
    public:
        // Load model data from a file, classes derived from this need to implement their own.
        // @param[in] fileName the full path of the model file.
        // @return Model pointer of the loaded model, null if failed.
        class Model* Load(const StringA& fileName);
        
    private:
        // $TEMP: Read data from obj file.
        const char* ReadDataFromFile(const StringA& fileName);

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
        // Translate verex postion from obj file or not.
        bool m_TranslateVertex;

        // Calculate normal or not.
        bool m_CalculateNormals;

        // The angle of calculating normal.
        float m_CalculateNormalAngle;
    };
}

#endif // !__YW_MODEL_LOADER_OBJ_H__

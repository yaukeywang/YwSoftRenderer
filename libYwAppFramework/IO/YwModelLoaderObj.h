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
        
        // Destructor.
        ~ModelLoaderObj();

    private:
        // Load model from kinds of data.
        // @param[in] data model raw data.
        // @param[in] calculateNormals always re-calculate model normals or not.
        // @param[in] calculateNormalAngle the angle used to re-calculate normals.
        // @param[out] model the loaded data to fill.
        // @return true if the model loading ok, false if loading failed.
        virtual bool LoadFormData(const uint8_t* data, bool calculateNormals, float calculateNormalAngle, Model* model);
        
    private:
        // Load Wavefront-Obj form data.
        void LoadWavefrontObjFromData(Model* model, const uint8_t* objData, bool calculateNormals, float calculateNormalAngle);

        // First pass of process.
        void FirstPass(Model* model, const char* objData);
        
        // Second pass of process.
        void SecondPass(class Model* model, const char* objData);

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
    };
}

#endif // !__YW_MODEL_LOADER_OBJ_H__

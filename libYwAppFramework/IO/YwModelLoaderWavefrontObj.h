// Add by Yaukey at 2019-03-01.
// YW model loader for obj file class.
// Modified version of glm.h
// Nate Robins, 1997, 2000
// nate@pobox.com, http://www.pobox.com/~nate
// Update at 2020-08-17.
// with some other reference:
// https://github.com/gameknife/SoftRenderer/blob/769eeccc8dedd3b1be0a876db7378f66eed401ac/code/SoftRenderer/SrObjLoader.cpp
// in https://github.com/gameknife/SoftRenderer
// Wavefront OBJ model file format reader/writer/manipulator.
// YW model loader for obj file class.

#ifndef __YW_MODEL_LOADER_WAVEFRONT_OBJ_H__
#define __YW_MODEL_LOADER_WAVEFRONT_OBJ_H__

#include <stdio.h>
#include "YwModelLoader.h"

namespace yw
{
    // Base model loader class.
    class ModelLoaderWavefrontObj : public IModelLoader
    {
    public:
        // Constructor.
        ModelLoaderWavefrontObj();
        
        // Destructor.
        ~ModelLoaderWavefrontObj();

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

        // Second pass of process.
        void LoadBasicData(class Model* model, const char* objData);

        // Calculate facet normals.
        void CalculateFacetNormals(class Model* model);

        // Calculate vertex normals.
        void CalculateVertexNormals(class Model* model, float angle);

        // Calculate vertex tangent.
        void CalculateVertexTangent(class Model* model);

        // Calculate vertex tangent with TBN.
        // http://foundationsofgameenginedev.com/
        // <<Foundations of Game Engine Development>> by Eric Lengyel, Volume 2: Rendering, 7.5 Tangent Space.
        // http://www.terathon.com/code/tangent.html
        void CalculateVertexTangentTBN(class Model* model);

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

#endif // !__YW_MODEL_LOADER_WAVEFRONT_OBJ_H__

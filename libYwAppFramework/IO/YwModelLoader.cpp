// Add by Yaukey at 2019-08-19.
// YW model loader base class.

#include "YwModelLoader.h"
#include "YwModel.h"
#include "YwFileIO.h"

namespace yw
{
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
        LoadFormData(objData, calculateNormals, calculateNormalAngle, objModel);

        // Release file data.
        YW_SAFE_DELETE_ARRAY(modelData);

        // Create vertex data.
        if (!objModel->CreateVertexData(device))
        {
            YW_SAFE_DELETE(model);
            return nullptr;
        }

        return true;
    }
}

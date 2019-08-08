// Add by Yaukey at 2019-03-01.
// YW model loader base class.

#ifndef __YW_MODEL_LOADER_H__
#define __YW_MODEL_LOADER_H__

#include "YwBase.h"
#include "YwMath.h"

namespace yw
{
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
        // @param[in] modelName the name of this model, optional. 
        // @return true if the model loading ok, false if loading failed.
        virtual bool Load(const StringA& fileName, class Yw3dDevice* device, class Model** model, bool modelReadOnly = true, const StringA* modelName = nullptr) = 0;
    };
}

#endif //!__YW_MODEL_LOADER_H__

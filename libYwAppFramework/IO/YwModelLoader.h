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
        // @return Model pointer of the loaded model, null if failed.
        virtual class Model* Load(const StringA& fileName) = 0;
    };
}

#endif //!__YW_MODEL_LOADER_H__

// Add by Yaukey at 2019-06-25.
// YW texture loader base class.

#ifndef __YW_TEXTURE_LOADER_H__
#define __YW_TEXTURE_LOADER_H__

#include "YwBase.h"
#include "YwMath.h"

namespace yw
{
    // Base texture loader class.
    class ITextureLoader
    {
    public:
        // Constructor.
        ITextureLoader() {}

        // Destructor.
        virtual ~ITextureLoader() {}

    public:
        // Load texture data from a file, classes derived from this need to implement their own.
        // @param[in] fileName the full path of the model file.
        // @param[out] texture the loaded data to fill.
        // @return Model pointer of the loaded model, null if failed.
        virtual bool Load(const StringA& fileName, class Yw3dTexture** texture) = 0;
    };
}

#endif //!__YW_TEXTURE_LOADER_H__

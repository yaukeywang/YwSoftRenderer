// Add by Yaukey at 2018-05-23.
// YW Soft Renderer cube map texture class.

#ifndef __YW_3D_CUBE_TEXTURE_H__
#define __YW_3D_CUBE_TEXTURE_H__

#include "Yw3dBase.h"
#include "Yw3dTypes.h"
#include "Yw3dBaseTexture.h"

namespace yw
{
    // Yw3dCubeTexture implements support for cubemaps.
    class Yw3dCubeTexture : public IYw3dBaseTexture
    {
        friend class Yw3dDevice;

    protected:
        // Accessible by Yw3dDevice which is the only class that may create a cube texture.
        // @param[in] device a pointer to the parent Yw3dDevice-object.
        Yw3dCubeTexture(class Yw3dDevice* device);

        // Accessible by IBase. The destructor is called when the reference count reaches zero.
        ~Yw3dCubeTexture();
    };
}

#endif // !__YW_3D_CUBE_TEXTURE_H__


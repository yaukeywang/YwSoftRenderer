// Add by Yaukey at 2018-05-23.
// YW Soft Renderer 3-dimensional image class.

#ifndef __YW_3D_VOLUME_H__
#define __YW_3D_VOLUME_H__

#include "Yw3dBase.h"
#include "Yw3dTypes.h"

namespace yw
{
    // Yw3dVolume implements a 3-dimensional image.
    class Yw3dVolume : public IBase
    {
        friend class Yw3dDevice;

    protected:
        // Accessible by Yw3dDevice which is the only class that may create a volume.
        // @param[in] device a pointer to the parent Yw3dDevice-object.
        Yw3dVolume(class Yw3dDevice* device);

        // Accessible by IBase. The destructor is called when the reference count reaches zero.
        ~Yw3dVolume();
    };
}

#endif // !__YW_3D_VOLUME_H__

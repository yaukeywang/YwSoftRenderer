// Add by Yaukey at 2018-05-23.
// YW Soft Renderer 3-dimensional texture class.

#ifndef __YW_3D_VOLUME_TEXTURE_H__
#define __YW_3D_VOLUME_TEXTURE_H__

#include "Yw3dBase.h"
#include "Yw3dTypes.h"
#include "Yw3dBaseTexture.h"

namespace yw
{
    // Yw3dVolumeTexture implements a 3-dimensional texture.
    class Yw3dVolumeTexture : public IYw3dBaseTexture
    {
        friend class Yw3dDevice;

    protected:
        // Accessible by Yw3dDevice which is the only class that may create a volume texture.
        // @param[in] device a pointer to the parent Yw3dDevice-object.
        Yw3dVolumeTexture(class Yw3dDevice* device);

        // Accessible by IBase. The destructor is called when the reference count reaches zero.
        ~Yw3dVolumeTexture();

    private:
        // Number of mip-levels.
        uint32_t m_MipLevels;

        // Squared dimensions of the base mip-level, used for mip-calculations.
        float m_SquaredWidth;
        float m_SquaredHeight;
        float m_SquaredDepth;

        //  The texture's mip-levels data.
        class Yw3dVolume** m_MipLevelsData;
    };
}

#endif // !__YW_3D_VOLUME_TEXTURE_H__

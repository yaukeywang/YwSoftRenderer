// Add by Yaukey at 2018-07-11.
// YW Soft Renderer 3d device graphics class.

#ifndef __YW_GRAPHICS_H__
#define __YW_GRAPHICS_H__

#include "Yw3d.h"

namespace yw
{
    class Graphics
    {
    public:
        Graphics();
        ~Graphics();

    public:
        inline Yw3d* GetYw3d()
        {
            return m_Yw3d;
        }

        inline Yw3dDevice* GetYw3dDevice()
        {
            return m_Yw3dDevice;
        }

    private:
        Yw3d* m_Yw3d;
        Yw3dDevice* m_Yw3dDevice;
    };
}

#endif // !__YW_GRAPHICS_H__

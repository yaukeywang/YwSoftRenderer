// Add by Yaukey at 2018-01-29.
// YW Soft Renderer 3d header file.

#ifndef __YW_3D_H__
#define __YW_3D_H__

#include "Yw3dBase.h"
#include "Yw3dTypes.h"

namespace yw
{
    // ------------------------------------------------------------------
    // The Yw3d parent-class - "the root of all evil ;)"
    class Yw3d : public IBase
    {
        friend Yw3dResult CreateYw3d(class Yw3d** yw3d);

    protected:
        // Accessible by CreateYw3d() which is the creator-function.
        Yw3d();

        // Accessible by IBase. The destructor is called when the reference count reaches zero.
        ~Yw3d();

    public:
        // Creates a device for rendering.
        // @param[out] device receives the pointer to the device.
        // @param[in] deviceParameters pointer to a Yw3dDeviceParameters-structure.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_OutOfMemory if memory allocation failed.
        Yw3dResult CreateDevice(class Yw3dDevice** device, const Yw3dDeviceParameters* deviceParameters);
    };

    // ------------------------------------------------------------------
    // Creates a Yw3d-instance.
    // @param[out] yw3d receives the pointer to the Yw3d instance.
    // @return Yw3d_S_OK if the function succeeds.
    // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
    // @return Yw3d_E_OutOfMemory if memory allocation failed.
    Yw3dResult CreateYw3d(class Yw3d** yw3d);
}

// ------------------------------------------------------------------
// Include all core-headers.
#include "Yw3dCubeTexture.h"
#include "Yw3dDevice.h"
#include "Yw3dIndexBuffer.h"
#include "Yw3dPrimitiveAssembler.h"
#include "Yw3dRenderTarget.h"
#include "Yw3dShader.h"
#include "Yw3dSurface.h"
#include "Yw3dTexture.h"
#include "Yw3dVertexBuffer.h"
#include "Yw3dVertexFormat.h"
#include "Yw3dVolume.h"
#include "Yw3dVolumeTexture.h"

#endif // !__YW_3D_H__

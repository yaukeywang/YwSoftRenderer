// Add by Yaukey at 2018-05-23.
// YW Soft Renderer cube map texture class.

#include "stdafx.h"
#include "Yw3dCubeTexture.h"
#include "Yw3dDevice.h"
#include "Yw3dTexture.h"
#include "System/Support/YwUtility.h"

namespace yw
{
    Yw3dCubeTexture::Yw3dCubeTexture(Yw3dDevice* device) :
        IYw3dBaseTexture(device)
    {

    }

    // Accessible by IBase. The destructor is called when the reference count reaches zero.
    Yw3dCubeTexture::~Yw3dCubeTexture()
    {

    }
}

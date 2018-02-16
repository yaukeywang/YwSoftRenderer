// Add by Yaukey at 2018-01-30.
// YW Soft Renderer 3d device class.

#ifndef __YW_3D_DEVICE_H__
#define __YW_3D_DEVICE_H__

#include "Yw3dBase.h"
#include "Yw3dTypes.h"

namespace yw
{
    // The Yw3d device.
    class Yw3dDevice : public IBase
    {
        friend class Yw3d;

    protected:
        // Accessible by Yw3d which is the only class that may create a device.
	    // @param[in] yw3d a pointer to the parent Yw3d-object.
	    // @param[in] deviceParameters pointer to a Yw3dDeviceParameters-structure.
        Yw3dDevice(class Yw3d* yw3d, const Yw3dDeviceParameters* deviceParameters);

        // Accessible by IBase. The destructor is called when the reference count reaches zero.
        ~Yw3dDevice();

    protected:
        // Creates and initializes the device.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_OutOfMemory if memory allocation failed.
        // @return Yw3d_E_Unknown if a presenttarget-specific problem was encountered.
        Yw3dResult Create();

    public:

    private:

    };
}

#endif // !__YW_3D_DEVICE_H__

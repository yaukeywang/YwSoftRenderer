// Add by Yaukey at 2018-03-31.
// YW Soft Renderer present target class.

#ifndef __YW_3D_PRESENT_TARGET_H__
#define __YW_3D_PRESENT_TARGET_H__

#include "Yw3dBase.h"
#include "Yw3dTypes.h"

namespace yw
{
    // The base class of the present target.
    class IYw3dPresentTarget : public IBase
    {
    protected:
        // Present-target constructor.
        // @param[in] device a pointer to the parent Yw3dDevice-object.
        IYw3dPresentTarget(class Yw3dDevice* device);

        // Accessible by IBase. The destructor is called when the reference count reaches zero.
        virtual ~IYw3dPresentTarget();

    public:
        // Creates and initializes the presenttarget.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_Unknown if a presenttarget-specific problem was encountered.
        virtual Yw3dResult Create() = 0;

        // Presents the contents of a given rendertarget's colorbuffer.
        // @param[in] source pointer to the data of the colorbuffer to be presented (backbuffer dimensions).
        // @param[in] floats format of the data (number of float32s).
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_InvalidFormat if an invalid format was encountered.
        // @return Yw3d_E_InvalidState if an invalid state was encountered.
        // @return Yw3d_E_Unknown if a present-target related problem was encountered.
        virtual Yw3dResult Present(const float* source, uint32_t floats) = 0;

        // Returns a pointer to the associated device. Calling this function will increase the internal reference count of the device. 
        // Failure to call Release() when finished using the pointer will result in a memory leak.
        class Yw3dDevice* AcquireDevice();

    protected:
        // Pointer to device.
        class Yw3dDevice* m_Device;
    };
}

#endif // __YW_3D_PRESENT_TARGET_H__

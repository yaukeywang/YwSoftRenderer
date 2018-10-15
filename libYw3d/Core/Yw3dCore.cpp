// Add by Yaukey at 2018-02-16.
// YW Soft Renderer 3d header file.

#include "Yw3dCore.h"
#include "Yw3dDevice.h"

namespace yw
{
    Yw3d::Yw3d()
    {

    }

    Yw3d::~Yw3d()
    {
        
    }

    Yw3dResult Yw3d::CreateDevice(class Yw3dDevice** device, const Yw3dDeviceParameters* deviceParameters)
    {
        if (nullptr == device)
        {
            LOGE(_T("Yw3d::CreateDevice: parameter device points to null.\n"));
            return Yw3d_E_InvalidParameters;
        }

        if (nullptr == deviceParameters)
        {
            *device = nullptr;
            LOGE(_T("Yw3d::CreateDevice: parameter deviceParameters points to null.\n"));
            return Yw3d_E_InvalidParameters;
        }

        // Create device.
        *device = new Yw3dDevice(this, deviceParameters);
        if (nullptr == *device)
        {
            LOGE(_T("Yw3d::CreateDevice: out of memory, cannot create device.\n"));
            return Yw3d_E_OutOfMemory;
        }

        return (*device)->Create();
    }

    Yw3dResult CreateYw3d(class Yw3d** yw3d)
    {
        if (nullptr == yw3d)
        {
            LOGE(_T("CreateYw3d: parameter yw3d points to null.\n"));
            return Yw3d_E_InvalidParameters;
        }

        // Create Yw3d object.
        *yw3d = new Yw3d();
        if (nullptr == *yw3d)
        {
            LOGE(_T("CreateYw3d: out of memory, cannot create yw3d-instance.\n"));
            return Yw3d_E_OutOfMemory;
        }

        return Yw3d_S_OK;
    }
}

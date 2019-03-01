// Add by Yaukey at 2018-03-31.
// YW Soft Renderer present target class.

#ifndef __YW_3D_PRESENT_TARGET_H__
#define __YW_3D_PRESENT_TARGET_H__

#include "../Yw3dBase.h"
#include "../Yw3dTypes.h"

namespace yw
{
    // ------------------------------------------------------------------
    // Present target base.

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

// ------------------------------------------------------------------
// Platform-dependent code.

// ------------------------------------------------------------------
// Windows platform.
#if defined(_WIN32) || defined(WIN32)

// The needed header and lib.
#include <ddraw.h>
#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dxguid.lib")

namespace yw
{
    // This class defines a Yw3d present target for the Windows-platform.
    class Yw3dPresentTargetWindows : IYw3dPresentTarget
    {
        friend class Yw3dDevice;

    protected:
        // Accessible by Yw3dDevice which is the only class that may create a present target.
        // @param[in] device a pointer to the parent Yw3dDevice-object.
        Yw3dPresentTargetWindows(class Yw3dDevice* device);

        // Accessible by IBase. The destructor is called when the reference count reaches zero.
        ~Yw3dPresentTargetWindows();

    public:
        // Creates and initializes the present target.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_Unknown if a presenttarget-specific problem was encountered.
        Yw3dResult Create();

        // Presents the contents of a given rendertarget's colorbuffer.
        // @param[in] source pointer to the data of the colorbuffer to be presented (backbuffer dimensions).
        // @param[in] floats format of the data (number of float32s).
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_InvalidFormat if an invalid format was encountered.
        // @return Yw3d_E_InvalidState if an invalid state was encountered.
        // @return Yw3d_E_Unknown if a present-target related problem was encountered.
        Yw3dResult Present(const float* source, uint32_t floats);

    private:
        // Returns low-bit and number of bits for a given color-channel mask.
        // @param[in] mask color-channel mask.
        // @param[out] lowBit lowbit of the channel.
        // @param[out] numberBits number of bits of the channel.
        void ProcessBits(uint32_t mask, uint16_t& lowBit, uint16_t& numberBits);

    private:
        // Pointer to DirectDraw.
        LPDIRECTDRAW7 m_DirectDraw;

        // Pointer to a DirectDraw clipper.
        LPDIRECTDRAWCLIPPER m_DirectDrawClipper;

        // Pointer to DirectDraw surfaces.
        LPDIRECTDRAWSURFACE7 m_DirectDrawSurfaces[2];

        // True if DirectDraw surfaces have been lost and need to be restored.
        bool m_DDSurfaceLost;

        // Used when presenting to a 16-bit backbuffer. Masks and maximum color-values per channel, e.g. (31,63,31) for 16-bit 565 mode.
        uint16_t m_16bitMaxVal[3];

        // Used when presenting to a 16-bit backbuffer. Shifts for the individual color channels, e.g. (
        uint16_t m_16bitShift[3];
    };
}

#endif // End of Windows platform.

// ------------------------------------------------------------------
// Linux platform.
#if defined(LINUX_X11) || defined(_LINUX)

namespace yw
{
    // This class defines a Yw3d present target for the Linux-platform.
    class Yw3dPresentTargetLinux : IYw3dPresentTarget
    {
        friend class Yw3dDevice;

    protected:
        // Accessible by Yw3dDevice which is the only class that may create a present target.
        // @param[in] device a pointer to the parent Yw3dDevice-object.
        Yw3dPresentTargetLinux(class Yw3dDevice* device);

        // Accessible by IBase. The destructor is called when the reference count reaches zero.
        ~Yw3dPresentTargetLinux();

    public:
        // Creates and initializes the present target.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_Unknown if a presenttarget-specific problem was encountered.
        Yw3dResult Create();

        // Presents the contents of a given rendertarget's colorbuffer.
        // @param[in] source pointer to the data of the colorbuffer to be presented (backbuffer dimensions).
        // @param[in] floats format of the data (number of float32s).
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_InvalidFormat if an invalid format was encountered.
        // @return Yw3d_E_InvalidState if an invalid state was encountered.
        // @return Yw3d_E_Unknown if a present-target related problem was encountered.
        Yw3dResult Present(const float* source, uint32_t floats);
    };
}

#endif

// ------------------------------------------------------------------
// Mac OSX platform.
#if defined(_MAC_OSX)

namespace yw
{
    // This class defines a Yw3d present target for the Linux-platform.
    class Yw3dPresentTargetMacOSX : IYw3dPresentTarget
    {
        friend class Yw3dDevice;

    protected:
        // Accessible by Yw3dDevice which is the only class that may create a present target.
        // @param[in] device a pointer to the parent Yw3dDevice-object.
        Yw3dPresentTargetMacOSX(class Yw3dDevice* device);

        // Accessible by IBase. The destructor is called when the reference count reaches zero.
        ~Yw3dPresentTargetMacOSX();

    public:
        // Creates and initializes the present target.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_Unknown if a presenttarget-specific problem was encountered.
        Yw3dResult Create();

        // Presents the contents of a given rendertarget's colorbuffer.
        // @param[in] source pointer to the data of the colorbuffer to be presented (backbuffer dimensions).
        // @param[in] floats format of the data (number of float32s).
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_InvalidFormat if an invalid format was encountered.
        // @return Yw3d_E_InvalidState if an invalid state was encountered.
        // @return Yw3d_E_Unknown if a present-target related problem was encountered.
        Yw3dResult Present(const float* source, uint32_t floats);
    };
}

#endif

// ------------------------------------------------------------------
// Amiga OS 4 platform.
#if defined(__amigaos4__) || (_AMIGAOS4)

namespace yw
{
    // This class defines a Yw3d present target for the Linux-platform.
    class Yw3dPresentTargetAmigaOS4 : IYw3dPresentTarget
    {
        friend class Yw3dDevice;

    protected:
        // Accessible by Yw3dDevice which is the only class that may create a present target.
        // @param[in] device a pointer to the parent Yw3dDevice-object.
        Yw3dPresentTargetAmigaOS4(class Yw3dDevice* device);

        // Accessible by IBase. The destructor is called when the reference count reaches zero.
        ~Yw3dPresentTargetAmigaOS4();

    public:
        // Creates and initializes the present target.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_Unknown if a presenttarget-specific problem was encountered.
        Yw3dResult Create();

        // Presents the contents of a given rendertarget's colorbuffer.
        // @param[in] source pointer to the data of the colorbuffer to be presented (backbuffer dimensions).
        // @param[in] floats format of the data (number of float32s).
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_InvalidFormat if an invalid format was encountered.
        // @return Yw3d_E_InvalidState if an invalid state was encountered.
        // @return Yw3d_E_Unknown if a present-target related problem was encountered.
        Yw3dResult Present(const float* source, uint32_t floats);
    };
}

#endif

#endif // __YW_3D_PRESENT_TARGET_H__

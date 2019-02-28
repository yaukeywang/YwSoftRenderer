// Add by Yaukey at 2018-03-31.
// YW Soft Renderer present target class.

#include "Yw3dPresentTarget.h"
#include "Yw3dDevice.h"

namespace yw
{
    // ------------------------------------------------------------------
    // Present target base.

    IYw3dPresentTarget::IYw3dPresentTarget(Yw3dDevice* device) :
        m_Device(device)
    {
        // Note: cannot add a reference to parent device or the presenttarget will never be freed.
        // m_Device->AddRef();
    }

    IYw3dPresentTarget::~IYw3dPresentTarget()
    {
        // Note: see note in constructor.
        // SAFE_RELEASE(m_Device);
    }

    Yw3dDevice* IYw3dPresentTarget::AcquireDevice()
    {
        if (nullptr != m_Device)
        {
            m_Device->AddRef();
        }

        return m_Device;
    }
}

// ------------------------------------------------------------------
// Platform-dependent code.

// ------------------------------------------------------------------
// Windows platform.
#if defined(_WIN32) || defined(WIN32)

namespace yw
{
    Yw3dPresentTargetWindows::Yw3dPresentTargetWindows(Yw3dDevice* device) : 
        IYw3dPresentTarget(device),
        m_DirectDraw(nullptr),
        m_DirectDrawClipper(nullptr),
        m_DDSurfaceLost(false)
    {
        m_DirectDrawSurfaces[0] = nullptr;
        m_DirectDrawSurfaces[1] = nullptr;

        m_16bitMaxVal[0] = 0;
        m_16bitMaxVal[1] = 0;
        m_16bitMaxVal[2] = 0;

        m_16bitShift[0] = 0;
        m_16bitShift[1] = 0;
        m_16bitShift[2] = 0;
    }

    Yw3dPresentTargetWindows::~Yw3dPresentTargetWindows()
    {
        // Get device parameters.
        Yw3dDeviceParameters deviceParameters = m_Device->GetDeviceParameters();

        // Release DirectDraw.
        if (nullptr != m_DirectDrawSurfaces[1])
        {
            if (deviceParameters.windowed)
            {
                m_DirectDrawSurfaces[1]->Release();
            }

            m_DirectDrawSurfaces[1] = nullptr;
        }

        YW_SAFE_RELEASE(m_DirectDrawSurfaces[0]);
        YW_SAFE_RELEASE(m_DirectDrawClipper);

        if (nullptr != m_DirectDraw)
        {
            if (deviceParameters.windowed)
            {
                m_DirectDraw->RestoreDisplayMode();
                m_DirectDraw->SetCooperativeLevel(deviceParameters.deviceWindow, DDSCL_NORMAL);
            }

            YW_SAFE_RELEASE(m_DirectDraw);
        }
    }

    Yw3dResult Yw3dPresentTargetWindows::Create()
    {
        // Get device parameters.
        Yw3dDeviceParameters deviceParameters = m_Device->GetDeviceParameters();
        if ((0 == deviceParameters.backBufferWidth) || (0 == deviceParameters.backBufferHeight))
        {
            LOGE(_T("Yw3dPresentTargetWindows::Create: invalid backbuffer dimensions have been supplied.\n"));
            return Yw3d_E_InvalidParameters;
        }

        // Crate direct draw 7 instance object.
        if (FAILED(DirectDrawCreateEx(nullptr, (LPVOID*)&m_DirectDraw, IID_IDirectDraw7, nullptr)))
        {
            LOGE(_T("Yw3dPresentTargetWindows::Create: couldn't create DirectDraw 7 instance.\n"));
            return Yw3d_E_Unknown;
        }

        // Set cooperative level by proper flags.
        uint32_t ddFlags = deviceParameters.windowed ? DDSCL_NORMAL : (DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT);
        if (FAILED(m_DirectDraw->SetCooperativeLevel(deviceParameters.deviceWindow, ddFlags)))
        {
            LOGE(_T("Yw3dPresentTargetWindows::Create: couldn't set cooperative level.\n"));
            return Yw3d_E_Unknown;
        }

        // Check color format.
        if (!deviceParameters.windowed)
        {
            switch (deviceParameters.fullScreenColorBits)
            {
            case 16:
            case 24:
            case 32:
                break;
            default:
                LOGE(_T("Yw3dPresentTargetWindows::Create: invalid backbuffer bit-depth sepcified.\n"));
                return Yw3d_E_Unknown;
            }

            // Go to fullscreen mode.
            if (FAILED(m_DirectDraw->SetDisplayMode(deviceParameters.backBufferWidth, deviceParameters.backBufferHeight, deviceParameters.fullScreenColorBits, 0, 0)))
            {
                LOGE(_T("Yw3dPresentTargetWindows::Create: couldn't set display mode.\n"));
                return Yw3d_E_Unknown;
            }
        }
        else
        {
            // Determine display mode.
            DDSURFACEDESC2 descSurface;
            descSurface.dwSize = sizeof(DDSURFACEDESC2);
            if (FAILED(m_DirectDraw->GetDisplayMode(&descSurface)))
            {
                LOGE(_T("Yw3dPresentTargetWindows::Create: couldn't get display mode.\n"));
                return Yw3d_E_Unknown;
            }

            switch (descSurface.ddpfPixelFormat.dwRGBBitCount)
            {
            case 16:
                {
                    // Also handles formats 555(15-bit) and 444(12-bit).
                    uint16_t numberRedBits = 0;
                    uint16_t numberGreenBits = 0;
                    uint16_t numberBlueBits = 0;
                    ProcessBits(descSurface.ddpfPixelFormat.dwRBitMask, m_16bitShift[0], numberRedBits);
                    ProcessBits(descSurface.ddpfPixelFormat.dwGBitMask, m_16bitShift[1], numberGreenBits);
                    ProcessBits(descSurface.ddpfPixelFormat.dwBBitMask, m_16bitShift[2], numberBlueBits);

                    m_16bitMaxVal[0] = (uint16_t)((1 << numberRedBits) - 1);
                    m_16bitMaxVal[1] = (uint16_t)((1 << numberGreenBits) - 1);
                    m_16bitMaxVal[2] = (uint16_t)((1 << numberBlueBits) - 1);
                }
                break;
            case 24:
            case 32:
                break;
            default:
                LOGE(_T("Yw3dPresentTargetWindows::Create: display modes with colors other than 16-, 24- or 32-bit are not supported!\n"));
                return Yw3d_E_Unknown;;
            }
        }

        // Create surface.
        if (deviceParameters.windowed)
        {
            // Create a primary surface without a backbuffer - a secondary surface will be used as backbuffer.
            DDSURFACEDESC2 descSurface;
            ZeroMemory(&descSurface, sizeof(descSurface));
            descSurface.dwSize = sizeof(DDSURFACEDESC2);
            descSurface.dwFlags = DDSD_CAPS;
            descSurface.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
            if (FAILED(m_DirectDraw->CreateSurface(&descSurface, &m_DirectDrawSurfaces[0], nullptr)))
            {
                LOGE(_T("Yw3dPresentTargetWindows::Create: couldn't create primary surface.\n"));
                return Yw3d_E_Unknown;
            }

            // Create the secondary surface.
            descSurface.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
            descSurface.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
            descSurface.dwWidth = deviceParameters.backBufferWidth;
            descSurface.dwHeight = deviceParameters.backBufferHeight;
            if (FAILED(m_DirectDraw->CreateSurface(&descSurface, &m_DirectDrawSurfaces[1], nullptr)))
            {
                LOGE(_T("Yw3dPresentTargetWindows::Create: couldn't create secondary surface.\n"));
                return Yw3d_E_Unknown;
            }

            // Create clipper.
            if (FAILED(m_DirectDraw->CreateClipper(0, &m_DirectDrawClipper, nullptr)))
            {
                LOGE(_T("Yw3dPresentTargetWindows::Create: couldn't create clipper.\n"));
                return Yw3d_E_Unknown;
            }

            // Set clipper to window.
            if (FAILED(m_DirectDrawClipper->SetHWnd(0, deviceParameters.deviceWindow)))
            {
                LOGE(_T("Yw3dPresentTargetWindows::Create: couldn't set clipper window handle.\n"));
                return Yw3d_E_Unknown;
            }

            // Attch clipper object to primary surface.
            if (FAILED(m_DirectDrawSurfaces[0]->SetClipper(m_DirectDrawClipper)))
            {
                LOGE(_T("Yw3dPresentTargetWindows::Create: couldn't attach clipper to primary surface.\n"));
                return Yw3d_E_Unknown;
            }
        }
        else
        {
            // Create a primary surface with one backbuffer if in fullscreen mode.
            DDSURFACEDESC2 descSurface;
            ZeroMemory(&descSurface, sizeof(descSurface));
            descSurface.dwSize = sizeof(DDSURFACEDESC2);
            descSurface.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
            descSurface.dwBackBufferCount = 1;
            descSurface.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP;
            if (FAILED(m_DirectDraw->CreateSurface(&descSurface, &m_DirectDrawSurfaces[0], nullptr)))
            {
                LOGE(_T("Yw3dPresentTargetWindows::Create: couldn't create primary surface.\n"));
                return Yw3d_E_Unknown;
            }

            // Get DD backbuffer.
            DDSCAPS2 capsDirectDraw = { DDSCAPS_BACKBUFFER };
            if (FAILED(m_DirectDrawSurfaces[0]->GetAttachedSurface(&capsDirectDraw, &m_DirectDrawSurfaces[1])))
            {
                LOGE(_T("Yw3dPresentTargetWindows::Create: couldn't access secondary surface.\n"));
                return Yw3d_E_Unknown;
            }
        }

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dPresentTargetWindows::Present(const float* source, uint32_t floats)
    {
        // Get device parameters.
        Yw3dDeviceParameters deviceParameters = m_Device->GetDeviceParameters();

        // Check for lost DirectDraw surface.
        if (m_DDSurfaceLost)
        {
            m_DirectDrawSurfaces[0]->Restore();
            if (deviceParameters.windowed)
            {
                m_DirectDrawSurfaces[1]->Restore();
            }

            m_DDSurfaceLost = false;
        }

        // Lock BackBuffer-surface.
        DDSURFACEDESC2 descSurface;
        descSurface.dwSize = sizeof(DDSURFACEDESC2);
        if (FAILED(m_DirectDrawSurfaces[1]->Lock(nullptr, &descSurface, DDLOCK_WAIT | DDLOCK_NOSYSLOCK | DDLOCK_WRITEONLY, 0)))
        {
            LOGE(_T("Yw3dPresentTargetWindows::Present: couldn't lock secondary surface.\n"));
            return Yw3d_E_Unknown;
        }

        // Get surface byte count.
        const uint32_t destBytes = descSurface.ddpfPixelFormat.dwRGBBitCount / 8;

        // Copy pixels to the BackBuffer-surface.
        const uint32_t destRowJump = descSurface.lPitch - deviceParameters.backBufferWidth * destBytes;
        uint8_t* destination = (uint8_t*)descSurface.lpSurface;

        fpuTruncate();

        if (2 == destBytes)
        {
            // 16-bit.
            uint32_t height = deviceParameters.backBufferHeight;
            while (height--)
            {
                uint32_t width = deviceParameters.backBufferWidth;
                while (width--)
                {
                    const int32_t r = Clamp(ftol(source[0] * (float)m_16bitMaxVal[0]), 0, m_16bitMaxVal[0]);
                    const int32_t g = Clamp(ftol(source[1] * (float)m_16bitMaxVal[1]), 0, m_16bitMaxVal[1]);
                    const int32_t b = Clamp(ftol(source[2] * (float)m_16bitMaxVal[2]), 0, m_16bitMaxVal[2]);
                    source += floats;

                    *((uint16_t*)destination) = (r << m_16bitShift[0]) | (g << m_16bitShift[1]) | (b << m_16bitShift[2]);
                    destination += destBytes;
                }

                destination += destRowJump;
            }
        }
        else
        {
            // 24-bit or 32-bit.
            uint32_t height = deviceParameters.backBufferHeight;
            while (height--)
            {
                uint32_t width = deviceParameters.backBufferWidth;
                while (width--)
                {
                    // descSurface.ddpfPixelFormat.dwRBitMask: 0x00ff0000.
                    // descSurface.ddpfPixelFormat.dwGBitMask: 0x0000ff00.
                    // descSurface.ddpfPixelFormat.dwBBitMask: 0x000000ff.
                    // descSurface.ddpfPixelFormat.dwRGBAlphaBitMask: 0x00000000.
                    destination[0] = Clamp(ftol(source[2] * 255.0f), 0, 255); // b
                    destination[1] = Clamp(ftol(source[1] * 255.0f), 0, 255); // g
                    destination[2] = Clamp(ftol(source[0] * 255.0f), 0, 255); // r

                    source += floats;
                    destination += destBytes;
                }

                destination += destRowJump;
            }
        }

        fpuReset();

        // Unlock BackBuffer-surface and surface.
        m_DirectDrawSurfaces[1]->Unlock(nullptr);

        // Present the image to the screen.
        if (deviceParameters.windowed)
        {
            POINT pntTopLeft = { 0, 0 };
            ClientToScreen(deviceParameters.deviceWindow, &pntTopLeft);

            RECT rctDestination;
            GetClientRect(deviceParameters.deviceWindow, &rctDestination);
            OffsetRect(&rctDestination, pntTopLeft.x, pntTopLeft.y);

            RECT rctSource;
            SetRect(&rctSource, 0, 0, deviceParameters.backBufferWidth, deviceParameters.backBufferHeight);

            // Blit secondary to primary surface.
            if (FAILED(m_DirectDrawSurfaces[0]->Blt(&rctDestination, m_DirectDrawSurfaces[1], &rctSource, DDBLT_WAIT, nullptr)))
            {
                m_DDSurfaceLost = true;
                LOGE(_T("Yw3dPresentTargetWindows::Present: primary surface have been lost!\n"));
                return Yw3d_E_Unknown;
            }
        }
        else
        {
            if (FAILED(m_DirectDrawSurfaces[0]->Flip(nullptr, DDFLIP_WAIT)))
            {
                m_DDSurfaceLost = true;
                LOGE(_T("Yw3dPresentTargetWindows::Present: surface have been lost!\n"));
                return Yw3d_E_Unknown;
            }
        }

        return Yw3d_S_OK;
    }

    // Small utility function to find the LowBit and Number of Bits in a supplied Mask.
    // Thank you, Nathan Davies!
    // - http://archive.gamedev.net/archive/reference/articles/article588.html
    // - https://www.gamedev.net/articles/programming/graphics/plotting-pixels-in-non-palettized-directdraw-su-r588
    void Yw3dPresentTargetWindows::ProcessBits(uint32_t mask, uint16_t& lowBit, uint16_t& numberBits)
    {
        // Find low bit start position.
        uint32_t testMask = 1;
        for (lowBit = 0; lowBit < 32; lowBit++)
        {
            if (0 != (mask & testMask))
            {
                break;
            }

            testMask <<= 1;
        }

        // Find low bit count.
        testMask <<= 1;
        for (numberBits = 1; numberBits < 32; numberBits++)
        {
            if (0 == (mask & testMask))
            {
                break;
            }

            testMask <<= 1;
        }
    }
}

#endif // End of Windows platform.

// ------------------------------------------------------------------
// Linux platform.
#if defined(LINUX_X11) || defined(_LINUX)
#endif

// ------------------------------------------------------------------
// Mac OSX platform.
#if defined(_MAC_OSX)
#endif

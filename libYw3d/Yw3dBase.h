// Add by Yaukey at 2018-01-29.
// YW Soft Renderer 3d base header file.

#ifndef __YW_3D_BASE_H__
#define __YW_3D_BASE_H__

#include "YwBase.h"

namespace yw
{
    // ------------------------------------------------------------------
    // Return result.
    enum Yw3dResult
    {
        // Success.
        Yw3d_S_OK = 0,                  // No errors were encountered, function returned successfully.
        
        // Failed.
        Yw3d_E_Unknown,			        // An unknown error has occured.
        Yw3d_E_InvalidParameters,       // One or more parameters were invalid.
        Yw3d_E_OutOfMemory,             // Allocation of memory within the function failed.
        Yw3d_E_InvalidFormat,           // A format is invalid for a specific task.
        Yw3d_E_InvalidState,            // An invalid state has been found and the function cannot continue.

        Yw3d_Result_End
    };

    // Macro to test for success.
    // @param[in] res a function return value.
    // @return true if the function succeeded.
    #define YW3D_SUCCESSFUL(res)    (Yw3d_S_OK == (res))

    // Macro to test for failure.
    // @param[in] res a function return value.
    // @return true if the function failed.
    #define YW3D_FAILED(res)    (Yw3d_S_OK != (res))

    // ------------------------------------------------------------------
    // Yw3d base-class definition.
    // IBase is the base class for all Yw3d classes. It implements a reference-counter with functions AddRef() and Release() known from COM interfaces.
    class IBase
    {
    protected:
        // The constructor initializes the reference count to 1.
        inline IBase() : m_RefCount(1) {}	
        virtual inline ~IBase() {}

    private:
        // Private copy-operator to avoid object copying.
        IBase(const IBase &) {}

    public:
        // AddRef() increases the reference count.
        inline void AddRef() 
        { 
            ++m_RefCount; 
        }	

        // Release() decreases the reference count and calls the destructor when it is 0.
        inline void Release()
        { 
            if (0 == --m_RefCount)
            {
                delete this;
            }
        }

    private:
        // Private assignment-operator to avoid object copying. Returns a value to avoid compiler warnings.
        IBase &operator =(const IBase &)
        { 
            return *this; 
        }	

    private:
        uint32_t m_RefCount;
    };

    // ------------------------------------------------------------------
    // Floating pointer helper-functions.

    // Windows.
    #ifdef WIN32

    // Float type header.
    #include <float.h>

    // Disable warning about _controlfp being deprecated.
    #pragma warning(disable:4996)

    // Sets FPU to truncation-mode and single precision.
    inline void fpuTruncate()
    {
        _controlfp(_RC_DOWN + _PC_24, _MCW_RC | _MCW_PC);
    }

    // Resets FPU to default.
    inline void fpuReset()
    {
        _controlfp(_CW_DEFAULT, _MCW_RC | _MCW_PC);
    }

    #endif // !WIN32

    // Linux.
    #ifdef LINUX_X11

    // Float type header.
    #include <fpu_control.h>

    // Sets FPU to truncation-mode and single precision.
    inline void fpuTruncate()
    {
        fpu_control_t cwChop = _FPU_RC_DOWN | _FPU_IEEE | _FPU_SINGLE;
        _FPU_SETCW(cwChop);
    }

    // Resets FPU to default.
    inline void fpuReset()
    {
        fpu_control_t cwDefault = _FPU_DEFAULT;
        _FPU_SETCW(cwDefault);
    }

    #endif // !LINUX_X11

    // AmigaOS4.
    #ifdef __amigaos4__

    inline void fpuTruncate()
    {
        // #warning: fpuTruncate() currently not implemented on AmigaOS 4 
    }

    inline void fpuReset()
    {
        // #warning: fpuReset() currently not implemented on AmigaOS 4
    }

    #endif // !__amigaos4__

    // ------------------------------------------------------------------
    // Others.

    // ftol() performs fast float to integer-conversion.
    // Ensure that fpuTruncate() has been called before using this function, otherwise
    // ftol() will round to the nearest integer. After calling fpuTruncate() it is advised to 
    // reset the fpu to rounding-mode using fpuReset().
    // @param[in] f The floating pointer number to be converted to an integer.
    // @return an integer.

    #ifdef __amigaos4__

    // Float calc union.
    typedef union
    {
        struct
        {
            unsigned long hi;
            unsigned long lo;
        } i;

        double d;
    } hexdouble;

    #endif // !__amigaos4__

    // Float to int.
    inline int32_t ftol(float f)
    {
    #ifdef __amigaos4__
        static hexdouble hd;
        __asm__("fctiw %0, %1" : "=f" (hd.d) : "f" (f));
        return hd.i.lo;
    #else
        static int32_t tmp;

        #if _MSC_VER > 1000
            __asm
            {
                fld f
                fistp tmp
            }
        #else
            asm volatile("fistpl %0" : "=m" (tmp) : "t" (f) : "st");
        #endif

        return tmp;
    #endif
    }
}

#endif // !__YW_3D_BASE_H__

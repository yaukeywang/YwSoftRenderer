// Add by Yaukey at 2018-01-29.
// YW Soft Renderer 3d base header file.

#ifndef __YW_3D_BASE_H__
#define __YW_3D_BASE_H__

#include "System/Support/YwBase.h"

namespace yw
{
    // Return result.
    enum Yw3dResult
    {
        // No errors were encountered, function returned successfully.
        Yw3d_S_OK = 0,

        // An unknown error has occured.
        Yw3d_E_Unknown,			

        // One or more parameters were invalid.
        Yw3d_E_InvalidParameters,

        // Allocation of memory within the function failed.
        Yw3d_E_OutOfMemory,

        // A format is invalid for a specific task.
        Yw3d_E_InvalidFormat,

        // An invalid state has been found and the function cannot continue.
        Yw3d_E_InvalidState,

        Yw3d_Result_End
    };

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
}

#endif // !__YW_3D_BASE_H__

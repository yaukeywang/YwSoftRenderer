// Add by Yaukey at 2018-01-31.
// YW Soft Renderer index buffer class.

#include "stdafx.h"
#include "Yw3dIndexBuffer.h"
#include "Yw3dDevice.h"
#include "System/Support/YwUtility.h"

namespace yw
{
    Yw3dIndexBuffer::Yw3dIndexBuffer(Yw3dDevice* device) :
        m_Device(device), 
        m_Length(0), 
        m_Format(Yw3d_FMT_INDEX32),
        m_Data(nullptr)
    {
        m_Device->AddRef();
    }

    Yw3dIndexBuffer::~Yw3dIndexBuffer()
    {
        YW_SAFE_DELETE_ARRAY(m_Data);
        YW_SAFE_RELEASE(m_Device);
    }

    Yw3dResult Yw3dIndexBuffer::Create(uint32_t length, Yw3dFormat format)
    {
        if (0 == length)
        {
            LOGE(_T("Yw3dIndexBuffer::Create: parameter i_iLength is 0.\n"));
            return Yw3d_E_InvalidParameters;
        }

        if ((Yw3d_FMT_INDEX16 != format) && (Yw3d_FMT_INDEX32 != format))
        {
            LOGE(_T("Yw3dIndexBuffer::Create: invalid format specified.\n"));
            return Yw3d_E_InvalidFormat;
        }

        // Get length and format.
        m_Length = length;
        m_Format = format;

        // Create buffer by length.
        m_Data = new uint8_t[length];
        if (nullptr != m_Data)
        {
            LOGE(_T("Yw3dIndexBuffer::Create: out of memory, cannot create vertex buffer.\n"));
            return Yw3d_E_OutOfMemory;
        }

        return Yw3d_S_OK;
    }

    Yw3dDevice* Yw3dIndexBuffer::AquireDevice()
    {
        if (nullptr != m_Device)
        {
            m_Device->AddRef();
        }

        return m_Device;
    }

    Yw3dResult Yw3dIndexBuffer::GetPointer(const uint32_t offset, void** pointer) const
    {
        if (nullptr == pointer)
        {
            LOGE(_T("Yw3dIndexBuffer::GetPointer: parameter pointer points to null.\n"));
            return Yw3d_E_InvalidParameters;
        }

        if (offset >= m_Length)
        {
            LOGE(_T("Yw3dIndexBuffer::GetPointer: offset is larger than index buffer length.\n"));
            return Yw3d_E_InvalidParameters;
        }

        *pointer = m_Data + offset;
        return Yw3d_S_OK;
    }

    uint32_t Yw3dIndexBuffer::GetLength() const
    {
        return m_Length;
    }

    Yw3dFormat Yw3dIndexBuffer::GetFormat() const
    {
        return m_Format;
    }

    Yw3dResult Yw3dIndexBuffer::GetVertexIndex(uint32_t arrayIndex, uint32_t& value) const
    {
        switch (m_Format)
        {
        case Yw3d_FMT_INDEX16:
            {
                if (arrayIndex >= (m_Length / sizeof(uint16_t)))
                {
                    LOGE(_T("Yw3dIndexBuffer::GetVertexIndex: parameter arrayIndex exceeds vertex buffer size.\n"));
                    return Yw3d_E_InvalidParameters;
                }

                const uint16_t* indexData = (const uint16_t*)m_Data;
                value = indexData[arrayIndex];

                return Yw3d_S_OK;
            }
        case Yw3d_FMT_INDEX32:
            {
                if (arrayIndex >= (m_Length / sizeof(uint32_t)))
                {
                    LOGE(_T("Yw3dIndexBuffer::GetVertexIndex: parameter arrayIndex exceeds vertex buffer size.\n"));
                    return Yw3d_E_InvalidParameters;
                }

                const uint32_t* indexData = (const uint32_t*)m_Data;
                value = indexData[arrayIndex];

                return Yw3d_S_OK;
            }
        default:
            LOGE(_T("Yw3dIndexBuffer::GetVertexIndex: invalid format.\n"));
            return Yw3d_E_InvalidFormat;
        }
    }
}

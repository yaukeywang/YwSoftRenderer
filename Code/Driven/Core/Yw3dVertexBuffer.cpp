// Add by Yaukey at 2018-01-31.
// YW Soft Renderer vertex buffer class.

#include "stdafx.h"
#include "Yw3dVertexBuffer.h"
#include "Yw3dDevice.h"
#include "System/Support/YwUtility.h"

namespace yw
{
    Yw3dVertexBuffer::Yw3dVertexBuffer(Yw3dDevice* device) :
        m_Device(device), 
        m_Length(0), 
        m_Data(nullptr)
    {
        m_Device->AddRef();
    }

    Yw3dVertexBuffer::~Yw3dVertexBuffer()
    {
        YW_SAFE_DELETE_ARRAY(m_Data);
        YW_SAFE_RELEASE(m_Device);
    }

    Yw3dResult Yw3dVertexBuffer::Create(const uint32_t length)
    {
        if (0 == length)
        {
            LOGE(_T("Yw3dVertexBuffer::Create: parameter length is 0.\n"));
            return Yw3d_E_InvalidParameters;
        }

        // Get length.
        m_Length = length;

        // Create buffer by length.
        m_Data = new uint8_t[m_Length];
        if (nullptr == m_Data)
        {
            LOGE(_T("Yw3dVertexBuffer::Create: out of memory, cannot create vertex buffer.\n"));
            return Yw3d_E_OutOfMemory;
        }

        return Yw3d_S_OK;
    }

    Yw3dDevice* Yw3dVertexBuffer::AquireDevice()
    {
        if (nullptr != m_Device)
        {
            m_Device->AddRef();
        }

        return m_Device;
    }

    Yw3dResult Yw3dVertexBuffer::GetPointer(const uint32_t offset, void** pointer) const
    {
        if (nullptr == pointer)
        {
            LOGE(_T("Yw3dVertexBuffer::GetPointer: parameter pointer points to null.\n"));
            return Yw3d_E_InvalidParameters;
        }

        if (offset >= m_Length)
        {
            *pointer = nullptr;
            LOGE(_T("Yw3dVertexBuffer::GetPointer: offset exceeds vertex buffer length.\n"));

            return Yw3d_E_InvalidParameters;
        }

        *pointer = m_Data + offset;
        return Yw3d_S_OK;
    }

    uint32_t Yw3dVertexBuffer::GetLength() const
    {
        return m_Length;
    }
}

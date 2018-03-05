// Add by Yaukey at 2018-01-31.
// YW Soft Renderer vertex format class.

#include "stdafx.h"
#include "Yw3dVertexFormat.h"
#include "Yw3dDevice.h"
#include "System/Support/YwUtility.h"

namespace yw
{
    Yw3dVertexFormat::Yw3dVertexFormat(Yw3dDevice* device) :
        m_Device(device),
        m_NumVertexElements(0),
        m_HighestStream(0),
        m_Elements(nullptr)
    {
        m_Device->AddRef();
    }

    Yw3dVertexFormat::~Yw3dVertexFormat()
    {
        YW_SAFE_DELETE_ARRAY(m_Elements);
        YW_SAFE_RELEASE(m_Device);
    }

    Yw3dResult Yw3dVertexFormat::Create(const Yw3dVertexElement* vertexDeclaration, const uint32_t vertexDeclSize)
    {
        if (nullptr == vertexDeclaration)
        {
            LOGE(_T("Yw3dVertexElement::Create: parameter vertexDeclaration points to null.\n"));
            return Yw3d_E_InvalidParameters;
        }

        if (0 == vertexDeclSize)
        {
            LOGE(_T("Yw3dVertexElement::Create: parameter vertexDeclSize is 0.\n"));
            return Yw3d_E_InvalidParameters;
        }

        // Calculate vertex element size.
        m_NumVertexElements = vertexDeclSize / sizeof(Yw3dVertexElement);
        m_HighestStream = 0;

        // Calculate highest stream.
        const Yw3dVertexElement* curElement = vertexDeclaration;
        for (uint32_t element = 0; element < m_NumVertexElements; element++, curElement++)
        {
            if (curElement->stream >= YW3D_MAX_VERTEX_STREAMS)
            {
                LOGE(_T("Yw3dVertexFormat::Create: vertex element's stream number exceeds number of available streams.\n"));
                return Yw3d_E_InvalidParameters;
            }

            if (curElement->shaderRegister >= YW3D_VERTEX_SHADER_REGISTERS)
            {
                LOGE(_T("Yw3dVertexFormat::Create: vertex element's target register exceeds number of available vertex shader registers.\n"));
                return Yw3d_E_InvalidParameters;
            }

            if ((curElement->type < Yw3d_VET_Float) || (curElement->type > Yw3d_VET_Vector4))
            {
                LOGE(_T("Yw3dVertexFormat::Create: vertex element has invalid type.\n"));
                return Yw3d_E_InvalidParameters;
            }

            // Get highest stream.
            if (curElement->stream > m_HighestStream)
            {
                m_HighestStream = curElement->stream;
            }
        }

        // Create vertex element data buffer.
        m_Elements = new Yw3dVertexElement[m_NumVertexElements];
        if (nullptr == m_Elements)
        {
            LOGE(_T("Yw3dVertexFormat::Create: out of memory, cannot create vertex element data.\n"));
            return Yw3d_E_OutOfMemory;
        }

        // Copy vertex element data.
        memcpy(m_Elements, vertexDeclaration, sizeof(Yw3dVertexElement) * m_NumVertexElements);

        return Yw3d_S_OK;
    }

    Yw3dDevice* Yw3dVertexFormat::AquireDevice()
    {
        if (nullptr != m_Device)
        {
            m_Device->AddRef();
        }

        return m_Device;
    }

    uint32_t Yw3dVertexFormat::GetNumVertexElements() const
    {
        return m_NumVertexElements;
    }

    uint32_t Yw3dVertexFormat::GetHighestStream() const
    {
        return m_HighestStream;
    }

    Yw3dVertexElement* Yw3dVertexFormat::GetElements() const
    {
        return m_Elements;
    }
}

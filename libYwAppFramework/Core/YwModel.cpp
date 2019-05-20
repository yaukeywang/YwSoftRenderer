// Add by Yaukey at 2019-03-05.
// YW Soft Renderer mesh struct.

#include "YwBase.h"
#include "YwModel.h"

namespace yw
{
    Model::Model() :
        m_VertexFormat(nullptr),
        m_VertexBuffer(nullptr)
    {
    }

    Model::~Model()
    {
        // Clear all group.
        for (size_t i = 0; i < m_Groups.size(); i++)
        {
            ModelGroup* meshGroup = m_Groups[i];
            YW_SAFE_DELETE(meshGroup);
        }

        m_Groups.clear();

        // Clear all triangles.
        for (size_t i = 0; i < m_Triangles.size(); i++)
        {
            ModelTriangle* meshTriangle = m_Triangles[i];
            YW_SAFE_DELETE(meshTriangle);
        }

        m_Triangles.clear();

        // Clear device vertex data.
        YW_SAFE_RELEASE(m_VertexFormat);
        YW_SAFE_RELEASE(m_VertexBuffer);
        for (size_t i = 0; i < m_IndexBuffers.size(); i++)
        {
            YW_SAFE_RELEASE(m_IndexBuffers[i]);
        }

        m_IndexBuffers.clear();
    }

    ModelGroup* Model::AddGroup(const StringA& groupName)
    {
        ModelGroup* group = FindGroup(groupName);
        if (nullptr == group)
        {
            group = new ModelGroup(groupName);
            m_Groups.push_back(group);
        }
        
        return group;
    }

    ModelGroup* Model::FindGroup(const StringA& groupName)
    {
        for (int32_t i = 0; i < (int32_t)m_Groups.size(); i++)
        {
            ModelGroup* group = m_Groups[i];
            if (groupName == group->m_Name)
            {
                return group;
            }
        }

        return nullptr;
    }

    Yw3dVertexElement Model::s_VertexDeclaration[6] = 
    {
        YW3D_VERTEX_FORMAT_DECL(0, Yw3d_VET_Vector3, 0),
        YW3D_VERTEX_FORMAT_DECL(0, Yw3d_VET_Vector3, 1),
        YW3D_VERTEX_FORMAT_DECL(0, Yw3d_VET_Vector3, 2),
        YW3D_VERTEX_FORMAT_DECL(0, Yw3d_VET_Vector4, 3),
        YW3D_VERTEX_FORMAT_DECL(0, Yw3d_VET_Vector2, 4),
        YW3D_VERTEX_FORMAT_DECL(0, Yw3d_VET_Vector2, 5)
    };

    bool Model::CreateVertexData(Yw3dDevice* device)
    {
        if (nullptr == device)
        {
            return false;
        }

        // ------------------------------------------------------------------
        // Create data.

        // Create vertex format, release old vertex format data.
        YW_SAFE_RELEASE(m_VertexFormat);
        if (YW3D_FAILED(device->CreateVertexFormat(&m_VertexFormat, s_VertexDeclaration, sizeof(s_VertexDeclaration))))
        {
            return false;
        }

        // Create vertex buffer, release old vertex buffer data.
        YW_SAFE_RELEASE(m_VertexBuffer);
        if (YW3D_FAILED(device->CreateVertexBuffer(&m_VertexBuffer, sizeof(Vertexformat) * (uint32_t)m_Vertices.size())))
        {
            return false;
        }

        // Get vertex buffer pointer.
        Vertexformat* vertexFormat = nullptr;
        if (YW3D_FAILED(m_VertexBuffer->GetPointer(0, (void**)&vertexFormat)))
        {
            return false;
        }

        // Fill vertex buffer data, through by triangles.
        for (int32_t i = 0; i < (int32_t)m_Triangles.size(); i++)
        {
            ModelTriangle* triangle = m_Triangles[i];
            for (int32_t j = 0; j < 3; j++)
            {
                Vertexformat& vf = vertexFormat[triangle->m_VertexIndices[j]];
                vf.position = m_Vertices[triangle->m_VertexIndices[j]];
                vf.normal = m_Normals[triangle->m_NormalIndices[j]];
                vf.tangent = (m_Tangents.size() > 0) ? m_Tangents[triangle->m_VertexIndices[j]] : Vector3::Zero();
                vf.color = (m_Colors.size() > 0) ? m_Colors[triangle->m_VertexIndices[j]] : Vector4::White();
                vf.texcoord = (m_Texcoords.size() > 0) ? m_Texcoords[triangle->m_TexcoordsIndices[j]] : Vector2::Zero();
                vf.texcoord2 = (m_Texcoord2s.size() > 0) ? m_Texcoord2s[triangle->m_Texcoords2Indices[j]] : Vector2::Zero();
            }
        }

        // Release old index buffer data.
        for (int i = 0; i < (int32_t)m_IndexBuffers.size(); i++)
        {
            YW_SAFE_RELEASE(m_IndexBuffers[i]);
        }

        m_IndexBuffers.clear();

        // Create and fill index buffer data by each group.
        for (int32_t i = 0; i < (int32_t)m_Groups.size(); i++)
        {
            ModelGroup* group = m_Groups[i];
            if (nullptr == group)
            {
                continue;
            }

            uint32_t triangleCount = (uint32_t)group->m_Triangles.size();
            if (0 == triangleCount)
            {
                continue;
            }

            // Create index buffer.
            Yw3dIndexBuffer* indexBuffer = nullptr;
            if (YW3D_FAILED(device->CreateIndexBuffer(&indexBuffer, sizeof(uint16_t) * triangleCount * 3, Yw3d_FMT_INDEX16)))
            {
                return false;
            }

            // Get index buffer pointer.
            uint16_t* indices = nullptr;
            if (YW3D_FAILED(indexBuffer->GetPointer(0, (void**)&indices)))
            {
                return false;
            }

            // Fill index buffer data.
            for (int32_t j = 0; j < (int32_t)group->m_Triangles.size(); j++)
            {
                ModelTriangle* triangle = m_Triangles[j];
                for (int32_t k = 0; k < 3; k++)
                {
                    *indices++ = triangle->m_VertexIndices[k];
                }
            }

            m_IndexBuffers.push_back(indexBuffer);
        }

        return true;
    }
}

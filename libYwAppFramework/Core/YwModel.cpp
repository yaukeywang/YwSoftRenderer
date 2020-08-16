// Add by Yaukey at 2019-03-05.
// YW Soft Renderer mesh struct.

#include "YwBase.h"
#include "YwModel.h"
#include "YwGraphics.h"

namespace yw
{
    Model::Model() :
        m_ReadOnly(true),
        m_VertexFormat(nullptr),
        m_VertexBuffer(nullptr),
        m_TotalVertexCount(0)
    {
    }

    Model::Model(const StringA& modelName, bool readOnly) : 
        m_VertexFormat(nullptr),
        m_VertexBuffer(nullptr),
        m_TotalVertexCount(0)
    {
        m_Name = modelName;
        m_ReadOnly = readOnly;
    }

    Model::~Model()
    {
        // Clear all group.
        for (size_t i = 0; i < m_Groups.size(); i++)
        {
            ModelGroup* modelGroup = m_Groups[i];
            YW_SAFE_DELETE(modelGroup);
        }

        m_Groups.clear();

        // Clear all triangles.
        for (size_t i = 0; i < m_Triangles.size(); i++)
        {
            ModelTriangle* modelTriangle = m_Triangles[i];
            YW_SAFE_DELETE(modelTriangle);
        }

        m_Triangles.clear();

        // Clear device vertex data.
        YW_SAFE_RELEASE(m_VertexFormat);
        YW_SAFE_RELEASE(m_VertexBuffer);
        for (size_t i = 0; i < m_IndexBuffers.size(); i++)
        {
            YW_SAFE_RELEASE(m_IndexBuffers[i].indexBuffer);
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
        YW3D_VERTEX_FORMAT_DECL(0, Yw3d_VET_Vector3, 0), // Position.
        YW3D_VERTEX_FORMAT_DECL(0, Yw3d_VET_Vector3, 1), // Normal.
        YW3D_VERTEX_FORMAT_DECL(0, Yw3d_VET_Vector4, 2), // Tangent.
        YW3D_VERTEX_FORMAT_DECL(0, Yw3d_VET_Vector4, 3), // Color.
        YW3D_VERTEX_FORMAT_DECL(0, Yw3d_VET_Vector2, 4), // Texcoord.
        YW3D_VERTEX_FORMAT_DECL(0, Yw3d_VET_Vector2, 5) // Texcoord2.
    };

    bool Model::CreateVertexData(Yw3dDevice* device)
    {
        if (nullptr == device)
        {
            return false;
        }

        if (m_Vertices.empty())
        {
            return false;
        }

        // ------------------------------------------------------------------
        // Create data.

        // Update vertex count.
        m_TotalVertexCount = (int32_t)m_Vertices.size();

        // Create vertex format, release old vertex format data.
        YW_SAFE_RELEASE(m_VertexFormat);
        if (YW3D_FAILED(device->CreateVertexFormat(&m_VertexFormat, s_VertexDeclaration, sizeof(s_VertexDeclaration))))
        {
            return false;
        }

        // Create vertex buffer, release old vertex buffer data.
        YW_SAFE_RELEASE(m_VertexBuffer);
        if (YW3D_FAILED(device->CreateVertexBuffer(&m_VertexBuffer, sizeof(ModelVertexFormat) * (uint32_t)m_Vertices.size())))
        {
            return false;
        }

        // Get vertex buffer pointer.
        ModelVertexFormat* vertexFormat = nullptr;
        if (YW3D_FAILED(m_VertexBuffer->GetPointer(0, (void**)&vertexFormat)))
        {
            return false;
        }

        // Fill vertex buffer data, through by triangles.
        memcpy(vertexFormat, m_Vertices.data(), (uint32_t)m_Vertices.size() * sizeof(ModelVertexFormat));

        // Release old index buffer data.
        for (int i = 0; i < (int32_t)m_IndexBuffers.size(); i++)
        {
            YW_SAFE_RELEASE(m_IndexBuffers[i].indexBuffer);
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

            // Get total triangle count in this group.
            int32_t triangleCount = (int32_t)group->m_Triangles.size();
            if (triangleCount <= 0)
            {
                continue;
            }

            // Get total index buffer data length of this grop.
            uint32_t indexDataLength = (uint32_t)group->m_TriangleIndices.size() * sizeof(uint32_t);

            // Create index buffer.
            Yw3dIndexBuffer* indexBuffer = nullptr;
            if (YW3D_FAILED(device->CreateIndexBuffer(&indexBuffer, indexDataLength, Yw3d_FMT_INDEX32)))
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
            memcpy(indices, group->m_TriangleIndices.data(), indexDataLength);

            // Push the index buffer of this group.
            m_IndexBuffers.push_back(ModelIndexBufferElement(indexBuffer, triangleCount));
        }

        // Clear base model data after finishing created graphics data if the model is read-only.
        if (ReadOnly())
        {
            ClearBaseModelData();
        }

        return true;
    }

    int Model::Render(Yw3dDevice* device) const
    {
        if (nullptr == device)
        {
            return 0;
        }

        device->SetVertexFormat(m_VertexFormat);
        device->SetVertexStream(0, m_VertexBuffer, 0, sizeof(ModelVertexFormat));

        int32_t renderedGroups = 0;
        for (int32_t i = 0; i < (int32_t)m_IndexBuffers.size(); i++, renderedGroups++)
        {
            const ModelIndexBufferElement& indexBuffer = m_IndexBuffers[i];
            device->SetIndexBuffer(indexBuffer.indexBuffer);
            device->DrawIndexedPrimitive(Yw3d_PT_TriangleList, 0, 0, m_TotalVertexCount, 0, indexBuffer.primitiveCount);
        }

        return renderedGroups;
    }

    int Model::Render(Graphics* graphics) const
    {
        if (nullptr == graphics)
        {
            return 0;
        }

        Yw3dDevice* device = graphics->GetYw3dDevice();
        if (nullptr == device)
        {
            return 0;
        }

        graphics->SetVertexFormat(m_VertexFormat);
        graphics->SetVertexStream(0, m_VertexBuffer, 0, sizeof(ModelVertexFormat));

        int32_t renderedGroups = 0;
        for (int32_t i = 0; i < (int32_t)m_IndexBuffers.size(); i++, renderedGroups++)
        {
            const ModelIndexBufferElement& indexBuffer = m_IndexBuffers[i];
            graphics->SetIndexBuffer(indexBuffer.indexBuffer);
            device->DrawIndexedPrimitive(Yw3d_PT_TriangleList, 0, 0, m_TotalVertexCount, 0, indexBuffer.primitiveCount);
        }

        return renderedGroups;
    }

    void Model::ClearBaseModelData()
    {
        m_Positions.clear();
        m_FacetNormals.clear();
        m_Normals.clear();
        m_Texcoords.clear();
        m_Texcoord2s.clear();
        m_Tangents.clear();
        m_Colors.clear();
        m_Vertices.clear();

        // Clear all group.
        for (size_t i = 0; i < m_Groups.size(); i++)
        {
            ModelGroup* modelGroup = m_Groups[i];
            YW_SAFE_DELETE(modelGroup);
        }

        m_Groups.clear();

        // Clear all triangles.
        for (size_t i = 0; i < m_Triangles.size(); i++)
        {
            ModelTriangle* modelTriangle = m_Triangles[i];
            YW_SAFE_DELETE(modelTriangle);
        }

        m_Triangles.clear();
    }
}

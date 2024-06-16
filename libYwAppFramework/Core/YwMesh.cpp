// Add by Yaukey at 2024-06-11.
// YW Soft Renderer mesh struct.

#include "YwMesh.h"
#include "YwBase.h"
#include "YwGraphics.h"

namespace yw
{
    // ------------------------------------------------------------------
    // MeshVertex class implementation.
    MeshVertex::MeshVertex()
    { 
        Reset();
    }

    void MeshVertex::Reset()
    { 
        memset(this, 0, sizeof(MeshVertex));
    }

    // ------------------------------------------------------------------
    // MeshVertexBoneWeight class implementation.
    MeshVertexBoneWeight::MeshVertexBoneWeight()
    {
        for (int32_t i = 0; i < MAX_BONES_PER_VERTEX; i++)
        {
            boneIndices[i] = -1;
            weights[i] = 0.0f;
        }
    }

    // ------------------------------------------------------------------
    // Mesh MeshIndexBufferElement class implementation.

    MeshIndexBufferElement::MeshIndexBufferElement() : 
        indexBuffer(nullptr), 
        primitiveCount(0)
    {
    }

    MeshIndexBufferElement::MeshIndexBufferElement(Yw3dIndexBuffer* buffer, int32_t count) : 
        indexBuffer(buffer), 
        primitiveCount(count)
    {
    }

    // ------------------------------------------------------------------
    // Mesh MeshTriangle class implementation.

    // Constructor.
    MeshTriangle::MeshTriangle()
    {
        for (int32_t i = 0; i < 3; i++)
        {
            positionIndices[i] = 0;
            normalIndices[i] = 0;
            texcoordsIndices[i] = 0;
            texcoords2Indices[i] = 0;
            vertexIndices[i] = 0;
            facetNormalIndex = 0;
        }
    }

    // Destructor.
    MeshTriangle::~MeshTriangle()
    {
    }

    // ------------------------------------------------------------------
    // Mesh VBO class implementation.

    Yw3dVertexElement MeshVBO::s_VertexDeclaration[6] =
    {
        YW3D_VERTEX_FORMAT_DECL(0, Yw3d_VET_Vector3, 0), // Position.
        YW3D_VERTEX_FORMAT_DECL(0, Yw3d_VET_Vector3, 1), // Normal.
        YW3D_VERTEX_FORMAT_DECL(0, Yw3d_VET_Vector4, 2), // Tangent.
        YW3D_VERTEX_FORMAT_DECL(0, Yw3d_VET_Vector4, 3), // Color.
        YW3D_VERTEX_FORMAT_DECL(0, Yw3d_VET_Vector2, 4), // Texcoord.
        YW3D_VERTEX_FORMAT_DECL(0, Yw3d_VET_Vector2, 5) // Texcoord2.
    };

    MeshVBO::MeshVBO(Yw3dDevice* device, const std::vector<MeshVertex>& vertices, const std::vector<SubMesh*>& allSubMeshes) :
        m_VertexFormat(nullptr), 
        m_VertexBuffer(nullptr), 
        m_TotalVertexCount(0)
    {
        Create(device, vertices, allSubMeshes);
    }

    MeshVBO::~MeshVBO()
    {
        Clear();
    }

    int32_t MeshVBO::Render(Yw3dDevice* device) const
    {
        if (nullptr == device)
        {
            return 0;
        }

        device->SetVertexFormat(m_VertexFormat);
        device->SetVertexStream(0, m_VertexBuffer, 0, sizeof(MeshVertex));

        int32_t renderedGroups = 0;
        for (int32_t i = 0; i < (int32_t)m_IndexBuffers.size(); i++, renderedGroups++)
        {
            const MeshIndexBufferElement& indexBuffer = m_IndexBuffers[i];
            device->SetIndexBuffer(indexBuffer.indexBuffer);
            device->DrawIndexedPrimitive(Yw3d_PT_TriangleList, 0, 0, m_TotalVertexCount, 0, indexBuffer.primitiveCount);
        }

        return renderedGroups;
    }

    int32_t MeshVBO::Render(Graphics* graphics) const
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
        graphics->SetVertexStream(0, m_VertexBuffer, 0, sizeof(MeshVertex));

        int32_t renderedGroups = 0;
        for (int32_t i = 0; i < (int32_t)m_IndexBuffers.size(); i++, renderedGroups++)
        {
            const MeshIndexBufferElement& indexBuffer = m_IndexBuffers[i];
            graphics->SetIndexBuffer(indexBuffer.indexBuffer);
            device->DrawIndexedPrimitive(Yw3d_PT_TriangleList, 0, 0, m_TotalVertexCount, 0, indexBuffer.primitiveCount);
        }

        return renderedGroups;
    }

    bool MeshVBO::Create(Yw3dDevice* device, const std::vector<MeshVertex>& vertices, const std::vector<SubMesh*>& allSubMeshes)
    {
        if (nullptr == device)
        {
            return false;
        }

        if (vertices.empty())
        {
            return false;
        }

        // Create data.

        // Update vertex count.
        m_TotalVertexCount = (int32_t)vertices.size();

        // Create vertex format, release old vertex format data.
        YW_SAFE_RELEASE(m_VertexFormat);
        if (YW3D_FAILED(device->CreateVertexFormat(&m_VertexFormat, s_VertexDeclaration, sizeof(s_VertexDeclaration))))
        {
            return false;
        }

        // Create vertex buffer, release old vertex buffer data.
        YW_SAFE_RELEASE(m_VertexBuffer);
        if (YW3D_FAILED(device->CreateVertexBuffer(&m_VertexBuffer, sizeof(MeshVertex) * (uint32_t)vertices.size())))
        {
            return false;
        }

        // Get vertex buffer pointer.
        MeshVertex* vertexFormat = nullptr;
        if (YW3D_FAILED(m_VertexBuffer->GetPointer(0, (void**)&vertexFormat)))
        {
            return false;
        }

        // Fill vertex buffer data, through by triangles.
        memcpy(vertexFormat, vertices.data(), (uint32_t)vertices.size() * sizeof(MeshVertex));

        // Release old index buffer data.
        for (int i = 0; i < (int32_t)m_IndexBuffers.size(); i++)
        {
            YW_SAFE_RELEASE(m_IndexBuffers[i].indexBuffer);
        }

        m_IndexBuffers.clear();

        // Create and fill index buffer data by each group.
        for (int32_t i = 0; i < (int32_t)allSubMeshes.size(); i++)
        {
            SubMesh* subMesh = allSubMeshes[i];
            if (nullptr == subMesh)
            {
                continue;
            }

            // Get total triangle count in this group.
            int32_t triangleCount = (int32_t)subMesh->triangles.size();
            if (triangleCount <= 0)
            {
                continue;
            }

            // Get total index buffer data length of this grop.
            uint32_t indexDataLength = (uint32_t)subMesh->triangleIndices.size() * sizeof(uint32_t);

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
            memcpy(indices, subMesh->triangleIndices.data(), indexDataLength);

            // Push the index buffer of this group.
            m_IndexBuffers.push_back(MeshIndexBufferElement(indexBuffer, triangleCount));
        }

        return true;
    }

    void MeshVBO::Clear()
    {
        // Clear device vertex data.
        YW_SAFE_RELEASE(m_VertexFormat);
        YW_SAFE_RELEASE(m_VertexBuffer);
        for (size_t i = 0; i < m_IndexBuffers.size(); i++)
        {
            YW_SAFE_RELEASE(m_IndexBuffers[i].indexBuffer);
        }

        m_IndexBuffers.clear();
    }

    // ------------------------------------------------------------------
    // Mesh class implementation.

    Mesh::Mesh(const StringA& meshName) :
        m_Name(meshName),
        m_VBO(nullptr)
    {
    }

    Mesh::~Mesh()
    {
        Clear();
    }

    SubMesh* Mesh::AddSubMesh(const StringA& subMeshName)
    {
        SubMesh* subMesh = FindSubMesh(subMeshName);
        if (nullptr == subMesh)
        {
            subMesh = new SubMesh(subMeshName);
            m_AllSubMeshes.push_back(subMesh);
        }

        return subMesh;
    }

    SubMesh* Mesh::FindSubMesh(const StringA& subMeshName)
    {
        for (int32_t i = 0; i < (int32_t)m_AllSubMeshes.size(); i++)
        {
            SubMesh* subMesh = m_AllSubMeshes[i];
            if (subMeshName == subMesh->name)
            {
                return subMesh;
            }
        }

        return nullptr;
    }

    bool Mesh::CreateVBOData(Yw3dDevice* device)
    {
        ClearVBOData();
        m_VBO = new MeshVBO(device, m_Vertices, m_AllSubMeshes);

        return (nullptr != m_VBO);
    }

    int32_t Mesh::Render(Yw3dDevice* device) const
    {
        if (nullptr == device)
        {
            return 0;
        }
;
        if (nullptr == m_VBO)
        {
            return 0;
        }

        return m_VBO->Render(device);
    }

    int32_t Mesh::Render(Graphics* graphics) const
    {
        if (nullptr == graphics)
        {
            return 0;
        }

        if (nullptr == m_VBO)
        {
            return 0;
        }

        return m_VBO->Render(graphics);
    }

    void Mesh::Clear()
    {
        ClearBaseData();
        ClearVBOData();
    }

    void Mesh::ClearBaseData()
    {
        m_Positions.clear();
        m_FacetNormals.clear();
        m_Normals.clear();
        m_Texcoords.clear();
        m_Texcoord2s.clear();
        m_Tangents.clear();
        m_Colors.clear();
        m_Vertices.clear();
        m_BoneWeights.clear();

        // Clear all vertex cache index info.
        for (int32_t i = 0; i < (int32_t)m_VertexIndexCache.size(); i++)
        {
            MeshVertexIndex* indexInfo = m_VertexIndexCache[i];
            while (nullptr != indexInfo)
            {
                MeshVertexIndex* nextIndexInfo = indexInfo->next;
                YW_SAFE_DELETE(indexInfo);
                indexInfo = nextIndexInfo;
            }
        }

        m_VertexIndexCache.clear();

        // Clear all triangles.
        for (size_t i = 0; i < m_Triangles.size(); i++)
        {
            MeshTriangle* meshTriangle = m_Triangles[i];
            YW_SAFE_DELETE(meshTriangle);
        }

        m_Triangles.clear();

        // Clear all group.
        for (size_t i = 0; i < m_AllSubMeshes.size(); i++)
        {
            SubMesh* subMesh = m_AllSubMeshes[i];
            YW_SAFE_DELETE(subMesh);
        }

        m_AllSubMeshes.clear();
    }

    void Mesh::ClearVBOData()
    {
        YW_SAFE_DELETE(m_VBO);
    }
}

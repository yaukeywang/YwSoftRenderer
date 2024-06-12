// Add by Yaukey at 2019-03-05.
// YW Soft Renderer mesh struct.

#include "YwModel.h"
#include "YwBase.h"
#include "YwGraphics.h"

namespace yw
{
    Model::Model(const StringA& modelName, bool readOnly) :
        m_Name(modelName),
        m_ReadOnly(readOnly)
    {
        m_Name = modelName;
        m_ReadOnly = readOnly;
        m_Mesh = new Mesh(modelName);
    }

    Model::~Model()
    {
        // Clear mesh data.
        YW_SAFE_DELETE(m_Mesh);
    }

    SubMesh* Model::AddSubMesh(const StringA& subMeshName)
    {
        if (nullptr == m_Mesh)
        {
            return nullptr;
        }

        return m_Mesh->AddSubMesh(subMeshName);
    }

    SubMesh* Model::FindSubMesh(const StringA& subMeshName)
    {
        if (nullptr == m_Mesh)
        {
            return nullptr;
        }

        return m_Mesh->FindSubMesh(subMeshName);
    }

    int Model::Render(Yw3dDevice* device) const
    {
        if (nullptr == device)
        {
            return 0;
        }

        if (nullptr == m_Mesh)
        {
            return 0;
        }

        return m_Mesh->Render(device);
    }

    int Model::Render(Graphics* graphics) const
    {
        if (nullptr == graphics)
        {
            return 0;
        }

        if (nullptr == m_Mesh)
        {
            return 0;
        }

        return m_Mesh->Render(graphics);
    }
}

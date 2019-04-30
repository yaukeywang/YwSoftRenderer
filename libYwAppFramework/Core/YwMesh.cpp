// Add by Yaukey at 2019-03-05.
// YW Soft Renderer mesh struct.

#include "YwBase.h"
#include "YwMesh.h"

namespace yw
{
    Mesh::Mesh() :
        m_Triangles(0)
    {
    }

    Mesh::~Mesh()
    {
        for (size_t i = 0; i < m_MeshGroups.size(); i++)
        {
            MeshGroup* meshGroup = m_MeshGroups[i];
            YW_SAFE_DELETE(meshGroup);
        }

        m_MeshGroups.clear();
    }

    MeshGroup* Mesh::AddGroup(const StringA& groupName, MeshGroup* meshGroup)
    {
        MeshGroup* group = FindGroup(groupName);
        if (nullptr == group)
        {
            m_MeshGroups.push_back(meshGroup);
            return meshGroup;
        }
        else
        {
            return nullptr;
        }
    }

    MeshGroup* Mesh::FindGroup(const StringA& groupName)
    {
        for (int i = 0; i < (int)m_MeshGroups.size(); i++)
        {
            MeshGroup* group = m_MeshGroups[i];
            if (groupName == group->m_Name)
            {
                return group;
            }
        }

        return nullptr;
    }
}

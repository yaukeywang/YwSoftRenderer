// Add by Yaukey at 2019-03-05.
// YW Soft Renderer mesh struct.

#include "YwBase.h"
#include "YwModel.h"

namespace yw
{
    Model::Model() :
        m_Triangles(0)
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
    }

    ModelGroup* Model::AddGroup(const StringA& groupName, ModelGroup* meshGroup)
    {
        ModelGroup* group = FindGroup(groupName);
        if (nullptr == group)
        {
            m_Groups.push_back(meshGroup);
            return meshGroup;
        }
        else
        {
            return nullptr;
        }
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
}

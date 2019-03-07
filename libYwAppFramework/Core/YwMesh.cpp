// Add by Yaukey at 2019-03-05.
// YW Soft Renderer mesh struct.

#include "YwBase.h"
#include "YwMesh.h"

namespace yw
{
    Mesh::Mesh()
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
}

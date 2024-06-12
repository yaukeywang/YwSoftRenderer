// Add by Yaukey at 2019-03-05.
// YW Soft Renderer model struct.

#ifndef __YW_MODEL_H__
#define __YW_MODEL_H__

#include "YwBase.h"
#include "YwMath.h"
#include "Yw3d.h"
#include "YwMesh.h"

namespace yw
{
    // The model info.
    class Model
    {
    public:
        // Constructor.
        Model(const StringA& modelName, bool readOnly = true);
        
        // Destructor.
        ~Model();

    public:
        // Get model name.
        inline const StringA& GetName() const
        {
            return m_Name;
        }

        // If this model data is read-only.
        inline bool ReadOnly() const
        {
            return m_ReadOnly;
        }

        inline Mesh* GetMesh()
        {
            return m_Mesh;
        }

        // Add a group.
        SubMesh* AddSubMesh(const StringA& subMeshName);

        // Find a group.
        SubMesh* FindSubMesh(const StringA& subMeshName);

        //// Create all vertex related data.
        //bool CreateVertexData(Yw3dDevice* device);

        // Render this model directly with device.
        // Return: How many groups rendered.
        int Render(Yw3dDevice* device) const;

        // Render this model with graphics management.
        // Return: How many groups rendered.
        int Render(class Graphics* graphics) const;

    private:
        // The model name.
        StringA m_Name;

        // If this model data is read-only.
        bool m_ReadOnly;

        // The mesh and sub-meshes associated with this model.
        Mesh* m_Mesh;
    };
}

#endif // !__YW_MODEL_H__

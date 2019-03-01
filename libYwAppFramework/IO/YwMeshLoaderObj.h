// Add by Yaukey at 2019-03-01.
// YW mesh loader for obj file class.

#ifndef __YW_MESH_LOADER_OBJ_H__
#define __YW_MESH_LOADER_OBJ_H__

#include "YwMeshLoader.h"

namespace yw
{
    // Base mesh loader class.
    class MeshLoaderObj : public IMeshLoader
    {
    public:
        // Constructor.
        MeshLoaderObj();
        
        // Destructor.
        ~MeshLoaderObj();
        
    public:
        // Load mesh data from a file.
        // @param[in] fileName the full path of the mesh file.
        // @return true if the function succeeds.
        bool Load(const String& fileName);
        
    private:
    };
}

#endif // !__YW_MESH_LOADER_OBJ_H__

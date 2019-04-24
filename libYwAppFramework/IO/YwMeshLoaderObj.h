// Add by Yaukey at 2019-03-01.
// YW mesh loader for obj file class.
// Modified version of glm.h
// Nate Robins, 1997, 2000
// nate@pobox.com, http://www.pobox.com/~nate
// Wavefront OBJ model file format reader/writer/manipulator.
// YW mesh loader for obj file class.

#ifndef __YW_MESH_LOADER_OBJ_H__
#define __YW_MESH_LOADER_OBJ_H__

#include <stdio.h>
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
        // Load mesh data from a file, classes devired from this need to implement their own.
        // @param[in] fileName the full path of the mesh file.
        // @return Mesh pointer of the loaded mesh, null if failed.
        class Mesh* Load(const StringA& fileName);
        
    private:
        // First pass of process.
        void FirstPass(class Mesh* mesh, FILE* objFile);
        
        // Second pass of process.
        void SecondPass(class Mesh* mesh, FILE* objFile);

        // Read material.
        void ReadMTL(class Mesh* mesh, StringA name);
    };
}

#endif // !__YW_MESH_LOADER_OBJ_H__

// Add by Yaukey at 2019-03-01.
// YW mesh loader for obj file class.
// Modified version of glm.h
// Nate Robins, 1997, 2000
// nate@pobox.com, http://www.pobox.com/~nate
// Wavefront OBJ model file format reader/writer/manipulator.
// YW mesh loader for obj file class.

#include "YwMesh.h"
#include "YwMeshLoaderObj.h"

// warning C4996: 'strdup': The POSIX name for this item is deprecated. Instead, use the ISO C and C++ conformant name: _strdup. See online help for details.
#pragma warning(disable:4996)

namespace yw
{
    MeshLoaderObj::MeshLoaderObj() : IMeshLoader()
    {
        
    }
    
    MeshLoaderObj::~MeshLoaderObj()
    {
        // Release each group.
    }
    
    Mesh* MeshLoaderObj::Load(const StringA& fileName)
    {
        // Alloc a mesh class.
        Mesh* objMesh = new Mesh();
        
        // Open the file.
        FILE* objFile = fopen(fileName.c_str(), "rt");
        if (nullptr == objFile)
        {
            return nullptr;
        }
        
        // Make a first pass through the file to get a count of the number of vertices, normals, texcoords & triangles.
        FirstPass(objMesh, objFile);
        
        /* rewind to beginning of file and read in the data this pass */
        rewind(objFile);
        
        // Second pass to organize data.
        SecondPass(objMesh, objFile);
        
        /* Close the file. */
        fclose(objFile);
        
        return objMesh;
    }
    
    void MeshLoaderObj::FirstPass(class Mesh* mesh, FILE* objFile)
    {
        uint32_t numVertices = 0;           /* number of vertices in model */
        uint32_t numNormals = 0;            /* number of normals in model */
        uint32_t numTexcoords = 0;          /* number of texcoords in model */
        uint32_t numTriangles = 0;          /* number of triangles in model */
        
        /* make a default group */
        MeshGroup* defaultMeshGroup = new MeshGroup("default");
        mesh->m_MeshGroups.push_back(defaultMeshGroup);
        
        /* current group */
        MeshGroup* group = defaultMeshGroup;
        int v, n, t;
        char buf[128];

        numVertices = numNormals = numTexcoords = numTriangles = 0;
        while (fscanf(objFile, "%s", buf) != EOF)
        {
            switch (buf[0]) {
            case '#':               /* comment */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), objFile);
                break;
            case 'v':               /* v, vn, vt */
                switch (buf[1]) {
                case '\0':          /* vertex */
                    /* eat up rest of line */
                    fgets(buf, sizeof(buf), objFile);
                    numVertices++;
                    break;
                case 'n':           /* normal */
                    /* eat up rest of line */
                    fgets(buf, sizeof(buf), objFile);
                    numNormals++;
                    break;
                case 't':           /* texcoord */
                    /* eat up rest of line */
                    fgets(buf, sizeof(buf), objFile);
                    numTexcoords++;
                    break;
                default:
                    LOGE_A("FirstPass(): Unknown token \"%s\".\n", buf);
                    exit(1);
                    break;
                }
                break;
            case 'm':
                fgets(buf, sizeof(buf), objFile);
                sscanf(buf, "%s %s", buf, buf);
                mesh->m_MaterialName = strdup(buf);
                ReadMTL(mesh, buf);
                break;
            case 'u':
                /* eat up rest of line */
                fgets(buf, sizeof(buf), objFile);
                break;
            case 'g':               /* group */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), objFile);
#if MESH_OBJ_LOAD_SINGLE_STRING_GROUP_NAMES
                sscanf(buf, "%s", buf);
#else
                buf[strlen(buf) - 1] = '\0';  /* nuke '\n' */
#endif
                group = new MeshGroup(buf);
                mesh->m_MeshGroups.push_back(group);
                break;
            case 'f':               /* face */
                v = n = t = 0;
                fscanf(objFile, "%s", buf);
                /* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
                if (strstr(buf, "//")) 
                {
                    /* v//n */
                    sscanf(buf, "%d//%d", &v, &n);
                    fscanf(objFile, "%d//%d", &v, &n);
                    fscanf(objFile, "%d//%d", &v, &n);
                    numTriangles++;
                    mesh->m_Triangles.push_back(new MeshTriangle());
                    while (fscanf(objFile, "%d//%d", &v, &n) > 0)
                    {
                        numTriangles++;
                        mesh->m_Triangles.push_back(new MeshTriangle());
                    }
                }
                else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3)
                {
                    /* v/t/n */
                    fscanf(objFile, "%d/%d/%d", &v, &t, &n);
                    fscanf(objFile, "%d/%d/%d", &v, &t, &n);
                    numTriangles++;
                    mesh->m_Triangles.push_back(new MeshTriangle());
                    while (fscanf(objFile, "%d/%d/%d", &v, &t, &n) > 0)
                    {
                        numTriangles++;
                        mesh->m_Triangles.push_back(new MeshTriangle());
                    }
                }
                else if (sscanf(buf, "%d/%d", &v, &t) == 2)
                {
                    /* v/t */
                    fscanf(objFile, "%d/%d", &v, &t);
                    fscanf(objFile, "%d/%d", &v, &t);
                    numTriangles++;
                    mesh->m_Triangles.push_back(new MeshTriangle());
                    while (fscanf(objFile, "%d/%d", &v, &t) > 0)
                    {
                        numTriangles++;
                        mesh->m_Triangles.push_back(new MeshTriangle());
                    }
                }
                else 
                {
                    /* v */
                    fscanf(objFile, "%d", &v);
                    fscanf(objFile, "%d", &v);
                    numTriangles++;
                    mesh->m_Triangles.push_back(new MeshTriangle());
                    while (fscanf(objFile, "%d", &v) > 0)
                    {
                        numTriangles++;
                        mesh->m_Triangles.push_back(new MeshTriangle());
                    }
                }
                break;
            default:
                /* eat up rest of line */
                fgets(buf, sizeof(buf), objFile);
                break;
            }
        }

        /* set the stats in the model structure */
        mesh->m_Vertices.resize(numVertices);
        mesh->m_Normals.resize(numNormals);
        mesh->m_Texcoords.resize(numTexcoords);
    }
    
    void MeshLoaderObj::SecondPass(class Mesh* mesh, FILE* objFile)
    {
        #define TRIANGLE(x) (mesh->m_Triangles[(x)])

        uint32_t numVertices = 0;        /* number of vertices in model */
        uint32_t numNormals = 0;         /* number of normals in model */
        uint32_t numTexcoords = 0;       /* number of texcoords in model */
        uint32_t numTriangles = 0;       /* number of triangles in model */

        // Used for parsing data.
        int v, n, t;
        char buf[128];

        /* set the pointer shortcuts */
        Vector3* vertices = &(mesh->m_Vertices[0]);         /* array of vertices  */
        Vector3* normals = &(mesh->m_Normals[0]);           /* array of normals */
        Vector2* texcoords = &(mesh->m_Texcoords[0]);       /* array of texture coordinates */
        MeshGroup*group = mesh->m_MeshGroups.front();       /* current group pointer */
        void* material = nullptr;                           /* current material (Need to implement) */

        /* on the second pass through the file, read all the data into the
        allocated arrays */
        numVertices = numNormals = numTexcoords = 1;
        numTriangles = 0;
        while (fscanf(objFile, "%s", buf) != EOF) {
            switch (buf[0]) {
            case '#':               /* comment */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), objFile);
                break;
            case 'v':               /* v, vn, vt */
                switch (buf[1]) {
                case '\0':          /* vertex */
                    fscanf(objFile, "%f %f %f",
                        &(vertices[numVertices].x),
                        &(vertices[numVertices].y),
                        &(vertices[numVertices].z));
                    numVertices++;
                    break;
                case 'n':           /* normal */
                    fscanf(objFile, "%f %f %f",
                        &(normals[numNormals].x),
                        &(normals[numNormals].y),
                        &(normals[numNormals].z));
                    numNormals++;
                    break;
                case 't':           /* texcoord */
                    fscanf(objFile, "%f %f",
                        &(texcoords[numTexcoords].x),
                        &(texcoords[numTexcoords].y));
                    numTexcoords++;
                    break;
                }
                break;
            case 'u':
                fgets(buf, sizeof(buf), objFile);
                sscanf(buf, "%s %s", buf, buf);
                group->m_Material = nullptr; // Need to implement material.
                //group->material = material = glmFindMaterial(model, buf);
                break;
            case 'g':               /* group */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), objFile);
#if SINGLE_STRING_GROUP_NAMES
                sscanf(buf, "%s", buf);
#else
                buf[strlen(buf) - 1] = '\0';  /* nuke '\n' */
#endif
                group = mesh->FindGroup(buf);
                group->m_Material = nullptr; // Need to implement material.
                //group = glmFindGroup(model, buf);
                //group->material = material;
                break;
            case 'f':               /* face */
                v = n = t = 0;
                fscanf(objFile, "%s", buf);
                /* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
                if (strstr(buf, "//")) {
                    /* v//n */
                    sscanf(buf, "%d//%d", &v, &n);
                    TRIANGLE(numTriangles)->m_VertexIndices[0] = v < 0 ? v + numVertices : v;
                    TRIANGLE(numTriangles)->m_NormalIndices[0] = n < 0 ? n + numNormals : n;
                    fscanf(objFile, "%d//%d", &v, &n);
                    TRIANGLE(numTriangles)->m_VertexIndices[1] = v < 0 ? v + numVertices : v;
                    TRIANGLE(numTriangles)->m_NormalIndices[1] = n < 0 ? n + numNormals : n;
                    fscanf(objFile, "%d//%d", &v, &n);
                    TRIANGLE(numTriangles)->m_VertexIndices[2] = v < 0 ? v + numVertices : v;
                    TRIANGLE(numTriangles)->m_NormalIndices[2] = n < 0 ? n + numNormals : n;
                    group->m_Triangles.push_back(numTriangles);
                    numTriangles++;
                    while (fscanf(objFile, "%d//%d", &v, &n) > 0) {
                        TRIANGLE(numTriangles)->m_VertexIndices[0] = TRIANGLE(numTriangles - 1)->m_VertexIndices[0];
                        TRIANGLE(numTriangles)->m_NormalIndices[0] = TRIANGLE(numTriangles - 1)->m_NormalIndices[0];
                        TRIANGLE(numTriangles)->m_VertexIndices[1] = TRIANGLE(numTriangles - 1)->m_VertexIndices[2];
                        TRIANGLE(numTriangles)->m_NormalIndices[1] = TRIANGLE(numTriangles - 1)->m_NormalIndices[2];
                        TRIANGLE(numTriangles)->m_VertexIndices[2] = v < 0 ? v + numVertices : v;
                        TRIANGLE(numTriangles)->m_NormalIndices[2] = n < 0 ? n + numNormals : n;
                        group->m_Triangles.push_back(numTriangles);
                        numTriangles++;
                    }
                }
                else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) {
                    /* v/t/n */
                    TRIANGLE(numTriangles)->m_VertexIndices[0] = v < 0 ? v + numVertices : v;
                    TRIANGLE(numTriangles)->m_TexcoordsIndices[0] = t < 0 ? t + numTexcoords : t;
                    TRIANGLE(numTriangles)->m_NormalIndices[0] = n < 0 ? n + numNormals : n;
                    fscanf(objFile, "%d/%d/%d", &v, &t, &n);
                    TRIANGLE(numTriangles)->m_VertexIndices[1] = v < 0 ? v + numVertices : v;
                    TRIANGLE(numTriangles)->m_TexcoordsIndices[1] = t < 0 ? t + numTexcoords : t;
                    TRIANGLE(numTriangles)->m_NormalIndices[1] = n < 0 ? n + numNormals : n;
                    fscanf(objFile, "%d/%d/%d", &v, &t, &n);
                    TRIANGLE(numTriangles)->m_VertexIndices[2] = v < 0 ? v + numVertices : v;
                    TRIANGLE(numTriangles)->m_TexcoordsIndices[2] = t < 0 ? t + numTexcoords : t;
                    TRIANGLE(numTriangles)->m_NormalIndices[2] = n < 0 ? n + numNormals : n;
                    group->m_Triangles.push_back(numTriangles);
                    numTriangles++;
                    while (fscanf(objFile, "%d/%d/%d", &v, &t, &n) > 0) {
                        TRIANGLE(numTriangles)->m_VertexIndices[0] = TRIANGLE(numTriangles - 1)->m_VertexIndices[0];
                        TRIANGLE(numTriangles)->m_TexcoordsIndices[0] = TRIANGLE(numTriangles - 1)->m_TexcoordsIndices[0];
                        TRIANGLE(numTriangles)->m_NormalIndices[0] = TRIANGLE(numTriangles - 1)->m_NormalIndices[0];
                        TRIANGLE(numTriangles)->m_VertexIndices[1] = TRIANGLE(numTriangles - 1)->m_VertexIndices[2];
                        TRIANGLE(numTriangles)->m_TexcoordsIndices[1] = TRIANGLE(numTriangles - 1)->m_TexcoordsIndices[2];
                        TRIANGLE(numTriangles)->m_NormalIndices[1] = TRIANGLE(numTriangles - 1)->m_NormalIndices[2];
                        TRIANGLE(numTriangles)->m_VertexIndices[2] = v < 0 ? v + numVertices : v;
                        TRIANGLE(numTriangles)->m_TexcoordsIndices[2] = t < 0 ? t + numTexcoords : t;
                        TRIANGLE(numTriangles)->m_NormalIndices[2] = n < 0 ? n + numNormals : n;
                        group->m_Triangles.push_back(numTriangles);
                        numTriangles++;
                    }
                }
                else if (sscanf(buf, "%d/%d", &v, &t) == 2) {
                    /* v/t */
                    TRIANGLE(numTriangles)->m_VertexIndices[0] = v < 0 ? v + numVertices : v;
                    TRIANGLE(numTriangles)->m_TexcoordsIndices[0] = t < 0 ? t + numTexcoords : t;
                    fscanf(objFile, "%d/%d", &v, &t);
                    TRIANGLE(numTriangles)->m_VertexIndices[1] = v < 0 ? v + numVertices : v;
                    TRIANGLE(numTriangles)->m_TexcoordsIndices[1] = t < 0 ? t + numTexcoords : t;
                    fscanf(objFile, "%d/%d", &v, &t);
                    TRIANGLE(numTriangles)->m_VertexIndices[2] = v < 0 ? v + numVertices : v;
                    TRIANGLE(numTriangles)->m_TexcoordsIndices[2] = t < 0 ? t + numTexcoords : t;
                    group->m_Triangles.push_back(numTriangles);
                    numTriangles++;
                    while (fscanf(objFile, "%d/%d", &v, &t) > 0) {
                        TRIANGLE(numTriangles)->m_VertexIndices[0] = TRIANGLE(numTriangles - 1)->m_VertexIndices[0];
                        TRIANGLE(numTriangles)->m_TexcoordsIndices[0] = TRIANGLE(numTriangles - 1)->m_TexcoordsIndices[0];
                        TRIANGLE(numTriangles)->m_VertexIndices[1] = TRIANGLE(numTriangles - 1)->m_VertexIndices[2];
                        TRIANGLE(numTriangles)->m_TexcoordsIndices[1] = TRIANGLE(numTriangles - 1)->m_TexcoordsIndices[2];
                        TRIANGLE(numTriangles)->m_VertexIndices[2] = v < 0 ? v + numVertices : v;
                        TRIANGLE(numTriangles)->m_TexcoordsIndices[2] = t < 0 ? t + numTexcoords : t;
                        group->m_Triangles.push_back(numTriangles);
                        numTriangles++;
                    }
                }
                else {
                    /* v */
                    sscanf(buf, "%d", &v);
                    TRIANGLE(numTriangles)->m_VertexIndices[0] = v < 0 ? v + numVertices : v;
                    fscanf(objFile, "%d", &v);
                    TRIANGLE(numTriangles)->m_VertexIndices[1] = v < 0 ? v + numVertices : v;
                    fscanf(objFile, "%d", &v);
                    TRIANGLE(numTriangles)->m_VertexIndices[2] = v < 0 ? v + numVertices : v;
                    group->m_Triangles.push_back(numTriangles);
                    numTriangles++;
                    while (fscanf(objFile, "%d", &v) > 0) {
                        TRIANGLE(numTriangles)->m_VertexIndices[0] = TRIANGLE(numTriangles - 1)->m_VertexIndices[0];
                        TRIANGLE(numTriangles)->m_VertexIndices[1] = TRIANGLE(numTriangles - 1)->m_VertexIndices[2];
                        TRIANGLE(numTriangles)->m_VertexIndices[2] = v < 0 ? v + numVertices : v;
                        group->m_Triangles.push_back(numTriangles);
                        numTriangles++;
                    }
                }
                break;

            default:
                /* eat up rest of line */
                fgets(buf, sizeof(buf), objFile);
                break;
            }
        }

#if 0
        /* announce the memory requirements */
        printf(" Memory: %d bytes\n",
            numvertices * 3 * sizeof(GLfloat) +
            numnormals * 3 * sizeof(GLfloat) * (numnormals ? 1 : 0) +
            numtexcoords * 3 * sizeof(GLfloat) * (numtexcoords ? 1 : 0) +
            numtriangles * sizeof(GLMtriangle));
#endif
    }
}

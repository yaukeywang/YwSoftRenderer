// Add by Yaukey at 2019-03-01.
// YW mesh loader for obj file class.
// Modified version of glm.h
// Nate Robins, 1997, 2000
// nate@pobox.com, http://www.pobox.com/~nate
// Wavefront OBJ model file format reader/writer/manipulator.
// YW mesh loader for obj file class.

#include "YwMesh.h"
#include "YwMeshLoaderObj.h"

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
                    group->m_Triangles.push_back(nullptr);
                    while (fscanf(objFile, "%d//%d", &v, &n) > 0) 
                    {
                        numTriangles++;
                        group->m_Triangles.push_back(nullptr);
                    }
                }
                else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) 
                {
                    /* v/t/n */
                    fscanf(objFile, "%d/%d/%d", &v, &t, &n);
                    fscanf(objFile, "%d/%d/%d", &v, &t, &n);
                    numTriangles++;
                    group->m_Triangles.push_back(nullptr);
                    while (fscanf(objFile, "%d/%d/%d", &v, &t, &n) > 0) 
                    {
                        numTriangles++;
                        group->m_Triangles.push_back(nullptr);
                    }
                }
                else if (sscanf(buf, "%d/%d", &v, &t) == 2) 
                {
                    /* v/t */
                    fscanf(objFile, "%d/%d", &v, &t);
                    fscanf(objFile, "%d/%d", &v, &t);
                    numTriangles++;
                    group->m_Triangles.push_back(nullptr);
                    while (fscanf(objFile, "%d/%d", &v, &t) > 0) 
                    {
                        numTriangles++;
                        group->m_Triangles.push_back(nullptr);
                    }
                }
                else 
                {
                    /* v */
                    fscanf(objFile, "%d", &v);
                    fscanf(objFile, "%d", &v);
                    numTriangles++;
                    group->m_Triangles.push_back(nullptr);
                    while (fscanf(objFile, "%d", &v) > 0)
                    {
                        numTriangles++;
                        group->m_Triangles.push_back(nullptr);
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
        mesh->m_Triangles = numTriangles;
    }
    
    void MeshLoaderObj::SecondPass(class Mesh* mesh, FILE* objFile)
    {
#if 0


        GLuint numvertices;        /* number of vertices in model */
        GLuint numnormals;         /* number of normals in model */
        GLuint numtexcoords;       /* number of texcoords in model */
        GLuint numtriangles;       /* number of triangles in model */
        GLfloat* vertices;         /* array of vertices  */
        GLfloat* normals;          /* array of normals */
        GLfloat* texcoords;        /* array of texture coordinates */
        GLMgroup* group;           /* current group pointer */
        GLuint material;           /* current material */
        int v, n, t;
        char buf[128];

        /* set the pointer shortcuts */
        vertices = model->vertices;
        normals = model->normals;
        texcoords = model->texcoords;
        group = model->groups;

        /* on the second pass through the file, read all the data into the
        allocated arrays */
        numvertices = numnormals = numtexcoords = 1;
        numtriangles = 0;
        material = 0;
        while (fscanf(file, "%s", buf) != EOF) {
            switch (buf[0]) {
            case '#':               /* comment */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                break;
            case 'v':               /* v, vn, vt */
                switch (buf[1]) {
                case '\0':          /* vertex */
                    fscanf(file, "%f %f %f",
                        &vertices[3 * numvertices + 0],
                        &vertices[3 * numvertices + 1],
                        &vertices[3 * numvertices + 2]);
                    numvertices++;
                    break;
                case 'n':           /* normal */
                    fscanf(file, "%f %f %f",
                        &normals[3 * numnormals + 0],
                        &normals[3 * numnormals + 1],
                        &normals[3 * numnormals + 2]);
                    numnormals++;
                    break;
                case 't':           /* texcoord */
                    fscanf(file, "%f %f",
                        &texcoords[2 * numtexcoords + 0],
                        &texcoords[2 * numtexcoords + 1]);
                    numtexcoords++;
                    break;
                }
                break;
            case 'u':
                fgets(buf, sizeof(buf), file);
                sscanf(buf, "%s %s", buf, buf);
                group->material = material = glmFindMaterial(model, buf);
                break;
            case 'g':               /* group */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
#if SINGLE_STRING_GROUP_NAMES
                sscanf(buf, "%s", buf);
#else
                buf[strlen(buf) - 1] = '\0';  /* nuke '\n' */
#endif
                group = glmFindGroup(model, buf);
                group->material = material;
                break;
            case 'f':               /* face */
                v = n = t = 0;
                fscanf(file, "%s", buf);
                /* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
                if (strstr(buf, "//")) {
                    /* v//n */
                    sscanf(buf, "%d//%d", &v, &n);
                    T(numtriangles).vindices[0] = v < 0 ? v + numvertices : v;
                    T(numtriangles).nindices[0] = n < 0 ? n + numnormals : n;
                    fscanf(file, "%d//%d", &v, &n);
                    T(numtriangles).vindices[1] = v < 0 ? v + numvertices : v;
                    T(numtriangles).nindices[1] = n < 0 ? n + numnormals : n;
                    fscanf(file, "%d//%d", &v, &n);
                    T(numtriangles).vindices[2] = v < 0 ? v + numvertices : v;
                    T(numtriangles).nindices[2] = n < 0 ? n + numnormals : n;
                    group->triangles[group->numtriangles++] = numtriangles;
                    numtriangles++;
                    while (fscanf(file, "%d//%d", &v, &n) > 0) {
                        T(numtriangles).vindices[0] = T(numtriangles - 1).vindices[0];
                        T(numtriangles).nindices[0] = T(numtriangles - 1).nindices[0];
                        T(numtriangles).vindices[1] = T(numtriangles - 1).vindices[2];
                        T(numtriangles).nindices[1] = T(numtriangles - 1).nindices[2];
                        T(numtriangles).vindices[2] = v < 0 ? v + numvertices : v;
                        T(numtriangles).nindices[2] = n < 0 ? n + numnormals : n;
                        group->triangles[group->numtriangles++] = numtriangles;
                        numtriangles++;
                    }
                }
                else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) {
                    /* v/t/n */
                    T(numtriangles).vindices[0] = v < 0 ? v + numvertices : v;
                    T(numtriangles).tindices[0] = t < 0 ? t + numtexcoords : t;
                    T(numtriangles).nindices[0] = n < 0 ? n + numnormals : n;
                    fscanf(file, "%d/%d/%d", &v, &t, &n);
                    T(numtriangles).vindices[1] = v < 0 ? v + numvertices : v;
                    T(numtriangles).tindices[1] = t < 0 ? t + numtexcoords : t;
                    T(numtriangles).nindices[1] = n < 0 ? n + numnormals : n;
                    fscanf(file, "%d/%d/%d", &v, &t, &n);
                    T(numtriangles).vindices[2] = v < 0 ? v + numvertices : v;
                    T(numtriangles).tindices[2] = t < 0 ? t + numtexcoords : t;
                    T(numtriangles).nindices[2] = n < 0 ? n + numnormals : n;
                    group->triangles[group->numtriangles++] = numtriangles;
                    numtriangles++;
                    while (fscanf(file, "%d/%d/%d", &v, &t, &n) > 0) {
                        T(numtriangles).vindices[0] = T(numtriangles - 1).vindices[0];
                        T(numtriangles).tindices[0] = T(numtriangles - 1).tindices[0];
                        T(numtriangles).nindices[0] = T(numtriangles - 1).nindices[0];
                        T(numtriangles).vindices[1] = T(numtriangles - 1).vindices[2];
                        T(numtriangles).tindices[1] = T(numtriangles - 1).tindices[2];
                        T(numtriangles).nindices[1] = T(numtriangles - 1).nindices[2];
                        T(numtriangles).vindices[2] = v < 0 ? v + numvertices : v;
                        T(numtriangles).tindices[2] = t < 0 ? t + numtexcoords : t;
                        T(numtriangles).nindices[2] = n < 0 ? n + numnormals : n;
                        group->triangles[group->numtriangles++] = numtriangles;
                        numtriangles++;
                    }
                }
                else if (sscanf(buf, "%d/%d", &v, &t) == 2) {
                    /* v/t */
                    T(numtriangles).vindices[0] = v < 0 ? v + numvertices : v;
                    T(numtriangles).tindices[0] = t < 0 ? t + numtexcoords : t;
                    fscanf(file, "%d/%d", &v, &t);
                    T(numtriangles).vindices[1] = v < 0 ? v + numvertices : v;
                    T(numtriangles).tindices[1] = t < 0 ? t + numtexcoords : t;
                    fscanf(file, "%d/%d", &v, &t);
                    T(numtriangles).vindices[2] = v < 0 ? v + numvertices : v;
                    T(numtriangles).tindices[2] = t < 0 ? t + numtexcoords : t;
                    group->triangles[group->numtriangles++] = numtriangles;
                    numtriangles++;
                    while (fscanf(file, "%d/%d", &v, &t) > 0) {
                        T(numtriangles).vindices[0] = T(numtriangles - 1).vindices[0];
                        T(numtriangles).tindices[0] = T(numtriangles - 1).tindices[0];
                        T(numtriangles).vindices[1] = T(numtriangles - 1).vindices[2];
                        T(numtriangles).tindices[1] = T(numtriangles - 1).tindices[2];
                        T(numtriangles).vindices[2] = v < 0 ? v + numvertices : v;
                        T(numtriangles).tindices[2] = t < 0 ? t + numtexcoords : t;
                        group->triangles[group->numtriangles++] = numtriangles;
                        numtriangles++;
                    }
                }
                else {
                    /* v */
                    sscanf(buf, "%d", &v);
                    T(numtriangles).vindices[0] = v < 0 ? v + numvertices : v;
                    fscanf(file, "%d", &v);
                    T(numtriangles).vindices[1] = v < 0 ? v + numvertices : v;
                    fscanf(file, "%d", &v);
                    T(numtriangles).vindices[2] = v < 0 ? v + numvertices : v;
                    group->triangles[group->numtriangles++] = numtriangles;
                    numtriangles++;
                    while (fscanf(file, "%d", &v) > 0) {
                        T(numtriangles).vindices[0] = T(numtriangles - 1).vindices[0];
                        T(numtriangles).vindices[1] = T(numtriangles - 1).vindices[2];
                        T(numtriangles).vindices[2] = v < 0 ? v + numvertices : v;
                        group->triangles[group->numtriangles++] = numtriangles;
                        numtriangles++;
                    }
                }
                break;

            default:
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
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

#endif
    }
}

// Add by Yaukey at 2019-03-01.
// YW model loader for obj file class.
// Modified version of glm.h
// Nate Robins, 1997, 2000
// nate@pobox.com, http://www.pobox.com/~nate
// Wavefront OBJ model file format reader/writer/manipulator.
// YW model loader for obj file class.

#include "YwModel.h"
#include "YwModelLoaderObj.h"

// warning C4996: 'strdup': The POSIX name for this item is deprecated. Instead, use the ISO C and C++ conformant name: _strdup. See online help for details.
#pragma warning(disable:4996)

namespace yw
{
    // Helper to read triangle in model.
    #define TRIANGLE(x) (model->m_Triangles[(x)])

    ModelLoaderObj::ModelLoaderObj() : 
        IModelLoader(),
        m_CalculateNormals(false),
        m_CalculateNormalAngle(90.0f)
    {
        
    }
    
    ModelLoaderObj::~ModelLoaderObj()
    {
        // Release each group.
    }
    
    Model* ModelLoaderObj::Load(const StringA& fileName)
    {
        // Alloc a model class.
        Model* objModel = new Model();
        
        // Open the file.
        FILE* objFile = fopen(fileName.c_str(), "rt");
        if (nullptr == objFile)
        {
            return nullptr;
        }
        
        // Make a first pass through the file to get a count of the number of vertices, normals, texcoords & triangles.
        FirstPass(objModel, objFile);
        
        /* rewind to beginning of file and read in the data this pass */
        rewind(objFile);
        
        // Second pass to organize data.
        SecondPass(objModel, objFile);

        // Calculate facet normals is necessary.
        CalculateFacetNormals(objModel);

        // Calculate normal if this obj file does not contains any normal.
        if (m_CalculateNormals || (objModel->m_Normals.size() <= 0))
        {
            CalculateVertexNormals(objModel, m_CalculateNormalAngle);
        }
        
        /* Close the file. */
        fclose(objFile);
        
        return objModel;
    }
    
    void ModelLoaderObj::FirstPass(Model* model, FILE* objFile)
    {
        uint32_t numVertices = 0;           /* number of vertices in model */
        uint32_t numNormals = 0;            /* number of normals in model */
        uint32_t numTexcoords = 0;          /* number of texcoords in model */
        uint32_t numTriangles = 0;          /* number of triangles in model */
        
        /* make a default group */
        ModelGroup* defaultGroup = new ModelGroup("default");
        model->m_Groups.push_back(defaultGroup);
        
        /* current group */
        ModelGroup* group = defaultGroup;
        int32_t v = 0;
        int32_t n = 0;
        int32_t t = 0;
        char buf[128];

        numVertices = numNormals = numTexcoords = numTriangles = 0;
        while (fscanf(objFile, "%s", buf) != EOF)
        {
            switch (buf[0]) 
            {
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
                model->m_MaterialName = strdup(buf);
                ReadMTL(model, buf);
                break;
            case 'u':
                /* eat up rest of line */
                fgets(buf, sizeof(buf), objFile);
                break;
            case 'g':               /* group */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), objFile);
#if MODEL_OBJ_LOAD_SINGLE_STRING_GROUP_NAMES
                sscanf(buf, "%s", buf);
#else
                buf[strlen(buf) - 1] = '\0';  /* nuke '\n' */
#endif
                group = new ModelGroup(buf);
                model->m_Groups.push_back(group);
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
                    model->m_Triangles.push_back(new ModelTriangle());
                    while (fscanf(objFile, "%d//%d", &v, &n) > 0)
                    {
                        numTriangles++;
                        model->m_Triangles.push_back(new ModelTriangle());
                    }
                }
                else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3)
                {
                    /* v/t/n */
                    fscanf(objFile, "%d/%d/%d", &v, &t, &n);
                    fscanf(objFile, "%d/%d/%d", &v, &t, &n);
                    numTriangles++;
                    model->m_Triangles.push_back(new ModelTriangle());
                    while (fscanf(objFile, "%d/%d/%d", &v, &t, &n) > 0)
                    {
                        numTriangles++;
                        model->m_Triangles.push_back(new ModelTriangle());
                    }
                }
                else if (sscanf(buf, "%d/%d", &v, &t) == 2)
                {
                    /* v/t */
                    fscanf(objFile, "%d/%d", &v, &t);
                    fscanf(objFile, "%d/%d", &v, &t);
                    numTriangles++;
                    model->m_Triangles.push_back(new ModelTriangle());
                    while (fscanf(objFile, "%d/%d", &v, &t) > 0)
                    {
                        numTriangles++;
                        model->m_Triangles.push_back(new ModelTriangle());
                    }
                }
                else 
                {
                    /* v */
                    fscanf(objFile, "%d", &v);
                    fscanf(objFile, "%d", &v);
                    numTriangles++;
                    model->m_Triangles.push_back(new ModelTriangle());
                    while (fscanf(objFile, "%d", &v) > 0)
                    {
                        numTriangles++;
                        model->m_Triangles.push_back(new ModelTriangle());
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
        model->m_Vertices.resize(numVertices);
        model->m_Normals.resize(numNormals);
        model->m_Texcoords.resize(numTexcoords);
        model->m_Texcoord2s.resize(numTexcoords);
        //model->m_Triangles.resize(numTriangles);
        assert((uint32_t)model->m_Triangles.size() == numTriangles);
    }
    
    void ModelLoaderObj::SecondPass(Model* model, FILE* objFile)
    {
        uint32_t numVertices = 0;        /* number of vertices in model */
        uint32_t numNormals = 0;         /* number of normals in model */
        uint32_t numTexcoords = 0;       /* number of texcoords in model */
        uint32_t numTriangles = 0;       /* number of triangles in model */

        // Used for parsing data.
        int32_t v = 0;
        int32_t n = 0;
        int32_t t = 0;
        char buf[128];

        /* set the pointer shortcuts */
        Vector3* vertices = &(model->m_Vertices[0]);         /* array of vertices  */
        Vector3* normals = &(model->m_Normals[0]);           /* array of normals */
        Vector2* texcoords = &(model->m_Texcoords[0]);       /* array of texture coordinates */
        ModelGroup*group = model->m_Groups.front();       /* current group pointer */
        void* material = nullptr;                           /* current material (Need to implement) */

        /* on the second pass through the file, read all the data into the
        allocated arrays */
        numVertices = numNormals = numTexcoords = 0;
        numTriangles = 0;
        while (fscanf(objFile, "%s", buf) != EOF) 
        {
            switch (buf[0]) 
            {
            case '#':               /* comment */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), objFile);
                break;
            case 'v':               /* v, vn, vt */
                switch (buf[1]) 
                {
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
                group = model->FindGroup(buf);
                group->m_Material = nullptr; // Need to implement material.
                //group = glmFindGroup(model, buf);
                //group->material = material;
                break;
            case 'f':               /* face */
                v = n = t = 0;
                fscanf(objFile, "%s", buf);
                /* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
                if (strstr(buf, "//")) 
                {
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
                    while (fscanf(objFile, "%d//%d", &v, &n) > 0) 
                    {
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
                else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) 
                {
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
                    while (fscanf(objFile, "%d/%d/%d", &v, &t, &n) > 0) 
                    {
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
                else if (sscanf(buf, "%d/%d", &v, &t) == 2) 
                {
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
                else 
                {
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
        /* Announce the memory requirements. (Minimum required) */
        printf("Obj Model Memory: %d bytes\n",
            numVertices * 3 * sizeof(Vector3) +
            numNormals * 3 * sizeof(Vector3) * (numNormals ? 1 : 0) +
            numTexcoords * 3 * sizeof(Vector2) * (numTexcoords ? 1 : 0) +
            numTriangles * sizeof(ModelTriangle));
#endif
    }

    void ModelLoaderObj::CalculateFacetNormals(Model* model)
    {
        assert(nullptr != model);
        assert(model->m_Vertices.size() > 0);

        /* clobber any old facet normals */
        model->m_FacetNormals.clear();

        /* allocate memory for the new facet normals */
        model->m_FacetNormals.resize(model->m_Triangles.size());
        for (uint32_t i = 0; i < (uint32_t)model->m_Triangles.size(); i++)
        {
            model->m_Triangles[i]->m_FacetNormalIndex = i;

            Vector3 u = model->m_Vertices[TRIANGLE(i)->m_VertexIndices[1]] - model->m_Vertices[TRIANGLE(i)->m_VertexIndices[0]];
            Vector3 v = model->m_Vertices[TRIANGLE(i)->m_VertexIndices[2]] - model->m_Vertices[TRIANGLE(i)->m_VertexIndices[0]];

            Vector3Cross(model->m_FacetNormals[i], u, v);
            Vector3Normalize(model->m_FacetNormals[i], model->m_FacetNormals[i]);
        }
    }

    void ModelLoaderObj::CalculateVertexNormals(Model* model, float angle)
    {
        assert(nullptr != model);
        assert(model->m_FacetNormals.size() > 0);

        /* calculate the cosine of the angle (in degrees) */
        float cos_angle = (float)cos(angle * YW_PI / 180.0);

        /* nuke any previous normals */
        model->m_Normals.clear();

        /* allocate space for new normals */
        std::vector<Vector3> normals(model->m_Triangles.size() * 3);

        /* allocate a structure that will hold a linked list of triangle indices for each vertex */
        std::vector<Node*> members(model->m_Vertices.size(), nullptr);

        /* for every triangle, create a node for each vertex in it */
        for (uint32_t i = 0; i < (uint32_t)model->m_Triangles.size(); i++)
        {
            Node* node = new Node();
            node->m_Index = i;
            node->m_Next = members[TRIANGLE(i)->m_VertexIndices[0]];
            members[TRIANGLE(i)->m_VertexIndices[0]] = node;

            node = new Node();
            node->m_Index = i;
            node->m_Next = members[TRIANGLE(i)->m_VertexIndices[1]];
            members[TRIANGLE(i)->m_VertexIndices[1]] = node;

            node = new Node();
            node->m_Index = i;
            node->m_Next = members[TRIANGLE(i)->m_VertexIndices[2]];
            members[TRIANGLE(i)->m_VertexIndices[2]] = node;
        }

        /* calculate the average normal for each vertex */
        uint32_t numNormals = 0;
        for (uint32_t i = 0; i <= (uint32_t)model->m_Vertices.size(); i++)
        {
            /* calculate an average normal for this vertex by averaging the
                facet normal of every triangle this vertex is in */
            Node* node = members[i];
            if (nullptr == node)
            {
                LOGE(_T("CalculateVertexNormals(): vertex w/o a triangle\n"));
                return;
            }

            Vector3 average;
            uint32_t avg = 0;
            while (nullptr != node)
            {
                /* only average if the dot product of the angle between the two
                facet normals is greater than the cosine of the threshold
                angle -- or, said another way, the angle between the two
                    facet normals is less than (or equal to) the threshold angle */
                float dot = Vector3Dot(model->m_FacetNormals[TRIANGLE(node->m_Index)->m_FacetNormalIndex],
                    model->m_FacetNormals[TRIANGLE(members[i]->m_Index)->m_FacetNormalIndex]);
                if (dot > cos_angle)
                {
                    node->m_Averaged = true;
                    average += model->m_FacetNormals[TRIANGLE(node->m_Index)->m_FacetNormalIndex];
                    avg = 1;            /* we averaged at least one normal! */
                }
                else 
                {
                    node->m_Averaged = false;
                }

                node = node->m_Next;
            }

            if (avg)
            {
                /* normalize the averaged normal */
                Vector3Normalize(average, average);

                /* add the normal to the vertex normals list */
                normals[numNormals] = average;
                avg = numNormals;
                numNormals++;
            }

            /* set the normal of this vertex in each triangle it is in */
            node = members[i];
            while (node)
            {
                if (node->m_Averaged)
                {
                    /* if this node was averaged, use the average normal */
                    if (TRIANGLE(node->m_Index)->m_VertexIndices[0] == i)
                    {
                        TRIANGLE(node->m_Index)->m_NormalIndices[0] = avg;
                    }
                    else if (TRIANGLE(node->m_Index)->m_VertexIndices[1] == i)
                    {
                        TRIANGLE(node->m_Index)->m_NormalIndices[1] = avg;
                    }
                    else if (TRIANGLE(node->m_Index)->m_VertexIndices[2] == i)
                    {
                        TRIANGLE(node->m_Index)->m_NormalIndices[2] = avg;
                    }
                }
                else
                {
                    /* if this node wasn't averaged, use the facet normal */
                    normals[numNormals] = model->m_FacetNormals[TRIANGLE(node->m_Index)->m_FacetNormalIndex];
                    if (TRIANGLE(node->m_Index)->m_VertexIndices[0] == i)
                    {
                        TRIANGLE(node->m_Index)->m_NormalIndices[0] = numNormals;
                    }
                    else if (TRIANGLE(node->m_Index)->m_VertexIndices[1] == i)
                    {
                        TRIANGLE(node->m_Index)->m_NormalIndices[1] = numNormals;
                    }
                    else if (TRIANGLE(node->m_Index)->m_VertexIndices[2] == i)
                    {
                        TRIANGLE(node->m_Index)->m_NormalIndices[2] = numNormals;
                    }

                    numNormals++;
                }

                node = node->m_Next;
            }
        }

        //model->numNormals = numNormals - 1;

        /* free the member information */
        for (uint32_t i = 0; i <= (uint32_t)members.size(); i++)
        {
            Node* node = members[i];
            while (nullptr != node)
            {
                Node* tail = node;
                node = node->m_Next;
                YW_SAFE_DELETE(tail);
            }
        }

        members.clear();

        /* pack the normals array (we previously allocated the maximum
        number of normals that could possibly be created (numtriangles *
        3), so get rid of some of them (usually alot unless none of the
        facet normals were averaged)) */
        model->m_Normals.resize(numNormals);
        for (uint32_t i = 1; i <= numNormals; i++)
        {
            model->m_Normals[i] = normals[i];
        }

        normals.clear();
    }

    void ModelLoaderObj::ReadMTL(Model* model, StringA name)
    {

    }
}

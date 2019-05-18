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

    ModelLoaderObj::ModelLoaderObj(bool calculateNormals, float calculateNormalAngle) :
        IModelLoader(),
        m_CalculateNormals(calculateNormals),
        m_CalculateNormalAngle(calculateNormalAngle)
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
        
        // Read data from file.
        const char* objData = ReadDataFromFile(fileName);
        if (nullptr == objData)
        {
            return nullptr;
        }
        
        // Try to load obj model from data.
        LoadWavefrontObjFormData(objModel, objData, m_CalculateNormals, m_CalculateNormalAngle);
        
        // Release file data.
        YW_SAFE_DELETE_ARRAY(objData);
        
        return objModel;
    }

    const char* ModelLoaderObj::ReadDataFromFile(const StringA& fileName)
    {
        // Open the file.
        FILE* objFile = fopen(fileName.c_str(), "rt");
        if (nullptr == objFile)
        {
            return nullptr;
        }

        // Get file size.
        fseek(objFile, 0, SEEK_END);
        uint32_t fileSize = ftell(objFile);

        // Rewind to the begin.
        rewind(objFile);

        // Read data.
        char* objData = new char[fileSize + 1];
        fileSize = (uint32_t)fread(objData, 1, fileSize, objFile);

        // Close file.
        fclose(objFile);

        // Process data.
        objData[fileSize] = 0;

#if !(defined(_WIN32) || defined(WIN32))
        {
            char* temp = new char[fileSize + 1];
            strcpy(temp, objData);

            char* src = temp;
            char* dest = objData;
            fileSize = 0;
            while (*src)
            {
                if ('\r' != *src)
                {
                    *dest++ = *src;
                    ++fileSize;
                }

                ++src;
            }

            *dest = 0;

            // Release temp data.
            YW_SAFE_DELETE_ARRAY(temp);
        }
#endif

        return objData;
    }

    void ModelLoaderObj::LoadWavefrontObjFormData(Model* objModel, const char* objData, bool calculateNormals, float calculateNormalAngle)
    {
        // Make a first pass through the file to get a count of the number of vertices, normals, texcoords & triangles.
        FirstPass(objModel, objData);

        // Second pass to organize data.
        SecondPass(objModel, objData);

        // Normalize model mesh indices from 1-based to 0-based.
        NormalizeIndices(objModel);

        // Calculate facet normals is necessary.
        CalculateFacetNormals(objModel);

        // Calculate normal if this obj file does not contains any normal.
        if (calculateNormals || (objModel->m_Normals.size() <= 0))
        {
            CalculateVertexNormals(objModel, calculateNormalAngle);
        }

        // Calculate facet and vertex tangents.
        CalculateVertexTangent(objModel);
    }

    void ModelLoaderObj::FirstPass(Model* model, const char* objData)
    {
        uint32_t numVertices = 0;           /* number of vertices in model */
        uint32_t numNormals = 0;            /* number of normals in model */
        uint32_t numTexcoords = 0;          /* number of texcoords in model */
        uint32_t numTriangles = 0;          /* number of triangles in model */

        /* current group */
        ModelGroup* group = nullptr;

        // Used for parsing face data.
        int32_t v = 0;
        int32_t n = 0;
        int32_t t = 0;

        // Used for reading temp buffer.
        char buf[128];
        memset(buf, 0, sizeof(buf));

        // Begin to parse data.
        const char* curPos = objData;
        while (0 != *curPos)
        {
            switch (*curPos)
            {
            case '#':               /* comment */
                /* eat up rest of line */
                break;
            case 'v':               /* v, vn, vt */
                curPos++;
                switch (*curPos) 
                {
                case ' ':          /* vertex */
                    /* eat up rest of line */
                    numVertices++;
                    break;
                case 'n':           /* normal */
                    /* eat up rest of line */
                    numNormals++;
                    break;
                case 't':           /* texcoord */
                    /* eat up rest of line */
                    numTexcoords++;
                    break;
                default:
                    LOGE_A(_T("FirstPass(): Unknown token \"%s\".\n", buf));
                    exit(1);
                    break;
                }
                break;
            case 'm':
                sscanf(curPos, "%s %s", buf, buf);
                model->m_MaterialName = buf;
                ReadMTL(model, buf);
                break;
            case 'u':
                /* eat up rest of line */
                break;
            case 'g':               /* group */
                /* eat up rest of line */
                sscanf(curPos, "%s %s", buf, buf);
                group = new ModelGroup(buf);
                model->m_Groups.push_back(group);
                break;
            case 'f':               /* face */
                curPos += 2; // Move to face start position, skipping the white space.
                
                /* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
                sscanf(curPos, "%s", buf);
                if (strstr(buf, "//"))
                {
                    /* v//n */
                    numTriangles++;
                    model->m_Triangles.push_back(new ModelTriangle());
                }
                else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3)
                {
                    /* v/t/n */
                    numTriangles++;
                    model->m_Triangles.push_back(new ModelTriangle());
                }
                else if (sscanf(buf, "%d/%d", &v, &t) == 2)
                {
                    /* v/t */
                    numTriangles++;
                    model->m_Triangles.push_back(new ModelTriangle());
                }
                else
                {
                    /* v */
                    numTriangles++;
                    model->m_Triangles.push_back(new ModelTriangle());
                }
                break;
            default:
                /* eat up rest of line */
                break;
            }

            // Advance to next line.
            while (0 != *curPos)
            {
                if ('\n' == *curPos++)
                {
                    break;
                }
            }
        }

        /* set the stats in the model structure */
        model->m_Vertices.resize(numVertices);
        model->m_Normals.resize(numNormals);
        model->m_Texcoords.resize(numTexcoords);
        model->m_Texcoord2s.resize(numTexcoords);
        //model->m_Triangles.resize(numTriangles);
        assert((uint32_t)model->m_Triangles.size() == numTriangles);
        model->m_Tangents.resize(numVertices);

        // Create a default group if no group found.
        if (model->m_Groups.size() <= 0)
        {
            /* make a default group */
            ModelGroup* defaultGroup = new ModelGroup("default");
            model->m_Groups.push_back(defaultGroup);
        }
    }

    void ModelLoaderObj::SecondPass(class Model* model, const char* objData)
    {
        uint32_t numVertices = 0;        /* number of vertices in model */
        uint32_t numNormals = 0;         /* number of normals in model */
        uint32_t numTexcoords = 0;       /* number of texcoords in model */
        uint32_t numTriangles = 0;       /* number of triangles in model */

        // Used for reading temp buffer.
        char buf[128];
        memset(buf, 0, sizeof(buf));

        // Used for parsing face data.
        int32_t v[3] = { 0, 0, 0 };
        int32_t n[3] = { 0, 0, 0 };
        int32_t t[3] = { 0, 0, 0 };

        /* set the pointer shortcuts */
        Vector3* vertices = &(model->m_Vertices[0]);        /* array of vertices  */
        Vector3* normals = &(model->m_Normals[0]);          /* array of normals */
        Vector2* texcoords = &(model->m_Texcoords[0]);      /* array of texture coordinates */
        ModelGroup* group = model->m_Groups.front();        /* current group pointer */
        void* material = nullptr;                           /* current material (Need to implement) */

        /* on the second pass through the file, read all the data into the
        allocated arrays */
        const char* curPos = objData;
        while (0 != *curPos)
        {
            switch (*curPos)
            {
            case '#':               /* comment */
                /* eat up rest of line */
                break;
            case 'v':               /* v, vn, vt */
                curPos++;
                switch (*curPos++)
                {
                case ' ':          /* vertex */
                    sscanf(curPos, "%f %f %f",
                        &(vertices[numVertices].x),
                        &(vertices[numVertices].y),
                        &(vertices[numVertices].z));
                    numVertices++;
                    break;
                case 'n':           /* normal */
                    sscanf(curPos, "%f %f %f",
                        &(normals[numNormals].x),
                        &(normals[numNormals].y),
                        &(normals[numNormals].z));
                    numNormals++;
                    break;
                case 't':           /* texcoord */
                    sscanf(curPos, "%f %f",
                        &(texcoords[numTexcoords].x),
                        &(texcoords[numTexcoords].y));
                    numTexcoords++;
                    break;
                }
                break;
            case 'u':
                sscanf(curPos, "%s %s", buf, buf);
                material = nullptr; // Find material.
                group->m_Material = nullptr; // Need to implement material. Material is 'buf'.
                //group->material = material = glmFindMaterial(model, buf);
                break;
            case 'g':               /* group */
                /* eat up rest of line */
                sscanf(curPos, "%s %s", buf, buf);
                group = model->FindGroup(buf);
                group->m_Material = material; // Need to implement material.
                //group = glmFindGroup(model, buf);
                //group->material = material;
                break;
            case 'f':               /* face */
                curPos += 2; // Move to face start position, skipping the white space.
                
                /* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
                sscanf(curPos, "%s", buf);
                if (strstr(buf, "//"))
                {
                    /* v//n */
                    sscanf(curPos, "%d//%d %d//%d %d//%d", &v[0], &n[0], &v[1], &n[1], &v[2], &n[2]);

                    TRIANGLE(numTriangles)->m_VertexIndices[0] = v[0] < 0 ? v[0] + numVertices : v[0];
                    TRIANGLE(numTriangles)->m_NormalIndices[0] = n[0] < 0 ? n[0] + numNormals : n[0];

                    TRIANGLE(numTriangles)->m_VertexIndices[1] = v[1] < 0 ? v[1] + numVertices : v[1];
                    TRIANGLE(numTriangles)->m_NormalIndices[1] = n[1] < 0 ? n[1] + numNormals : n[1];

                    TRIANGLE(numTriangles)->m_VertexIndices[2] = v[2] < 0 ? v[2] + numVertices : v[2];
                    TRIANGLE(numTriangles)->m_NormalIndices[2] = n[2] < 0 ? n[2] + numNormals : n[2];

                    group->m_Triangles.push_back(numTriangles);
                    numTriangles++;
                }
                else if (sscanf(curPos, "%d/%d/%d %d/%d/%d %d/%d/%d", &v[0], &t[0], &n[0], &v[1], &t[1], &n[1], &v[2], &t[2], &n[2]) == 9)
                {
                    /* v/t/n */
                    TRIANGLE(numTriangles)->m_VertexIndices[0] = v[0] < 0 ? v[0] + numVertices : v[0];
                    TRIANGLE(numTriangles)->m_TexcoordsIndices[0] = t[0] < 0 ? t[0] + numTexcoords : t[0];
                    TRIANGLE(numTriangles)->m_NormalIndices[0] = n[0] < 0 ? n[0] + numNormals : n[0];

                    TRIANGLE(numTriangles)->m_VertexIndices[1] = v[1] < 0 ? v[1] + numVertices : v[1];
                    TRIANGLE(numTriangles)->m_TexcoordsIndices[1] = t[1] < 0 ? t[1] + numTexcoords : t[1];
                    TRIANGLE(numTriangles)->m_NormalIndices[1] = n[1] < 0 ? n[1] + numNormals : n[1];

                    TRIANGLE(numTriangles)->m_VertexIndices[2] = v[2] < 0 ? v[2] + numVertices : v[2];
                    TRIANGLE(numTriangles)->m_TexcoordsIndices[2] = t[2] < 0 ? t[2] + numTexcoords : t[2];
                    TRIANGLE(numTriangles)->m_NormalIndices[2] = n[2] < 0 ? n[2] + numNormals : n[2];

                    group->m_Triangles.push_back(numTriangles);
                    numTriangles++;
                }
                else if (sscanf(curPos, "%d/%d %d/%d %d/%d", &v[0], &t[0], &v[1], &t[1], &v[2], &t[2]) == 6)
                {
                    /* v/t */
                    TRIANGLE(numTriangles)->m_VertexIndices[0] = v[0] < 0 ? v[0] + numVertices : v[0];
                    TRIANGLE(numTriangles)->m_TexcoordsIndices[0] = t[0] < 0 ? t[0] + numTexcoords : t[0];

                    TRIANGLE(numTriangles)->m_VertexIndices[1] = v[1] < 0 ? v[1] + numVertices : v[1];
                    TRIANGLE(numTriangles)->m_TexcoordsIndices[1] = t[1] < 0 ? t[1] + numTexcoords : t[1];

                    TRIANGLE(numTriangles)->m_VertexIndices[2] = v[2] < 0 ? v[2] + numVertices : v[2];
                    TRIANGLE(numTriangles)->m_TexcoordsIndices[2] = t[2] < 0 ? t[2] + numTexcoords : t[2];

                    group->m_Triangles.push_back(numTriangles);
                    numTriangles++;
                }
                else
                {
                    /* v */
                    sscanf(curPos, "%d %d %d", &v[0], &v[1], &v[2]);
                    TRIANGLE(numTriangles)->m_VertexIndices[0] = v[0] < 0 ? v[0] + numVertices : v[0];
                    TRIANGLE(numTriangles)->m_VertexIndices[1] = v[1] < 0 ? v[1] + numVertices : v[1];
                    TRIANGLE(numTriangles)->m_VertexIndices[2] = v[2] < 0 ? v[2] + numVertices : v[2];
                    group->m_Triangles.push_back(numTriangles);
                    numTriangles++;
                }
                break;
            default:
                /* eat up rest of line */
                break;
            }

            /* eat up rest of line */
            // Advance to next line.
            while (0 != *curPos)
            {
                if ('\n' == *curPos++)
                {
                    break;
                }
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

    void ModelLoaderObj::NormalizeIndices(class Model* model)
    {
        for (int32_t i = 0; i < (int32_t)model->m_Triangles.size(); i++)
        {
            ModelTriangle* triangle = model->m_Triangles[i];

            triangle->m_VertexIndices[0] -= 1;
            triangle->m_VertexIndices[1] -= 1;
            triangle->m_VertexIndices[2] -= 1;

            triangle->m_NormalIndices[0] -= 1;
            triangle->m_NormalIndices[1] -= 1;
            triangle->m_NormalIndices[2] -= 1;

            triangle->m_TexcoordsIndices[0] -= 1;
            triangle->m_TexcoordsIndices[1] -= 1;
            triangle->m_TexcoordsIndices[2] -= 1;

            triangle->m_Texcoords2Indices[0] -= 1;
            triangle->m_Texcoords2Indices[1] -= 1;
            triangle->m_Texcoords2Indices[2] -= 1;
        }
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

            Vector3& facetNormal = model->m_FacetNormals[i];
            Vector3Cross(facetNormal, u, v);
            Vector3Normalize(facetNormal, facetNormal);
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
        for (uint32_t i = 0; i < (uint32_t)model->m_Vertices.size(); i++)
        {
            /* calculate an average normal for this vertex by averaging the
                facet normal of every triangle this vertex is in */
            Node* node = members[i];
            if (nullptr == node)
            {
                LOGE(_T("CalculateVertexNormals(): vertex w/o a triangle\n"));
                assert(false);

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
        for (uint32_t i = 0; i < (uint32_t)model->m_Vertices.size(); i++)
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
        for (uint32_t i = 0; i < numNormals; i++)
        {
            model->m_Normals[i] = normals[i];
        }

        normals.clear();
    }

    void ModelLoaderObj::CalculateVertexTangent(class Model* model)
    {
        // Calculate triangle tangent vector.
        // TODO: average tangents of shared vertices.
        for (int32_t i = 0; i < (int32_t)model->m_Triangles.size(); i++)
        {
            const ModelTriangle* triangle = TRIANGLE(i);

            const Vector3& v0 = model->m_Vertices[triangle->m_VertexIndices[0]];
            const Vector3& v1 = model->m_Vertices[triangle->m_VertexIndices[1]];
            const Vector3& v2 = model->m_Vertices[triangle->m_VertexIndices[2]];

            const float texCoordY0 = model->m_Texcoords[triangle->m_TexcoordsIndices[0]].y;
            const float texCoordY1 = model->m_Texcoords[triangle->m_TexcoordsIndices[1]].y;
            const float texCoordY2 = model->m_Texcoords[triangle->m_TexcoordsIndices[2]].y;

            const Vector3 deltaVertex[2] = { v1 - v0, v2 - v0 };
            const float deltaTexCoordY[2] = { texCoordY1 - texCoordY0, texCoordY2 - texCoordY0 };
            const Vector3 tangent = (deltaVertex[0] * deltaTexCoordY[1] - deltaVertex[1] * deltaTexCoordY[0]).Normalize();
            
            model->m_Tangents[triangle->m_VertexIndices[0]] = tangent;
            model->m_Tangents[triangle->m_VertexIndices[1]] = tangent;
            model->m_Tangents[triangle->m_VertexIndices[2]] = tangent;
        }
    }

    void ModelLoaderObj::ReadMTL(Model* model, StringA name)
    {

    }
}

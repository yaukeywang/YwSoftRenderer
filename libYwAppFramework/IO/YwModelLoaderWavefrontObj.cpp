// Add by Yaukey at 2019-03-01.
// YW model loader for obj file class.
// Modified version of glm.h
// Nate Robins, 1997, 2000
// nate@pobox.com, http://www.pobox.com/~nate
// Wavefront OBJ model file format reader/writer/manipulator.
// YW model loader for obj file class.

#include "YwModel.h"
#include "YwModelLoaderWavefrontObj.h"
#include "YwFileIO.h"
#include <unordered_map>

// warning C4996: 'strdup': The POSIX name for this item is deprecated. Instead, use the ISO C and C++ conformant name: _strdup. See online help for details.
//#pragma warning(disable:4996)

namespace yw
{
    // Helper to read triangle in model.
    #define TRIANGLE(x) (model->m_Triangles[(x)])

    // Scanning a string with sscanf and moving ahead with skipping white-space, stopped when empty or '\n' is found.
    const char* sscanf_string_and_go_ahead(const char* buffer, const char* format, const char* destination)
    {
        if (strlen(buffer) == 0)
        {
            return nullptr;
        }

        while (' ' == *buffer)
        {
            buffer++;
            if (('\0' == *buffer) || ('\n' == *buffer))
            {
                return nullptr;
            }
        }

        if ('\n' == *buffer)
        {
            return nullptr;
        }

        int32_t result = sscanf(buffer, format, destination);
        if (result <= 0)
        {
            return nullptr;
        }

        buffer += strlen(destination);
        while (' ' == *buffer)
        {
            buffer++;
            if (('\0' == *buffer) || ('\n' == *buffer))
            {
                break;
            }
        }

        if ('\n' == *buffer)
        {
            return nullptr;
        }

        return buffer;
    }

    // Hash function of ModelVertexAttributeIndex.
    struct ModelVertexAttributeIndexHashFunc
    {
        uint64_t hash_64(const ModelVertexAttributeIndex& o) const
        {
            uint64_t result = 0;

            // Only these attributes valid for obj model.
            result |= ((uint64_t)(o.positionIndex & 0x0000ffff) << 48);
            result |= ((uint64_t)(o.normalIndex & 0x0000ffff) << 32);
            result |= ((uint64_t)(o.tangentIndex & 0x0000ffff) << 16);
            result |= (uint64_t)(o.texcoordIndex & 0x0000ffff);

            return result;
        }

        uint32_t hash_32(const ModelVertexAttributeIndex& o) const
        {
            uint32_t result = 0;

            result |= ((uint32_t)(o.positionIndex & 0x0000ffff) << 16); // Very important.
            result |= (uint32_t)(o.texcoordIndex & 0x0000ffff);         // Mostly important.
            result += o.normalIndex * 1000;                             // Very important, but mostly same with position count.
            result += o.tangentIndex * 100;                             // Very important, but mostly same with position count.

            return result;
        }

        size_t operator()(const ModelVertexAttributeIndex& o) const
        {
            size_t result = 0;

        #if defined(_WIN32) || defined(WIN32)
            // Windows.
            #if defined(_WIN64) /* defined(_WIN64) */
                return hash_64(o);
            #else
                return hash_32(o);
            #endif
        #elif defined(LINUX_X11) || defined(_LINUX)
            // Linux
            #if defined(__LP64__) /* defined(__LP64__) */
                return hash_64(o);
            #else
                return hash_32(o);
            #endif
        #elif defined(_MAC_OSX)
            // OSX
            #if defined(__LP64__) /* defined(__LP64__) */
                return hash_64(o);
            #else
                return hash_32(o);
            #endif
        #elif defined(__amigaos4__) || defined(_AMIGAOS4)
            // OSX
            #if defined(__LP64__) /* defined(__LP64__) */
                return hash_64(o);
            #else
                return hash_32(o);
            #endif
        #endif

            return result;
        }
    };

    // Compare function of ModelVertexAttributeIndex.
    struct ModelVertexAttributeIndexCompareFunc
    {
        bool operator()(const ModelVertexAttributeIndex&l, const ModelVertexAttributeIndex& r) const
        {
            // Only these attributes valid for obj model.
            return (
                (l.positionIndex == r.positionIndex) &&
                (l.normalIndex == r.normalIndex) &&
                (l.tangentIndex == r.tangentIndex) &&
                (l.texcoordIndex == r.texcoordIndex)
                );
        }
    };

    // Process vertex attribute cache.
    uint32_t add_vertex_attribute(
        std::vector<ModelVertexAttributeIndex>* verticesCache, 
        std::unordered_map<ModelVertexAttributeIndex, uint32_t, ModelVertexAttributeIndexHashFunc, ModelVertexAttributeIndexCompareFunc>* verticesAttributes,
        ModelVertexAttributeIndex* vertex)
    {
        std::unordered_map<ModelVertexAttributeIndex, uint32_t>::const_iterator itFind = verticesAttributes->find(*vertex);
        if (verticesAttributes->end() == itFind)
        {
            uint32_t vertexIndex = (uint32_t)verticesCache->size();
            verticesCache->push_back(*vertex);
            verticesAttributes->insert(std::pair<ModelVertexAttributeIndex, uint32_t>(*vertex, vertexIndex));

            return vertexIndex;
        }
        else
        {
            return itFind->second;
        }
    }

    ModelLoaderWavefrontObj::ModelLoaderWavefrontObj() : 
        IModelLoader()
    {
        
    }
    
    ModelLoaderWavefrontObj::~ModelLoaderWavefrontObj()
    {
        // Release each group.
    }

    bool ModelLoaderWavefrontObj::LoadFormData(const uint8_t* data, bool calculateNormals, float calculateNormalAngle, Model* model)
    {
        LoadWavefrontObjFromData(model, data, calculateNormals, calculateNormalAngle);
        return true;
    }

    void ModelLoaderWavefrontObj::LoadWavefrontObjFromData(Model* objModel, const uint8_t* objData, bool calculateNormals, float calculateNormalAngle)
    {
        // Make a first pass through the file to get a count of the number of vertices, normals, texcoords & triangles.
        FirstPass(objModel, (const char*)objData);

        // Second pass to organize data.
        SecondPass(objModel, (const char*)objData);

        // Calculate facet normals is necessary.
        CalculateFacetNormals(objModel);

        // Calculate normal if this obj file does not contains any normal.
        if (calculateNormals || (objModel->m_Normals.size() <= 0))
        {
            CalculateVertexNormals(objModel, calculateNormalAngle);
        }

        // Calculate facet and vertex tangents.
        //CalculateVertexTangent(objModel);
        CalculateVertexTangentTBN(objModel);

        // Process secondary texture coordinates.
        ProcessOtherData(objModel);
    }

    void ModelLoaderWavefrontObj::FirstPass(Model* model, const char* objData)
    {
        uint32_t numPositions = 0;           /* number of vertices in model */
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
                    numPositions++;
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
                group = model->AddGroup(buf);
                break;
            case 'f':               /* face */
                curPos += 2; // Move to face start position, skipping the white space.
                
                /* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
                sscanf(curPos, "%s", buf);
                if (strstr(buf, "//"))
                {
                    /* v//n */
                    const char* movingAhead = curPos;
                    movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                    sscanf(buf, "%d//%d", &v, &n);

                    movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                    sscanf(buf, "%d//%d", &v, &n);

                    movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                    sscanf(buf, "%d//%d", &v, &n);

                    numTriangles++;
                    model->m_Triangles.push_back(new ModelTriangle());

                    while (nullptr != movingAhead)
                    {
                        movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                        sscanf(buf, "%d//%d", &v, &n);

                        numTriangles++;
                        model->m_Triangles.push_back(new ModelTriangle());
                    }
                }
                else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3)
                {
                    /* v/t/n */
                    const char* movingAhead = curPos;
                    movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                    sscanf(buf, "%d/%d/%d", &v, &t, &n);

                    movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                    sscanf(buf, "%d/%d/%d", &v, &t, &n);

                    movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                    sscanf(buf, "%d/%d/%d", &v, &t, &n);

                    numTriangles++;
                    model->m_Triangles.push_back(new ModelTriangle());

                    while (nullptr != movingAhead)
                    {
                        movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                        sscanf(buf, "%d/%d/%d", &v, &t, &n);

                        numTriangles++;
                        model->m_Triangles.push_back(new ModelTriangle());
                    }
                }
                else if (sscanf(buf, "%d/%d", &v, &t) == 2)
                {
                    /* v/t */
                    const char* movingAhead = curPos;
                    movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                    sscanf(buf, "%d/%d", &v, &t);

                    movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                    sscanf(buf, "%d/%d", &v, &t);

                    movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                    sscanf(buf, "%d/%d", &v, &t);

                    numTriangles++;
                    model->m_Triangles.push_back(new ModelTriangle());

                    while (nullptr != movingAhead)
                    {
                        movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                        sscanf(buf, "%d/%d", &v, &t);

                        numTriangles++;
                        model->m_Triangles.push_back(new ModelTriangle());
                    }
                }
                else
                {
                    /* v */
                    const char* movingAhead = curPos;
                    movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                    sscanf(buf, "%d", &v);

                    movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                    sscanf(buf, "%d", &v);

                    movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                    sscanf(buf, "%d", &v);

                    numTriangles++;
                    model->m_Triangles.push_back(new ModelTriangle());

                    while (nullptr != movingAhead)
                    {
                        movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                        sscanf(buf, "%d", &v);

                        numTriangles++;
                        model->m_Triangles.push_back(new ModelTriangle());
                    }
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
        model->m_Positions.resize(numPositions);
        model->m_Normals.resize(numNormals);
        model->m_Texcoords.resize(numTexcoords);
        model->m_Texcoord2s.resize(numTexcoords);
        //model->m_Triangles.resize(numTriangles);
        assert((uint32_t)model->m_Triangles.size() == numTriangles);

        // Create a default group if no group found.
        if (model->m_Groups.size() <= 0)
        {
            /* make a default group */
            ModelGroup* defaultGroup = new ModelGroup("default");
            model->m_Groups.push_back(defaultGroup);
        }
    }

    void ModelLoaderWavefrontObj::SecondPass(class Model* model, const char* objData)
    {
        uint32_t numPositions = 0;      /* number of positions in model */
        uint32_t numNormals = 0;        /* number of normals in model */
        uint32_t numTexcoords = 0;      /* number of texcoords in model */
        uint32_t numTriangles = 0;      /* number of triangles in model */
        uint32_t numVertices = 0;       /* number of vertices in model */

        // Used for reading temp buffer.
        char buf[128];
        memset(buf, 0, sizeof(buf));

        // Used for parsing face data.
        int32_t v = 0;
        int32_t n = 0;
        int32_t t = 0;

        /* set the pointer shortcuts */
        Vector3* vertices = &(model->m_Positions[0]);        /* array of vertices  */
        Vector3* normals = &(model->m_Normals[0]);          /* array of normals */
        Vector2* texcoords = &(model->m_Texcoords[0]);      /* array of texture coordinates */
        ModelGroup* group = model->m_Groups.front();        /* current group pointer */
        void* material = nullptr;                           /* current material (Need to implement) */

        // All triangle vertex attributes cache.
        std::unordered_map<ModelVertexAttributeIndex, uint32_t, ModelVertexAttributeIndexHashFunc, ModelVertexAttributeIndexCompareFunc> verticesAttributeIndices;

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
                        &(vertices[numPositions].x),
                        &(vertices[numPositions].y),
                        &(vertices[numPositions].z));
                    numPositions++;
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
                    texcoords[numTexcoords].y = 1.0f - texcoords[numTexcoords].y;
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
                    const char* movingAhead = curPos;
                    movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                    sscanf(buf, "%d//%d", &v, &n);
                    v = (v < 0 ? v + numPositions : v) - 1;
                    n = (n < 0 ? n + numNormals : n) - 1;
                    TRIANGLE(numTriangles)->m_PositionIndices[0] = v;
                    TRIANGLE(numTriangles)->m_NormalIndices[0] = n;
                    TRIANGLE(numTriangles)->m_VertexAttributeIndices[0] = add_vertex_attribute(&model->m_Vertices, &verticesAttributeIndices, &ModelVertexAttributeIndex(v, n));

                    movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                    sscanf(buf, "%d//%d", &v, &n);
                    v = (v < 0 ? v + numPositions : v) - 1;
                    n = (n < 0 ? n + numNormals : n) - 1;
                    TRIANGLE(numTriangles)->m_PositionIndices[1] = v;
                    TRIANGLE(numTriangles)->m_NormalIndices[1] = n;
                    TRIANGLE(numTriangles)->m_VertexAttributeIndices[1] = add_vertex_attribute(&model->m_Vertices, &verticesAttributeIndices, &ModelVertexAttributeIndex(v, n));

                    movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                    sscanf(buf, "%d//%d", &v, &n);
                    v = (v < 0 ? v + numPositions : v) - 1;
                    n = (n < 0 ? n + numNormals : n) - 1;
                    TRIANGLE(numTriangles)->m_PositionIndices[2] = v;
                    TRIANGLE(numTriangles)->m_NormalIndices[2] = n;
                    TRIANGLE(numTriangles)->m_VertexAttributeIndices[2] = add_vertex_attribute(&model->m_Vertices, &verticesAttributeIndices, &ModelVertexAttributeIndex(v, n));

                    group->m_Triangles.push_back(numTriangles);
                    numTriangles++;

                    while (nullptr != movingAhead)
                    {
                        movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                        sscanf(buf, "%d//%d", &v, &n);
                        v = (v < 0 ? v + numPositions : v) - 1;
                        n = (n < 0 ? n + numNormals : n) - 1;

                        TRIANGLE(numTriangles)->m_PositionIndices[0] = TRIANGLE(numTriangles - 1)->m_PositionIndices[0];
                        TRIANGLE(numTriangles)->m_NormalIndices[0] = TRIANGLE(numTriangles - 1)->m_NormalIndices[0];
                        TRIANGLE(numTriangles)->m_VertexAttributeIndices[0] = TRIANGLE(numTriangles - 1)->m_VertexAttributeIndices[0];

                        TRIANGLE(numTriangles)->m_PositionIndices[1] = TRIANGLE(numTriangles - 1)->m_PositionIndices[2];
                        TRIANGLE(numTriangles)->m_NormalIndices[1] = TRIANGLE(numTriangles - 1)->m_NormalIndices[2];
                        TRIANGLE(numTriangles)->m_VertexAttributeIndices[1] = TRIANGLE(numTriangles - 1)->m_VertexAttributeIndices[2];

                        TRIANGLE(numTriangles)->m_PositionIndices[2] = v;
                        TRIANGLE(numTriangles)->m_NormalIndices[2] = n;
                        TRIANGLE(numTriangles)->m_VertexAttributeIndices[2] = add_vertex_attribute(&model->m_Vertices, &verticesAttributeIndices, &ModelVertexAttributeIndex(v, n));

                        group->m_Triangles.push_back(numTriangles);
                        numTriangles++;
                    }
                }
                else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3)
                {
                    /* v/t/n */
                    const char* movingAhead = curPos;
                    movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                    sscanf(buf, "%d/%d/%d", &v, &t, &n);
                    v = (v < 0 ? v + numPositions : v) - 1;
                    t = (t < 0 ? t + numTexcoords : t) - 1;
                    n = (n < 0 ? n + numNormals : n) - 1;
                    TRIANGLE(numTriangles)->m_PositionIndices[0] = v;
                    TRIANGLE(numTriangles)->m_TexcoordsIndices[0] = t;
                    TRIANGLE(numTriangles)->m_NormalIndices[0] = n;
                    TRIANGLE(numTriangles)->m_VertexAttributeIndices[0] = add_vertex_attribute(&model->m_Vertices, &verticesAttributeIndices, &ModelVertexAttributeIndex(v, n, n, -1, t, t));

                    movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                    sscanf(buf, "%d/%d/%d", &v, &t, &n);
                    v = (v < 0 ? v + numPositions : v) - 1;
                    t = (t < 0 ? t + numTexcoords : t) - 1;
                    n = (n < 0 ? n + numNormals : n) - 1;
                    TRIANGLE(numTriangles)->m_PositionIndices[1] = v;
                    TRIANGLE(numTriangles)->m_TexcoordsIndices[1] = t;
                    TRIANGLE(numTriangles)->m_NormalIndices[1] = n;
                    TRIANGLE(numTriangles)->m_VertexAttributeIndices[1] = add_vertex_attribute(&model->m_Vertices, &verticesAttributeIndices, &ModelVertexAttributeIndex(v, n, n, -1, t, t));

                    movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                    sscanf(buf, "%d/%d/%d", &v, &t, &n);
                    v = (v < 0 ? v + numPositions : v) - 1;
                    t = (t < 0 ? t + numTexcoords : t) - 1;
                    n = (n < 0 ? n + numNormals : n) - 1;
                    TRIANGLE(numTriangles)->m_PositionIndices[2] = v;
                    TRIANGLE(numTriangles)->m_TexcoordsIndices[2] = t;
                    TRIANGLE(numTriangles)->m_NormalIndices[2] = n;
                    TRIANGLE(numTriangles)->m_VertexAttributeIndices[2] = add_vertex_attribute(&model->m_Vertices, &verticesAttributeIndices, &ModelVertexAttributeIndex(v, n, n, -1, t, t));

                    group->m_Triangles.push_back(numTriangles);
                    numTriangles++;

                    while (nullptr != movingAhead)
                    {
                        movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                        sscanf(buf, "%d/%d/%d", &v, &t, &n);
                        v = (v < 0 ? v + numPositions : v) - 1;
                        t = (t < 0 ? t + numTexcoords : t) - 1;
                        n = (n < 0 ? n + numNormals : n) - 1;

                        TRIANGLE(numTriangles)->m_PositionIndices[0] = TRIANGLE(numTriangles - 1)->m_PositionIndices[0];
                        TRIANGLE(numTriangles)->m_TexcoordsIndices[0] = TRIANGLE(numTriangles - 1)->m_TexcoordsIndices[0];
                        TRIANGLE(numTriangles)->m_NormalIndices[0] = TRIANGLE(numTriangles - 1)->m_NormalIndices[0];
                        TRIANGLE(numTriangles)->m_VertexAttributeIndices[0] = TRIANGLE(numTriangles - 1)->m_VertexAttributeIndices[0];

                        TRIANGLE(numTriangles)->m_PositionIndices[1] = TRIANGLE(numTriangles - 1)->m_PositionIndices[2];
                        TRIANGLE(numTriangles)->m_TexcoordsIndices[1] = TRIANGLE(numTriangles - 1)->m_TexcoordsIndices[2];
                        TRIANGLE(numTriangles)->m_NormalIndices[1] = TRIANGLE(numTriangles - 1)->m_NormalIndices[2];
                        TRIANGLE(numTriangles)->m_VertexAttributeIndices[1] = TRIANGLE(numTriangles - 1)->m_VertexAttributeIndices[2];

                        TRIANGLE(numTriangles)->m_PositionIndices[2] = v;
                        TRIANGLE(numTriangles)->m_TexcoordsIndices[2] = t;
                        TRIANGLE(numTriangles)->m_NormalIndices[2] = n;
                        TRIANGLE(numTriangles)->m_VertexAttributeIndices[2] = add_vertex_attribute(&model->m_Vertices, &verticesAttributeIndices, &ModelVertexAttributeIndex(v, n, n, -1, t, t));

                        group->m_Triangles.push_back(numTriangles);
                        numTriangles++;
                    }
                }
                else if (sscanf(buf, "%d/%d", &v, &t) == 2)
                {
                    /* v/t */
                    const char* movingAhead = curPos;
                    movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                    sscanf(buf, "%d/%d", &v, &t);
                    v = (v < 0 ? v + numPositions : v) - 1;
                    t = (t < 0 ? t + numTexcoords : t) - 1;
                    TRIANGLE(numTriangles)->m_PositionIndices[0] = v;
                    TRIANGLE(numTriangles)->m_TexcoordsIndices[0] = t;
                    TRIANGLE(numTriangles)->m_VertexAttributeIndices[0] = add_vertex_attribute(&model->m_Vertices, &verticesAttributeIndices, &ModelVertexAttributeIndex(v, v, v, -1, t, t));

                    movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                    sscanf(buf, "%d/%d", &v, &t);
                    v = (v < 0 ? v + numPositions : v) - 1;
                    t = (t < 0 ? t + numTexcoords : t) - 1;
                    TRIANGLE(numTriangles)->m_PositionIndices[1] = v;
                    TRIANGLE(numTriangles)->m_TexcoordsIndices[1] = t;
                    TRIANGLE(numTriangles)->m_VertexAttributeIndices[1] = add_vertex_attribute(&model->m_Vertices, &verticesAttributeIndices, &ModelVertexAttributeIndex(v, v, v, -1, t, t));

                    movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                    sscanf(buf, "%d/%d", &v, &t);
                    v = (v < 0 ? v + numPositions : v) - 1;
                    t = (t < 0 ? t + numTexcoords : t) - 1;
                    TRIANGLE(numTriangles)->m_PositionIndices[2] = v;
                    TRIANGLE(numTriangles)->m_TexcoordsIndices[2] = t;
                    TRIANGLE(numTriangles)->m_VertexAttributeIndices[2] = add_vertex_attribute(&model->m_Vertices, &verticesAttributeIndices, &ModelVertexAttributeIndex(v, v, v, -1, t, t));

                    group->m_Triangles.push_back(numTriangles);
                    numTriangles++;

                    while (nullptr != movingAhead)
                    {
                        movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                        sscanf(buf, "%d/%d", &v, &t);
                        v = (v < 0 ? v + numPositions : v) - 1;
                        t = (t < 0 ? t + numTexcoords : t) - 1;

                        TRIANGLE(numTriangles)->m_PositionIndices[0] = TRIANGLE(numTriangles - 1)->m_PositionIndices[0];
                        TRIANGLE(numTriangles)->m_TexcoordsIndices[0] = TRIANGLE(numTriangles - 1)->m_TexcoordsIndices[0];
                        TRIANGLE(numTriangles)->m_VertexAttributeIndices[0] = TRIANGLE(numTriangles - 1)->m_VertexAttributeIndices[0];

                        TRIANGLE(numTriangles)->m_PositionIndices[1] = TRIANGLE(numTriangles - 1)->m_PositionIndices[2];
                        TRIANGLE(numTriangles)->m_TexcoordsIndices[1] = TRIANGLE(numTriangles - 1)->m_TexcoordsIndices[2];
                        TRIANGLE(numTriangles)->m_VertexAttributeIndices[1] = TRIANGLE(numTriangles - 1)->m_VertexAttributeIndices[2];

                        TRIANGLE(numTriangles)->m_PositionIndices[2] = v;
                        TRIANGLE(numTriangles)->m_TexcoordsIndices[2] = t;
                        TRIANGLE(numTriangles)->m_VertexAttributeIndices[2] = add_vertex_attribute(&model->m_Vertices, &verticesAttributeIndices, &ModelVertexAttributeIndex(v, v, v, -1, t, t));

                        group->m_Triangles.push_back(numTriangles);
                        numTriangles++;
                    }
                }
                else
                {
                    /* v */
                    const char* movingAhead = curPos;
                    movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                    sscanf(buf, "%d", &v);
                    v = (v < 0 ? v + numPositions : v) - 1;
                    TRIANGLE(numTriangles)->m_PositionIndices[0] = v;
                    TRIANGLE(numTriangles)->m_VertexAttributeIndices[0] = add_vertex_attribute(&model->m_Vertices, &verticesAttributeIndices, &ModelVertexAttributeIndex(v));

                    movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                    sscanf(buf, "%d", &v);
                    v = (v < 0 ? v + numPositions : v) - 1;
                    TRIANGLE(numTriangles)->m_PositionIndices[1] = v;
                    TRIANGLE(numTriangles)->m_VertexAttributeIndices[1] = add_vertex_attribute(&model->m_Vertices, &verticesAttributeIndices, &ModelVertexAttributeIndex(v));

                    movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                    sscanf(buf, "%d", &v);
                    v = (v < 0 ? v + numPositions : v) - 1;
                    TRIANGLE(numTriangles)->m_PositionIndices[2] = v;
                    TRIANGLE(numTriangles)->m_VertexAttributeIndices[2] = add_vertex_attribute(&model->m_Vertices, &verticesAttributeIndices, &ModelVertexAttributeIndex(v));

                    group->m_Triangles.push_back(numTriangles);
                    numTriangles++;

                    while (nullptr != movingAhead)
                    {
                        movingAhead = sscanf_string_and_go_ahead(movingAhead, "%s", buf);
                        sscanf(buf, "%d", &v);
                        v = (v < 0 ? v + numPositions : v) - 1;

                        TRIANGLE(numTriangles)->m_PositionIndices[0] = TRIANGLE(numTriangles - 1)->m_PositionIndices[0];
                        TRIANGLE(numTriangles)->m_VertexAttributeIndices[0] = TRIANGLE(numTriangles - 1)->m_VertexAttributeIndices[0];

                        TRIANGLE(numTriangles)->m_PositionIndices[1] = TRIANGLE(numTriangles - 1)->m_PositionIndices[2];
                        TRIANGLE(numTriangles)->m_VertexAttributeIndices[1] = TRIANGLE(numTriangles - 1)->m_VertexAttributeIndices[2];

                        TRIANGLE(numTriangles)->m_PositionIndices[2] = v;
                        TRIANGLE(numTriangles)->m_VertexAttributeIndices[2] = add_vertex_attribute(&model->m_Vertices, &verticesAttributeIndices, &ModelVertexAttributeIndex(v));

                        group->m_Triangles.push_back(numTriangles);
                        numTriangles++;
                    }
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
            numPositions * 3 * sizeof(Vector3) +
            numNormals * 3 * sizeof(Vector3) * (numNormals ? 1 : 0) +
            numTexcoords * 3 * sizeof(Vector2) * (numTexcoords ? 1 : 0) +
            numTriangles * sizeof(ModelTriangle));
#endif
    }

    void ModelLoaderWavefrontObj::CalculateFacetNormals(Model* model)
    {
        assert(nullptr != model);
        assert(model->m_Positions.size() > 0);

        /* clobber any old facet normals */
        model->m_FacetNormals.clear();

        /* allocate memory for the new facet normals */
        model->m_FacetNormals.resize(model->m_Triangles.size());
        for (uint32_t i = 0; i < (uint32_t)model->m_Triangles.size(); i++)
        {
            model->m_Triangles[i]->m_FacetNormalIndex = i;

            Vector3 u = model->m_Positions[TRIANGLE(i)->m_PositionIndices[1]] - model->m_Positions[TRIANGLE(i)->m_PositionIndices[0]];
            Vector3 v = model->m_Positions[TRIANGLE(i)->m_PositionIndices[2]] - model->m_Positions[TRIANGLE(i)->m_PositionIndices[0]];

            Vector3& facetNormal = model->m_FacetNormals[i];
            Vector3Cross(facetNormal, u, v);
            Vector3Normalize(facetNormal, facetNormal);
        }
    }

    void ModelLoaderWavefrontObj::CalculateVertexNormals(Model* model, float angle)
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
        std::vector<Node*> members(model->m_Positions.size(), nullptr);

        /* for every triangle, create a node for each vertex in it */
        for (uint32_t i = 0; i < (uint32_t)model->m_Triangles.size(); i++)
        {
            Node* node = new Node();
            node->m_Index = i;
            node->m_Next = members[TRIANGLE(i)->m_PositionIndices[0]];
            members[TRIANGLE(i)->m_PositionIndices[0]] = node;

            node = new Node();
            node->m_Index = i;
            node->m_Next = members[TRIANGLE(i)->m_PositionIndices[1]];
            members[TRIANGLE(i)->m_PositionIndices[1]] = node;

            node = new Node();
            node->m_Index = i;
            node->m_Next = members[TRIANGLE(i)->m_PositionIndices[2]];
            members[TRIANGLE(i)->m_PositionIndices[2]] = node;
        }

        /* calculate the average normal for each vertex */
        uint32_t numNormals = 0;
        for (uint32_t i = 0; i < (uint32_t)model->m_Positions.size(); i++)
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
                    if (TRIANGLE(node->m_Index)->m_PositionIndices[0] == i)
                    {
                        TRIANGLE(node->m_Index)->m_NormalIndices[0] = avg;
                    }
                    else if (TRIANGLE(node->m_Index)->m_PositionIndices[1] == i)
                    {
                        TRIANGLE(node->m_Index)->m_NormalIndices[1] = avg;
                    }
                    else if (TRIANGLE(node->m_Index)->m_PositionIndices[2] == i)
                    {
                        TRIANGLE(node->m_Index)->m_NormalIndices[2] = avg;
                    }
                }
                else
                {
                    /* if this node wasn't averaged, use the facet normal */
                    normals[numNormals] = model->m_FacetNormals[TRIANGLE(node->m_Index)->m_FacetNormalIndex];
                    if (TRIANGLE(node->m_Index)->m_PositionIndices[0] == i)
                    {
                        TRIANGLE(node->m_Index)->m_NormalIndices[0] = numNormals;
                    }
                    else if (TRIANGLE(node->m_Index)->m_PositionIndices[1] == i)
                    {
                        TRIANGLE(node->m_Index)->m_NormalIndices[1] = numNormals;
                    }
                    else if (TRIANGLE(node->m_Index)->m_PositionIndices[2] == i)
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
        for (uint32_t i = 0; i < (uint32_t)model->m_Positions.size(); i++)
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

    void ModelLoaderWavefrontObj::CalculateVertexTangent(class Model* model)
    {
        // Skip if no texture coordinates data.
        if (model->m_Texcoords.size() <= 0)
        {
            return;
        }

        // Alloc model tangents space.
        model->m_Tangents.resize(model->m_Positions.size());

        // Calculate triangle tangent vector.
        // TODO: average tangents of shared vertices.
        for (int32_t i = 0; i < (int32_t)model->m_Triangles.size(); i++)
        {
            const ModelTriangle* triangle = TRIANGLE(i);

            const Vector3& v0 = model->m_Positions[triangle->m_PositionIndices[0]];
            const Vector3& v1 = model->m_Positions[triangle->m_PositionIndices[1]];
            const Vector3& v2 = model->m_Positions[triangle->m_PositionIndices[2]];

            const float texCoordY0 = model->m_Texcoords[triangle->m_TexcoordsIndices[0]].y;
            const float texCoordY1 = model->m_Texcoords[triangle->m_TexcoordsIndices[1]].y;
            const float texCoordY2 = model->m_Texcoords[triangle->m_TexcoordsIndices[2]].y;

            const Vector3 deltaVertex[2] = { v1 - v0, v2 - v0 };
            const float deltaTexCoordY[2] = { texCoordY1 - texCoordY0, texCoordY2 - texCoordY0 };
            const Vector3 tangent = (deltaVertex[0] * deltaTexCoordY[1] - deltaVertex[1] * deltaTexCoordY[0]).Normalize();
            
            model->m_Tangents[triangle->m_PositionIndices[0]] = Vector4(tangent, 1.0f);
            model->m_Tangents[triangle->m_PositionIndices[1]] = Vector4(tangent, 1.0f);
            model->m_Tangents[triangle->m_PositionIndices[2]] = Vector4(tangent, 1.0f);
        }
    }

    void ModelLoaderWavefrontObj::CalculateVertexTangentTBN(class Model* model)
    {
        // Skip if no texture coordinates data.
        if (model->m_Texcoords.size() <= 0)
        {
            return;
        }

        // Get vertex count. (Positions count.)
        int32_t vertexCount = (int32_t)model->m_Positions.size();

        // Alloc model tangents space.
        model->m_Tangents.resize(vertexCount);

        // Allocate temporary storage for tangents and bitangents and initialize to zeros.
        std::vector<Vector3> tangent(vertexCount);
        std::vector<Vector3> bitangent(vertexCount);

        // Calculate tangent and bitangent for each triangle and add to all three vertices.
        for (int32_t i = 0; i < (int32_t)model->m_Triangles.size(); i++)
        {
            const ModelTriangle* triangle = TRIANGLE(i);

            const uint32_t i0 = triangle->m_PositionIndices[0];
            const uint32_t i1 = triangle->m_PositionIndices[1];
            const uint32_t i2 = triangle->m_PositionIndices[2];

            const Vector3& p0 = model->m_Positions[i0];
            const Vector3& p1 = model->m_Positions[i1];
            const Vector3& p2 = model->m_Positions[i2];

            const Vector2& w0 = model->m_Texcoords[i0];
            const Vector2& w1 = model->m_Texcoords[i1];
            const Vector2& w2 = model->m_Texcoords[i2];

            Vector3 e1 = p1 - p0;
            Vector3 e2 = p2 - p0;
            float x1 = w1.x - w0.x;
            float x2 = w2.x - w0.x;
            float y1 = w1.y - w0.y;
            float y2 = w2.y - w0.y;
            float r = 1.0f / ((x1 * y2 - x2 * y1) + 1e-4f);
            Vector3 t = (e1 * y2 - e2 * y1) * r;
            Vector3 b = (e2 * x1 - e1 * x2) * r;

            tangent[i0] += t;
            tangent[i1] += t;
            tangent[i2] += t;

            bitangent[i0] += b;
            bitangent[i1] += b;
            bitangent[i2] += b;
        }

        // Orthonormalize each tangent and calculate the handedness.
        for (int32_t i = 0; i < vertexCount; i++)
        {
            const Vector3& t = tangent[i];
            const Vector3& b = bitangent[i];
            const Vector3& n = model->m_Normals[i];
            Vector4& vertexTangent = model->m_Tangents[i];
            
            // We use left-handed.
            Vector3 tangentXYZ = Vector3Reject(Vector3(), t, n).Normalize();
            float tangentW = (Vector3Dot(Vector3Cross(Vector3(), t, b), n) > 0.0f) ? 1.0f : -1.0f;
            vertexTangent.Set(tangentXYZ.x, tangentXYZ.y, tangentXYZ.z, tangentW);
        }
    }

    void ModelLoaderWavefrontObj::ProcessOtherData(class Model* model)
    {
        assert(model->m_Texcoords.size() == model->m_Texcoord2s.size());
        for (int32_t i = 0; i < (int32_t)model->m_Texcoords.size(); i++)
        {
            model->m_Texcoord2s[i] = model->m_Texcoords[i];
        }

        for (int32_t i = 0; i < (int32_t)model->m_Triangles.size(); i++)
        {
            ModelTriangle* triangle = model->m_Triangles[i];
            for (int32_t j = 0; j < 3; j++)
            {
                triangle->m_Texcoords2Indices[j] = triangle->m_TexcoordsIndices[j];
            }
        }
    }

    void ModelLoaderWavefrontObj::ReadMTL(Model* model, StringA name)
    {
        // Read material.
    }
}

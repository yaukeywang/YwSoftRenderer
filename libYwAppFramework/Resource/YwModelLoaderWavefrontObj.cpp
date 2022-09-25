// Add by Yaukey at 2019-03-01.
// YW model loader for obj file class.
// Modified version of glm.h
// Nate Robins, 1997, 2000
// nate@pobox.com, http://www.pobox.com/~nate
// Update at 2020-08-17.
// with some other reference:
// https://github.com/gameknife/SoftRenderer/blob/769eeccc8dedd3b1be0a876db7378f66eed401ac/code/SoftRenderer/SrObjLoader.cpp
// in https://github.com/gameknife/SoftRenderer
// Wavefront OBJ model file format reader/writer/manipulator.
// YW model loader for obj file class.

#include <sstream>
#include "YwModel.h"
#include "YwModelLoaderWavefrontObj.h"

// warning C4996: 'strdup': The POSIX name for this item is deprecated. Instead, use the ISO C and C++ conformant name: _strdup. See online help for details.
//#pragma warning(disable:4996)

namespace yw
{
    // ------------------------------------------------------------------
    // Model data parser helper.

    // Helper to read triangle in model.
    #define TRIANGLE(x) (model->m_Triangles[(x)])

    /* Node: general purpose node. */
    struct TriangleVertexNode
    {
        uint32_t m_Index;
        bool m_Averaged;
        TriangleVertexNode* m_Next;

        TriangleVertexNode() : m_Index(0), m_Averaged(false), m_Next(nullptr) {}
    };

    // Find vertex index in vertex format element cache.
    static uint32_t _AddModelVertexIntoCache(std::vector<ModelVertex>& vertices, std::vector<ModelVertexIndex*>& indexCache, const uint32_t newVertexPositionIndex, const ModelVertex& newVertex)
    {
        // If this vertex doesn't already exist in the Vertices list, create a new entry.
        // Add the index of the vertex to the Indices list.
        bool foundVertex = false;
        uint32_t vertexIndex = 0;

        // Since it's very slow to check every element in the vertex list, a hashtable stores
        // vertex indices according to the vertex position's index as reported by the OBJ file.
        if ((uint32_t)indexCache.size() > newVertexPositionIndex)
        {
            ModelVertexIndex* vertexIndexNode = indexCache[newVertexPositionIndex];
            while (nullptr != vertexIndexNode)
            {
                const ModelVertex* vertexFormat = vertices.data() + vertexIndexNode->index;

                // If this vertex is identical to the vertex already in the list, simply
                // point the index buffer to the existing vertex.
                if (0 == memcmp(&newVertex, vertexFormat, sizeof(ModelVertex)))
                {
                    foundVertex = true;
                    vertexIndex = vertexIndexNode->index;

                    break;
                }

                vertexIndexNode = vertexIndexNode->next;
            }
        }

        // Vertex was not found in the list. Create a new entry, both within the Vertices list
        // and also within the hashtable cache.
        if (!foundVertex)
        {
            // Add to the Vertices list.
            vertexIndex = (uint32_t)vertices.size();
            vertices.push_back(newVertex);

            // Add this to the hashtable.
            ModelVertexIndex* newVertexIndexNode = new ModelVertexIndex(vertexIndex, nullptr);
            if (nullptr == newVertexIndexNode)
            {
                return 0;
            }

            // Grow the cache if needed.
            if ((uint32_t)indexCache.size() <= newVertexPositionIndex)
            {
                indexCache.resize(newVertexPositionIndex + 1, nullptr);
            }

            // Add to the end of the linked list.
            ModelVertexIndex* curVertexIndexNode = indexCache[newVertexPositionIndex];
            if (nullptr == curVertexIndexNode)
            {
                // This is the head element.
                indexCache[newVertexPositionIndex] = newVertexIndexNode;
            }
            else
            {
                // Find the tail.
                while (nullptr != curVertexIndexNode->next)
                {
                    curVertexIndexNode = curVertexIndexNode->next;
                }

                curVertexIndexNode->next = newVertexIndexNode;
            }
        }

        return vertexIndex;
    }

    // ------------------------------------------------------------------
    // Wavefront data processing.

    ModelLoaderWavefrontObj::ModelLoaderWavefrontObj() : 
        IModelLoader()
    {
        
    }
    
    ModelLoaderWavefrontObj::~ModelLoaderWavefrontObj()
    {
        // Release each group.
    }

    bool ModelLoaderWavefrontObj::LoadFormData(const StringA& fileName, const uint8_t* data, bool calculateNormals, float calculateNormalAngle, Model* model)
    {
        LoadWavefrontObjFromData(model, data, calculateNormals, calculateNormalAngle);
        return true;
    }

    void ModelLoaderWavefrontObj::LoadWavefrontObjFromData(Model* objModel, const uint8_t* objData, bool calculateNormals, float calculateNormalAngle)
    {
        // Load all basic data like vertices, normals, texcoords & triangles.
        LoadBasicData(objModel, (const char*)objData);

        // Calculate normal if this obj file does not contains any normal.
        if (calculateNormals || (objModel->m_Normals.size() <= 0))
        {
            // Calculate facet normals is necessary.
            CalculateFacetNormals(objModel);

            // Calculate vertex normals using facet normals.
            CalculateVertexNormals(objModel, calculateNormalAngle);
        }

        // Calculate facet and vertex tangents.
        //CalculateVertexTangent(objModel);
        CalculateVertexTangentTBN(objModel);

        // Process other data.
        ProcessOtherData(objModel);
    }

    void ModelLoaderWavefrontObj::LoadBasicData(class Model* model, const char* objData)
    {
        // Use string streaam to parse data.
        std::stringstream modelData(objData);

        // Used for reading temp buffer.
        char command[256];
        memset(command, 0, sizeof(command));

        // Used for reading command name.
        StringA commandName;

        uint32_t numPositions = 0;      /* number of positions in model */
        uint32_t numNormals = 0;        /* number of normals in model */
        uint32_t numTexcoords = 0;      /* number of texcoords in model */
        uint32_t numTriangles = 0;      /* number of triangles in model */
        uint32_t numVertices = 0;       /* number of vertices in model */

        /* set the pointer shortcuts */
        std::vector<Vector3>& vertices = model->m_Positions;    /* array of vertices  */
        std::vector<Vector3>& normals = model->m_Normals;       /* array of normals */
        std::vector<Vector2>& texcoords = model->m_Texcoords;   /* array of texture coordinates */
        std::vector<ModelGroup*>& groups = model->m_Groups;     /* current group pointer */

        /* make a default group */
        ModelGroup* group = new ModelGroup("default"); /* current group pointer */
        groups.push_back(group);

        /* current material (Need to implement) */
        void* material = nullptr;

        // Reading and parsing through all the data until reaching the end.
        while (true)
        {
            modelData >> command;
            if (!modelData)
            {
                break;
            }

            if (0 == strcmp(command, "#"))
            {
                // Comments.
            }
            else if (0 == strcmp(command, "mtllib"))
            {
                modelData >> commandName;
                model->m_MaterialName = commandName;
                ReadMTL(model, commandName);
            }
            else if (0 == strcmp(command, "usemtl"))
            {
                modelData >> commandName;
                material = nullptr; // Find material.
                group->material = nullptr; // Need to implement material. Material is 'buf'.
                //group->material = material = glmFindMaterial(model, buf);
            }
            else if (0 == strcmp(command, "usemap"))
            {
                // Material map.
            }
            else if (0 == strcmp(command, "g"))
            {
                modelData >> commandName;
                group = model->AddGroup(commandName);
                group->material = material; // Need to implement material.
                //group = glmFindGroup(model, buf);
                //group->material = material;
            }
            else if (0 == strcmp(command, "v"))
            {
                float x = 0.0f;
                float y = 0.0f;
                float z = 0.0f;
                modelData >> x >> y >> z;
                vertices.push_back(Vector3(x, y, z));
                numPositions++;
            }
            else if (0 == strcmp(command, "vn"))
            {
                float x = 0.0f;
                float y = 0.0f;
                float z = 0.0f;
                modelData >> x >> y >> z;
                normals.push_back(Vector3(x, y, z));
                numNormals++;
            }
            else if (0 == strcmp(command, "vt"))
            {
                float u = 0.0f;
                float v = 0.0f;
                modelData >> u >> v;
                texcoords.push_back(Vector2(u, 1.0f - v));
                numTexcoords++;
            }
            else if (0 == strcmp(command, "f") && (numPositions > 0))
            {
                // v//n or v/t/n or v/t or v.

                // Add "Empty-Data" (same length with positions) if texcoords and normals are not provided.
                texcoords.empty() ? texcoords.resize(numPositions) : 0;
                normals.empty() ? normals.resize(numPositions) : 0;

                // Used for parsing face data.
                // v = "Position Index", n = "Normal Index", t = "Texture Coordinate index".
                int32_t v = 0;
                int32_t n = 0;
                int32_t t = 0;

                // Vertex format node and vertex index in vertex index buffer.
                ModelVertex modelVertex;
                uint32_t modelVertexIndex = 0;

                // Add a triangle.
                model->m_Triangles.push_back(new ModelTriangle());

                // Base 3 vertex of first face.
                for (int32_t faceIdx = 0; faceIdx < 3; faceIdx++)
                {
                    // Reset vertex data.
                    modelVertex.Reset();

                    // Read vertex.
                    modelData >> v;
                    v = (v < 0 ? v + numPositions : v) - 1;
                    modelVertex.position = vertices[v];
                    TRIANGLE(numTriangles)->positionIndices[faceIdx] = v;

                    if ('/' == modelData.peek())
                    {
                        modelData.ignore();
                        if ('/' != modelData.peek())
                        {
                            // v/t/n or v/t.

                            // Read texcoord.
                            modelData >> t;
                            t = (t < 0 ? t + numTexcoords : t) - 1;
                            modelVertex.texcoord = texcoords[t];
                            TRIANGLE(numTriangles)->texcoordsIndices[faceIdx] = t;
                        }

                        if ('/' == modelData.peek())
                        {
                            // v//n
                            modelData.ignore();

                            // Read normal.
                            modelData >> n;
                            n = (n < 0 ? n + numNormals : n) - 1;
                            modelVertex.normal = normals[n];
                            TRIANGLE(numTriangles)->normalIndices[faceIdx] = n;
                        }
                    }

                    // Add this vertex into cache.
                    modelVertexIndex = _AddModelVertexIntoCache(model->m_Vertices, model->m_VertexIndexCache, v, modelVertex);
                    TRIANGLE(numTriangles)->vertexIndices[faceIdx] = modelVertexIndex;
                    group->triangleIndices.push_back(modelVertexIndex);
                }

                group->triangles.push_back(numTriangles);
                numTriangles++;

                // Extra faces.
                while ('\n' != modelData.peek())
                {
                    // Invalid vertex position index when no more data. (v >= 1)
                    modelData >> v;
                    if (v <= 0)
                    {
                        break;
                    }

                    // Add a triangle.
                    model->m_Triangles.push_back(new ModelTriangle());

                    // Reset vertex data.
                    modelVertex.Reset();

                    // Read vertex.
                    //modelData >> v;
                    v = (v < 0 ? v + numPositions : v) - 1;
                    modelVertex.position = vertices[v];
                    TRIANGLE(numTriangles)->positionIndices[2] = v;

                    if ('/' == modelData.peek())
                    {
                        modelData.ignore();
                        if ('/' != modelData.peek())
                        {
                            // v/t/n or v/t.

                            // Read texcoord.
                            modelData >> t;
                            t = (t < 0 ? t + numTexcoords : t) - 1;
                            modelVertex.texcoord = texcoords[t];
                            TRIANGLE(numTriangles)->texcoordsIndices[2] = t;
                        }

                        if ('/' == modelData.peek())
                        {
                            // v//n
                            modelData.ignore();

                            // Read normal.
                            modelData >> n;
                            n = (n < 0 ? n + numNormals : n) - 1;
                            modelVertex.normal = normals[n];
                            TRIANGLE(numTriangles)->normalIndices[2] = n;
                        }
                    }

                    // Add this vertex into cache.
                    modelVertexIndex = _AddModelVertexIntoCache(model->m_Vertices, model->m_VertexIndexCache, v, modelVertex);

                    // Vertex 0.
                    TRIANGLE(numTriangles)->positionIndices[0] = TRIANGLE(numTriangles - 1)->positionIndices[0];
                    TRIANGLE(numTriangles)->texcoordsIndices[0] = TRIANGLE(numTriangles - 1)->texcoordsIndices[0];
                    TRIANGLE(numTriangles)->normalIndices[0] = TRIANGLE(numTriangles - 1)->normalIndices[0];
                    TRIANGLE(numTriangles)->vertexIndices[0] = TRIANGLE(numTriangles - 1)->vertexIndices[0];
                    group->triangleIndices.push_back(TRIANGLE(numTriangles - 1)->vertexIndices[0]);

                    // Vertex 1.
                    TRIANGLE(numTriangles)->positionIndices[1] = TRIANGLE(numTriangles - 1)->positionIndices[2];
                    TRIANGLE(numTriangles)->texcoordsIndices[1] = TRIANGLE(numTriangles - 1)->texcoordsIndices[2];
                    TRIANGLE(numTriangles)->normalIndices[1] = TRIANGLE(numTriangles - 1)->normalIndices[2];
                    TRIANGLE(numTriangles)->vertexIndices[1] = TRIANGLE(numTriangles - 1)->vertexIndices[2];
                    group->triangleIndices.push_back(TRIANGLE(numTriangles - 1)->vertexIndices[2]);

                    // Vertex 2.
                    TRIANGLE(numTriangles)->vertexIndices[2] = modelVertexIndex;
                    group->triangleIndices.push_back(modelVertexIndex);

                    group->triangles.push_back(numTriangles);
                    numTriangles++;
                }
            }
            else
            {
                // Invalid data or other command we are current not support.
                //assert(false);
                LOGE_A("ModelLoaderWavefrontObj.LoadBasicData: Invalid data or not support command provided, command = \"%s\".", command);
            }

            modelData.ignore(1000, '\n');
        }
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
            model->m_Triangles[i]->facetNormalIndex = i;

            Vector3 u = model->m_Positions[TRIANGLE(i)->positionIndices[1]] - model->m_Positions[TRIANGLE(i)->positionIndices[0]];
            Vector3 v = model->m_Positions[TRIANGLE(i)->positionIndices[2]] - model->m_Positions[TRIANGLE(i)->positionIndices[0]];

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
        std::vector<TriangleVertexNode*> members(model->m_Positions.size(), nullptr);

        /* for every triangle, create a node for each vertex in it */
        for (uint32_t i = 0; i < (uint32_t)model->m_Triangles.size(); i++)
        {
            TriangleVertexNode* node = new TriangleVertexNode();
            node->m_Index = i;
            node->m_Next = members[TRIANGLE(i)->positionIndices[0]];
            members[TRIANGLE(i)->positionIndices[0]] = node;

            node = new TriangleVertexNode();
            node->m_Index = i;
            node->m_Next = members[TRIANGLE(i)->positionIndices[1]];
            members[TRIANGLE(i)->positionIndices[1]] = node;

            node = new TriangleVertexNode();
            node->m_Index = i;
            node->m_Next = members[TRIANGLE(i)->positionIndices[2]];
            members[TRIANGLE(i)->positionIndices[2]] = node;
        }

        /* calculate the average normal for each vertex */
        uint32_t numNormals = 0;
        for (uint32_t i = 0; i < (uint32_t)model->m_Positions.size(); i++)
        {
            /* calculate an average normal for this vertex by averaging the
                facet normal of every triangle this vertex is in */
            TriangleVertexNode* node = members[i];
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
                float dot = Vector3Dot(model->m_FacetNormals[TRIANGLE(node->m_Index)->facetNormalIndex],
                    model->m_FacetNormals[TRIANGLE(members[i]->m_Index)->facetNormalIndex]);
                if (dot > cos_angle)
                {
                    node->m_Averaged = true;
                    average += model->m_FacetNormals[TRIANGLE(node->m_Index)->facetNormalIndex];
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
                    if (TRIANGLE(node->m_Index)->positionIndices[0] == i)
                    {
                        TRIANGLE(node->m_Index)->normalIndices[0] = avg;
                    }
                    else if (TRIANGLE(node->m_Index)->positionIndices[1] == i)
                    {
                        TRIANGLE(node->m_Index)->normalIndices[1] = avg;
                    }
                    else if (TRIANGLE(node->m_Index)->positionIndices[2] == i)
                    {
                        TRIANGLE(node->m_Index)->normalIndices[2] = avg;
                    }
                }
                else
                {
                    /* if this node wasn't averaged, use the facet normal */
                    normals[numNormals] = model->m_FacetNormals[TRIANGLE(node->m_Index)->facetNormalIndex];
                    if (TRIANGLE(node->m_Index)->positionIndices[0] == i)
                    {
                        TRIANGLE(node->m_Index)->normalIndices[0] = numNormals;
                    }
                    else if (TRIANGLE(node->m_Index)->positionIndices[1] == i)
                    {
                        TRIANGLE(node->m_Index)->normalIndices[1] = numNormals;
                    }
                    else if (TRIANGLE(node->m_Index)->positionIndices[2] == i)
                    {
                        TRIANGLE(node->m_Index)->normalIndices[2] = numNormals;
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
            TriangleVertexNode* node = members[i];
            while (nullptr != node)
            {
                TriangleVertexNode* tail = node;
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

        // Update vertex normal in vertex element buffer.
        for (uint32_t i = 0; i < (uint32_t)model->m_Triangles.size(); i++)
        {
            ModelTriangle* triangle = TRIANGLE(i);
            for (uint32_t j = 0; j < 3; j++)
            {
                ModelVertex* modelVertex = &(model->m_Vertices[triangle->vertexIndices[j]]);
                modelVertex->normal = model->m_Normals[triangle->normalIndices[j]];
            }
        }
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

            const Vector3& v0 = model->m_Positions[triangle->positionIndices[0]];
            const Vector3& v1 = model->m_Positions[triangle->positionIndices[1]];
            const Vector3& v2 = model->m_Positions[triangle->positionIndices[2]];

            const float texCoordY0 = model->m_Texcoords[triangle->texcoordsIndices[0]].y;
            const float texCoordY1 = model->m_Texcoords[triangle->texcoordsIndices[1]].y;
            const float texCoordY2 = model->m_Texcoords[triangle->texcoordsIndices[2]].y;

            const Vector3 deltaVertex[2] = { v1 - v0, v2 - v0 };
            const float deltaTexCoordY[2] = { texCoordY1 - texCoordY0, texCoordY2 - texCoordY0 };
            const Vector3 tangent = (deltaVertex[0] * deltaTexCoordY[1] - deltaVertex[1] * deltaTexCoordY[0]).Normalize();
            
            model->m_Tangents[triangle->positionIndices[0]] = Vector4(tangent, 1.0f);
            model->m_Tangents[triangle->positionIndices[1]] = Vector4(tangent, 1.0f);
            model->m_Tangents[triangle->positionIndices[2]] = Vector4(tangent, 1.0f);
        }

        // Update all tangets in final vertex format cache.
        for (int32_t i = 0; i < (int32_t)model->m_VertexIndexCache.size(); i++)
        {
            ModelVertexIndex* vertexIndexNode = model->m_VertexIndexCache[i];
            while (nullptr != vertexIndexNode)
            {
                ModelVertex* vertexFormat = &(model->m_Vertices[vertexIndexNode->index]);
                vertexFormat->tangent = model->m_Tangents[i];
                vertexIndexNode = vertexIndexNode->next;
            }
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
        std::vector<Vector3> normals(vertexCount);

        // Calculate tangent and bitangent for each triangle and add to all three vertices.
        for (int32_t i = 0; i < (int32_t)model->m_Triangles.size(); i++)
        {
            const ModelTriangle* triangle = TRIANGLE(i);

            const uint32_t i0 = triangle->positionIndices[0];
            const uint32_t i1 = triangle->positionIndices[1];
            const uint32_t i2 = triangle->positionIndices[2];

            const Vector3& p0 = model->m_Positions[i0];
            const Vector3& p1 = model->m_Positions[i1];
            const Vector3& p2 = model->m_Positions[i2];

            const Vector2& w0 = model->m_Texcoords[triangle->texcoordsIndices[0]];
            const Vector2& w1 = model->m_Texcoords[triangle->texcoordsIndices[1]];
            const Vector2& w2 = model->m_Texcoords[triangle->texcoordsIndices[2]];

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

            normals[i0] = model->m_Normals[triangle->normalIndices[0]];
            normals[i1] = model->m_Normals[triangle->normalIndices[1]];
            normals[i2] = model->m_Normals[triangle->normalIndices[2]];
        }

        // Orthonormalize each tangent and calculate the handedness.
        for (int32_t i = 0; i < vertexCount; i++)
        {
            const Vector3& t = tangent[i];
            const Vector3& b = bitangent[i];
            const Vector3& n = normals[i];
            Vector4& vertexTangent = model->m_Tangents[i];
            
            // We use left-handed.
            Vector3 tangentXYZ = Vector3Reject(Vector3(), t, n).Normalize();
            float tangentW = (Vector3Dot(Vector3Cross(Vector3(), t, b), n) > 0.0f) ? 1.0f : -1.0f;
            vertexTangent.Set(tangentXYZ.x, tangentXYZ.y, tangentXYZ.z, tangentW);
        }

        // Update all tangets in final vertex format cache.
        for (int32_t i = 0; i < (int32_t)model->m_VertexIndexCache.size(); i++)
        {
            ModelVertexIndex* vertexIndexNode = model->m_VertexIndexCache[i];
            while (nullptr != vertexIndexNode)
            {
                ModelVertex* vertexFormat = &(model->m_Vertices[vertexIndexNode->index]);
                vertexFormat->tangent = model->m_Tangents[i];
                vertexIndexNode = vertexIndexNode->next;
            }
        }
    }

    void ModelLoaderWavefrontObj::ProcessOtherData(class Model* model)
    {
    }

    void ModelLoaderWavefrontObj::ReadMTL(Model* model, StringA name)
    {
        // Read material.
    }
}

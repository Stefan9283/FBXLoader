#include "Mesh.h"

Mesh* getMeshData(FbxNode* pNode)
{
    const char* nodeName = pNode->GetName();

    FbxVector4 rotation = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
    FbxVector4 translation = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
    FbxVector4 scaling = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
    
    /*
    FbxDouble3 translation = pNode->LclTranslation.Get();
    FbxDouble3 rotation = pNode->LclRotation.Get();
    FbxDouble3 scaling = pNode->LclScaling.Get();
    */
    glm::mat4 T, R, S;
    S = glm::scale(glm::mat4(1), glm::vec3(scaling[0], scaling[1], scaling[2]));
    T = glm::translate(glm::mat4(1), glm::vec3(translation[0], translation[1], translation[2]));
    R = glm::rotate(glm::mat4(1), glm::radians((float)rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
    R = glm::rotate(           R, glm::radians((float)rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
    R = glm::rotate(           R, glm::radians((float)rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));

    FbxMesh* mesh = pNode->GetMesh();



    Mesh* newMesh = new Mesh;

    FbxGeometryElementNormal* lNormalElement = mesh->GetElementNormal();


    //fbxsdk::FbxTransform Transform = pNode->GetTransform();

    //std::cout << Transform. << "\n";

    newMesh->Transform = T * R * S;

    int currentIndex = 0;

    mesh->GenerateNormals(true);

    for (auto i = 0; i < mesh->GetPolygonCount(); i++)
    {
        for (auto j = 0; j < mesh->GetPolygonSize(i); j++)
        {
            int vertIndex = mesh->GetPolygonVertex(i, j);
            newMesh->indices.push_back(currentIndex++);
            Vertex newVert;


            fbxsdk::FbxVector4 Position = mesh->GetControlPointAt(vertIndex);
            newVert.Position = glm::vec3(Position[0], Position[1], Position[2]);
            
            FbxVector4 Normals;
            mesh->GetPolygonVertexNormal(i, vertIndex, Normals);
            std::cout << Normals[0] << " " << Normals[1] << " " << Normals[2] << " " << Normals[3] <<  "\n";

            /*
            int lNormalIndex = lNormalElement->GetIndexArray().GetAt(vertIndex);
            fbxsdk::FbxVector4 Normals;
            Normals = lNormalElement->GetDirectArray().GetAt(lNormalIndex);

            newVert.Normal = glm::vec3(Normals[0], Normals[1], Normals[2]);
          
            std::cout << Normals[0] << " " << Normals[1] << " " << Normals[2] << "\n";
            */
            newMesh->vertices.push_back(newVert);
            //std::cout << i << " " << j << " " << vertIndex << " " << "\n";


        }
    }

    return newMesh;

    
}
//////////////////////////////////
/*
bool VBOMesh::Initialize(const FbxMesh* pMesh)
{
    if (!pMesh->GetNode())
        return false;

    const int lPolygonCount = pMesh->GetPolygonCount();

    // Count the polygon count of each material
    FbxLayerElementArrayTemplate<int>* lMaterialIndice = NULL;
    FbxGeometryElement::EMappingMode lMaterialMappingMode = FbxGeometryElement::eNone;
    if (pMesh->GetElementMaterial())
    {
        lMaterialIndice = &pMesh->GetElementMaterial()->GetIndexArray();
        lMaterialMappingMode = pMesh->GetElementMaterial()->GetMappingMode();
        if (lMaterialIndice && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
        {
            FBX_ASSERT(lMaterialIndice->GetCount() == lPolygonCount);
            if (lMaterialIndice->GetCount() == lPolygonCount)
            {
                // Count the faces of each material
                for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
                {
                    const int lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);
                    if (mSubMeshes.GetCount() < lMaterialIndex + 1)
                    {
                        mSubMeshes.Resize(lMaterialIndex + 1);
                    }
                    if (mSubMeshes[lMaterialIndex] == NULL)
                    {
                        mSubMeshes[lMaterialIndex] = new SubMesh;
                    }
                    mSubMeshes[lMaterialIndex]->TriangleCount += 1;
                }

                // Make sure we have no "holes" (NULL) in the mSubMeshes table. This can happen
                // if, in the loop above, we resized the mSubMeshes by more than one slot.
                for (int i = 0; i < mSubMeshes.GetCount(); i++)
                {
                    if (mSubMeshes[i] == NULL)
                        mSubMeshes[i] = new SubMesh;
                }

                // Record the offset (how many vertex)
                const int lMaterialCount = mSubMeshes.GetCount();
                int lOffset = 0;
                for (int lIndex = 0; lIndex < lMaterialCount; ++lIndex)
                {
                    mSubMeshes[lIndex]->IndexOffset = lOffset;
                    lOffset += mSubMeshes[lIndex]->TriangleCount * 3;
                    // This will be used as counter in the following procedures, reset to zero
                    mSubMeshes[lIndex]->TriangleCount = 0;
                }
                FBX_ASSERT(lOffset == lPolygonCount * 3);
            }
        }
    }

    // All faces will use the same material.
    if (mSubMeshes.GetCount() == 0)
    {
        mSubMeshes.Resize(1);
        mSubMeshes[0] = new SubMesh();
    }

    // Congregate all the data of a mesh to be cached in VBOs.
    // If normal or UV is by polygon vertex, record all vertex attributes by polygon vertex.
    mHasNormal = pMesh->GetElementNormalCount() > 0;
    mHasUV = pMesh->GetElementUVCount() > 0;
    FbxGeometryElement::EMappingMode lNormalMappingMode = FbxGeometryElement::eNone;
    FbxGeometryElement::EMappingMode lUVMappingMode = FbxGeometryElement::eNone;
    if (mHasNormal)
    {
        lNormalMappingMode = pMesh->GetElementNormal(0)->GetMappingMode();
        if (lNormalMappingMode == FbxGeometryElement::eNone)
        {
            mHasNormal = false;
        }
        if (mHasNormal && lNormalMappingMode != FbxGeometryElement::eByControlPoint)
        {
            mAllByControlPoint = false;
        }
    }
    if (mHasUV)
    {
        lUVMappingMode = pMesh->GetElementUV(0)->GetMappingMode();
        if (lUVMappingMode == FbxGeometryElement::eNone)
        {
            mHasUV = false;
        }
        if (mHasUV && lUVMappingMode != FbxGeometryElement::eByControlPoint)
        {
            mAllByControlPoint = false;
        }
    }

    // Allocate the array memory, by control point or by polygon vertex.
    int lPolygonVertexCount = pMesh->GetControlPointsCount();
    if (!mAllByControlPoint)
    {
        lPolygonVertexCount = lPolygonCount * TRIANGLE_VERTEX_COUNT;
    }
    float* lVertices = new float[lPolygonVertexCount * VERTEX_STRIDE];
    unsigned int* lIndices = new unsigned int[lPolygonCount * TRIANGLE_VERTEX_COUNT];
    float* lNormals = NULL;
    if (mHasNormal)
    {
        lNormals = new float[lPolygonVertexCount * NORMAL_STRIDE];
    }
    float* lUVs = NULL;
    FbxStringList lUVNames;
    pMesh->GetUVSetNames(lUVNames);
    const char* lUVName = NULL;
    if (mHasUV && lUVNames.GetCount())
    {
        lUVs = new float[lPolygonVertexCount * UV_STRIDE];
        lUVName = lUVNames[0];
    }

    // Populate the array with vertex attribute, if by control point.
    const FbxVector4* lControlPoints = pMesh->GetControlPoints();
    FbxVector4 lCurrentVertex;
    FbxVector4 lCurrentNormal;
    FbxVector2 lCurrentUV;
    if (mAllByControlPoint)
    {
        const FbxGeometryElementNormal* lNormalElement = NULL;
        const FbxGeometryElementUV* lUVElement = NULL;
        if (mHasNormal)
        {
            lNormalElement = pMesh->GetElementNormal(0);
        }
        if (mHasUV)
        {
            lUVElement = pMesh->GetElementUV(0);
        }
        for (int lIndex = 0; lIndex < lPolygonVertexCount; ++lIndex)
        {
            // Save the vertex position.
            lCurrentVertex = lControlPoints[lIndex];
            lVertices[lIndex * VERTEX_STRIDE] = static_cast<float>(lCurrentVertex[0]);
            lVertices[lIndex * VERTEX_STRIDE + 1] = static_cast<float>(lCurrentVertex[1]);
            lVertices[lIndex * VERTEX_STRIDE + 2] = static_cast<float>(lCurrentVertex[2]);
            lVertices[lIndex * VERTEX_STRIDE + 3] = 1;

            // Save the normal.
            if (mHasNormal)
            {
                int lNormalIndex = lIndex;
                if (lNormalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
                {
                    lNormalIndex = lNormalElement->GetIndexArray().GetAt(lIndex);
                }
                lCurrentNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
                lNormals[lIndex * NORMAL_STRIDE] = static_cast<float>(lCurrentNormal[0]);
                lNormals[lIndex * NORMAL_STRIDE + 1] = static_cast<float>(lCurrentNormal[1]);
                lNormals[lIndex * NORMAL_STRIDE + 2] = static_cast<float>(lCurrentNormal[2]);
            }

            // Save the UV.
            if (mHasUV)
            {
                int lUVIndex = lIndex;
                if (lUVElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
                {
                    lUVIndex = lUVElement->GetIndexArray().GetAt(lIndex);
                }
                lCurrentUV = lUVElement->GetDirectArray().GetAt(lUVIndex);
                lUVs[lIndex * UV_STRIDE] = static_cast<float>(lCurrentUV[0]);
                lUVs[lIndex * UV_STRIDE + 1] = static_cast<float>(lCurrentUV[1]);
            }
        }

    }

    int lVertexCount = 0;
    for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
    {
        // The material for current face.
        int lMaterialIndex = 0;
        if (lMaterialIndice && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
        {
            lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);
        }

        // Where should I save the vertex attribute index, according to the material
        const int lIndexOffset = mSubMeshes[lMaterialIndex]->IndexOffset +
            mSubMeshes[lMaterialIndex]->TriangleCount * 3;
        for (int lVerticeIndex = 0; lVerticeIndex < TRIANGLE_VERTEX_COUNT; ++lVerticeIndex)
        {
            const int lControlPointIndex = pMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);

            if (mAllByControlPoint)
            {
                lIndices[lIndexOffset + lVerticeIndex] = static_cast<unsigned int>(lControlPointIndex);
            }
            // Populate the array with vertex attribute, if by polygon vertex.
            else
            {
                lIndices[lIndexOffset + lVerticeIndex] = static_cast<unsigned int>(lVertexCount);

                lCurrentVertex = lControlPoints[lControlPointIndex];
                lVertices[lVertexCount * VERTEX_STRIDE] = static_cast<float>(lCurrentVertex[0]);
                lVertices[lVertexCount * VERTEX_STRIDE + 1] = static_cast<float>(lCurrentVertex[1]);
                lVertices[lVertexCount * VERTEX_STRIDE + 2] = static_cast<float>(lCurrentVertex[2]);
                lVertices[lVertexCount * VERTEX_STRIDE + 3] = 1;

                if (mHasNormal)
                {
                    pMesh->GetPolygonVertexNormal(lPolygonIndex, lVerticeIndex, lCurrentNormal);
                    lNormals[lVertexCount * NORMAL_STRIDE] = static_cast<float>(lCurrentNormal[0]);
                    lNormals[lVertexCount * NORMAL_STRIDE + 1] = static_cast<float>(lCurrentNormal[1]);
                    lNormals[lVertexCount * NORMAL_STRIDE + 2] = static_cast<float>(lCurrentNormal[2]);
                }

                if (mHasUV)
                {
                    bool lUnmappedUV;
                    pMesh->GetPolygonVertexUV(lPolygonIndex, lVerticeIndex, lUVName, lCurrentUV, lUnmappedUV);
                    lUVs[lVertexCount * UV_STRIDE] = static_cast<float>(lCurrentUV[0]);
                    lUVs[lVertexCount * UV_STRIDE + 1] = static_cast<float>(lCurrentUV[1]);
                }
            }
            ++lVertexCount;
        }
        mSubMeshes[lMaterialIndex]->TriangleCount += 1;
    }


    return true;
}

void VBOMesh::UpdateVertexPosition(const FbxMesh* pMesh, const FbxVector4* pVertices) const
{
    // Convert to the same sequence with data in GPU.
    float* lVertices = NULL;
    int lVertexCount = 0;
    if (mAllByControlPoint)
    {
        lVertexCount = pMesh->GetControlPointsCount();
        lVertices = new float[lVertexCount * VERTEX_STRIDE];
        for (int lIndex = 0; lIndex < lVertexCount; ++lIndex)
        {
            lVertices[lIndex * VERTEX_STRIDE] = static_cast<float>(pVertices[lIndex][0]);
            lVertices[lIndex * VERTEX_STRIDE + 1] = static_cast<float>(pVertices[lIndex][1]);
            lVertices[lIndex * VERTEX_STRIDE + 2] = static_cast<float>(pVertices[lIndex][2]);
            lVertices[lIndex * VERTEX_STRIDE + 3] = 1;
        }
    }
    else
    {
        const int lPolygonCount = pMesh->GetPolygonCount();
        lVertexCount = lPolygonCount * TRIANGLE_VERTEX_COUNT;
        lVertices = new float[lVertexCount * VERTEX_STRIDE];

        int lVertexCount = 0;
        for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
        {
            for (int lVerticeIndex = 0; lVerticeIndex < TRIANGLE_VERTEX_COUNT; ++lVerticeIndex)
            {
                const int lControlPointIndex = pMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);
                lVertices[lVertexCount * VERTEX_STRIDE] = static_cast<float>(pVertices[lControlPointIndex][0]);
                lVertices[lVertexCount * VERTEX_STRIDE + 1] = static_cast<float>(pVertices[lControlPointIndex][1]);
                lVertices[lVertexCount * VERTEX_STRIDE + 2] = static_cast<float>(pVertices[lControlPointIndex][2]);
                lVertices[lVertexCount * VERTEX_STRIDE + 3] = 1;
                ++lVertexCount;
            }
        }
    }

    // Transfer into GPU.
    if (lVertices)
    {
        glBindBuffer(GL_ARRAY_BUFFER, mVBONames[VERTEX_VBO]);
        glBufferData(GL_ARRAY_BUFFER, lVertexCount * VERTEX_STRIDE * sizeof(float), lVertices, GL_STATIC_DRAW);
        delete[] lVertices;
    }
}

void VBOMesh::Draw(int pMaterialIndex, ShadingMode pShadingMode) const
{
    // Where to start.
    GLsizei lOffset = mSubMeshes[pMaterialIndex]->IndexOffset * sizeof(unsigned int);
    if (pShadingMode == SHADING_MODE_SHADED)
    {
        const GLsizei lElementCount = mSubMeshes[pMaterialIndex]->TriangleCount * 3;
        glDrawElements(GL_TRIANGLES, lElementCount, GL_UNSIGNED_INT, reinterpret_cast<const GLvoid*>(lOffset));
    }
    else
    {
        for (int lIndex = 0; lIndex < mSubMeshes[pMaterialIndex]->TriangleCount; ++lIndex)
        {
            // Draw line loop for every triangle.
            glDrawElements(GL_LINE_LOOP, TRIANGLE_VERTEX_COUNT, GL_UNSIGNED_INT, reinterpret_cast<const GLvoid*>(lOffset));
            lOffset += sizeof(unsigned int) * TRIANGLE_VERTEX_COUNT;
        }
    }
}
*/


//////////////////////////////////


Model* ReadFBX(const char* path)
{

#pragma region LOADING
    // Change the following filename to a suitable filename value.
    const char* lFilename = path;

    // Initialize the SDK manager. This object handles memory management.
    FbxManager* lSdkManager = FbxManager::Create();

    // Create the IO settings object.
    FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
    lSdkManager->SetIOSettings(ios);

    // Create an importer using the SDK manager.
    FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");


    // Use the first argument as the filename for the importer.
    if (!lImporter->Initialize(lFilename, -1, lSdkManager->GetIOSettings())) {
        printf("Call to FbxImporter::Initialize() failed.\n");
        printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
        exit(-1);
    }
    // Create a new scene so that it can be populated by the imported file.
    FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");

    // Import the contents of the file into the scene.
    lImporter->Import(lScene);

    FbxGeometryConverter conv(lSdkManager);

    conv.Triangulate(lScene, true);

    // The file is imported, so get rid of the importer.
    lImporter->Destroy();

    // Print the nodes of the scene and their attributes recursively.
    // Note that we are not printing the root node because it should
    // not contain any attributes.

    Model* new_model = new Model;
    std::vector<Mesh*> Meshes;

    FbxNode* lRootNode = lScene->GetRootNode();

    std::cout << "nodes total " << lScene->GetNodeCount() << "\n";
    
    if (lRootNode)
        for (int i = 0; i < lScene->GetNodeCount(); i++)
        {
            //std::cout << lScene->GetNode(i)->GetName() << "\n";

            if (lScene->GetNode(i)->GetMesh())
            {
                new_model->meshes.push_back(getMeshData(lScene->GetNode(i)));
            }
        }
    //lScene->GetNode()
    /*
    if (lRootNode)
        for (int i = 0; i < lRootNode->GetChildCount(); i++)
        {
            std::cout << lRootNode->GetChild(i)->GetName() << "\n";

            if (lRootNode->GetChild(i)->GetMesh())
            {
                new_model->meshes.push_back(getMeshData(lRootNode->GetChild(i)));
            }
        }
     */
    // Destroy the SDK manager and all the other objects it was handling.
    lSdkManager->Destroy();
#pragma endregion 

    for (auto mesh : new_model->meshes)
        mesh->prepare();

    return new_model;
}
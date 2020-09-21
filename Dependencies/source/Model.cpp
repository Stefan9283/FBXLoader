#include "Model.h"

void Model::Draw(Shader* shader)
{
    glm::mat4 transform = getModelMatrix();
    shader->setMat4("model", &transform);
    for (auto mesh : meshes)
        mesh->Draw(shader);
}
Model::~Model()
{
    for (auto mesh : meshes)
        delete mesh;
}
Model::Model()
{
    Position = glm::vec3(0.0f);
    Scaling = glm::vec3(1.0f);
    Rotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
}
glm::mat4 Model::getModelMatrix()
{
    glm::mat4 T, R, S;
    T = glm::translate(glm::mat4(1), Position);
    R = glm::toMat4(Rotation);
    S = glm::scale(glm::mat4(1), Scaling);
    return T * R * S;
}


Mesh* getMeshData(FbxNode* pNode)
{
    //const char* nodeName = pNode->GetName();

    FbxAMatrix geometricMatrix;
    geometricMatrix.SetIdentity();

    FbxVector4 rotation = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
    FbxVector4 translation = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
    FbxVector4 scaling = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
    geometricMatrix.SetT(translation);
    geometricMatrix.SetR(rotation);
    geometricMatrix.SetS(scaling);

    FbxAMatrix localMatrix = pNode->EvaluateLocalTransform();

    FbxNode* pParentNode = pNode->GetParent();
    FbxAMatrix parentMatrix = pParentNode->EvaluateLocalTransform();
    while ((pParentNode = pParentNode->GetParent()) != NULL)
    {
        parentMatrix = pParentNode->EvaluateLocalTransform() * parentMatrix;
    }

    FbxAMatrix matrix = parentMatrix * localMatrix * geometricMatrix;

    translation = matrix.GetT();
    scaling = matrix.GetS();
    rotation = matrix.GetR();

    //std::cout << rotation[0] << " " << rotation[1] << " " << rotation[2] << " " << rotation[3] << "\n";

    glm::mat4 T, R1, R2, R3, S;

    T = glm::translate(glm::mat4(1), glm::vec3(translation[0], translation[1], translation[2]));
    
    //glm::rotate(glm::mat4(1), (float)glm::radians(rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
    
    R1 = glm::rotate(glm::mat4(1), (float)glm::radians(rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
    R2 = glm::rotate(glm::mat4(1), (float)glm::radians(rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
    R3 = glm::rotate(glm::mat4(1), (float)glm::radians(rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));
    
    S = glm::scale(glm::mat4(1), glm::vec3(scaling[0], scaling[1], scaling[2]));

    FbxMesh* mesh = pNode->GetMesh();

#pragma region WIP

    

    /*
    for (int index = 0; index < mesh->GetLayerCount(); index++)
    {
        FbxLayer* layer = mesh->GetLayer(index);
        FbxLayerElementTexture* tex = layer->GetTextures(fbxsdk::FbxLayerElement::EType::eTextureDiffuse);
        FbxLayerElementTexture* tex2 = layer->GetTextures(fbxsdk::FbxLayerElement::EType::eTextureAmbient);
        std::cout << index << " ";

        if (tex)
            std::cout << tex->GetName();
        if (tex2)
            std::cout << tex->GetName();

        std::cout << "\n";
        
    }*/


    for (auto i = 0; i < pNode->GetMaterialCount(); i++)
    {
        FbxSurfaceMaterial* mat = pNode->GetMaterial(i);
        std::cout << mat->sDiffuse << "\n";
        
        FbxProperty prop = mat->FindProperty(mat->sDiffuse);

        

        // FbxDouble3 diffuse = 
    }






    /*

    int materialCount = pNode->GetSrcObjectCount<FbxSurfaceMaterial>();

    for (int index = 0; index < materialCount; index++)
    {
        FbxSurfaceMaterial* material = (FbxSurfaceMaterial*)pNode->GetSrcObject<FbxSurfaceMaterial>(index);

        if (material != NULL)
        {
            // This only gets the material of type sDiffuse, you probably need to traverse all Standard Material Property by its name to get all possible textures.
            FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);

            // Check if it's layeredtextures
            int layeredTextureCount = prop.GetSrcObjectCount<FbxLayeredTexture>();


            int textureCount = prop.GetSrcObjectCount<FbxTexture>();
            for (int j = 0; j < textureCount; j++)
            {
                FbxTexture* texture = FbxCast<FbxTexture>(prop.GetSrcObject<FbxTexture>(j));
                // Then, you can get all the properties of the texture, include its name
                const char* textureName = texture->GetName();

                FbxProperty p = texture->RootProperty.Find("Filename");
                std::cout << textureName << " " << p.GetNameAsCStr() << "\n";
            }
        }
    }

    */



    /*
    for (auto i = 0; i < pNode->GetSrcObjectCount<FbxSurfaceMaterial>(); i++)
    {
        FbxSurfaceMaterial* material = (FbxSurfaceMaterial*)pNode->GetSrcObject<FbxSurfaceMaterial>(i);

        std::cout << material->GetName() << "\n";
        FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
        // Check if it's layeredtextures
        int layeredTextureCount = prop.GetSrcObjectCount<FbxLayeredTexture>();

        std::cout << layeredTextureCount << "\n";

        if (layeredTextureCount > 0)
        {
            for (int j = 0; j < layeredTextureCount; j++)
            {
                FbxLayeredTexture* layered_texture = FbxCast<FbxLayeredTexture>(prop.GetSrcObject<FbxLayeredTexture>(j));
                int lcount = layered_texture->GetSrcObjectCount<FbxTexture>();

                for (int k = 0; k < lcount; k++)
                {
                    FbxTexture* texture = FbxCast<FbxTexture>(layered_texture->GetSrcObject<FbxTexture>(k));
                    // Then, you can get all the properties of the texture, include its name
                    const char* textureName = texture->GetName();
                    std::cout << "\t name" << textureName << "\n";
                }
            }
        }
    }
    
    

    for (auto materialIndex = 0; materialIndex < mesh->GetElementMaterialCount(); materialIndex++)
    {
         FbxGeometryElementMaterial* material = mesh->GetElementMaterial(materialIndex);
         std::cout << material->GetName() << "\n";
         
         //material.text
    }



    FbxStringList lUVSetNameList;
    mesh->GetUVSetNames(lUVSetNameList);

    

    for (auto textureIndex = 0; textureIndex < lUVSetNameList.GetCount(); textureIndex++)
    {
        FbxStringListItem* texture = lUVSetNameList.GetItemAt(textureIndex);
        //texture->mString
  
        std::cout << lUVSetNameList.GetStringAt(textureIndex) << "\n";
    }
    */

#pragma endregion

    Mesh* newMesh = new Mesh;

    newMesh->Transform = T * R2 * R3 * R1 * S;

    int currentIndex = 0;

    FbxGeometryElementNormal* lNormalElement = mesh->GetElementNormal();
    FbxGeometryElementUV* lUVElement = mesh->GetElementUV();

    if (lNormalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
    {
        if (lUVElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
        { 
        
            int lIndexByPolygonVertex = 0;

            for (auto i = 0; i < mesh->GetPolygonCount(); i++)
            {

                for (auto j = 0; j < mesh->GetPolygonSize(i); j++)
                {
                    int vertIndex = mesh->GetPolygonVertex(i, j);
                    newMesh->indices.push_back(currentIndex++);
                    Vertex newVert;


                    fbxsdk::FbxVector4 Position = mesh->GetControlPointAt(vertIndex);
                    newVert.Position = glm::vec3(Position[0], Position[1], Position[2]);

                    int lNormalIndex = 0;
                    //reference mode is direct, the normal index is same as lIndexByPolygonVertex.
                    if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
                        lNormalIndex = lIndexByPolygonVertex;

                    //reference mode is index-to-direct, get normals by the index-to-direct
                    if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
                        lNormalIndex = lNormalElement->GetIndexArray().GetAt(lIndexByPolygonVertex);

                    //Got normals of each polygon-vertex.
                    FbxVector4 lNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
                    //std::cout << "normals for polygon " << i << " vertex " << j << " " << lNormal[0] << " " << lNormal[1] << " " << lNormal[2] << " " << lNormal[3] << "\n";

                    newVert.Normal = glm::vec3(lNormal[0], lNormal[1], lNormal[2]);



                    int UVindex;


                    UVindex = lUVElement->GetIndexArray().GetAt(lIndexByPolygonVertex);
                    FbxVector2 TextureCoords = lUVElement->GetDirectArray().GetAt(UVindex);

                    newVert.TextureCoords = glm::vec2(TextureCoords[0], TextureCoords[1]);

                    newMesh->vertices.push_back(newVert);



                    lIndexByPolygonVertex++;
                }
            }

        }
        
    }
    else {
        delete newMesh;
        throw std::runtime_error("Add control point method\n");
        }
    return newMesh;

    
}



void buildSkellyBoi(FbxNode* rootNode)
{
    FbxSkeleton* lSkeleton = (FbxSkeleton*)rootNode->GetNodeAttribute();
    /*
    printf("Skeleton Name: ", (char*)rootNode->GetName());
    //DisplayMetaDataConnections(lSkeleton);

    const char* lSkeletonTypes[] = { "Root", "Limb", "Limb Node", "Effector" };

    printf("    Type: ", lSkeletonTypes[lSkeleton->GetSkeletonType()]);

    if (lSkeleton->GetSkeletonType() == FbxSkeleton::eLimb)
    {
        printf("    Limb Length: ", lSkeleton->LimbLength.Get());
    }
    else if (lSkeleton->GetSkeletonType() == FbxSkeleton::eLimbNode)
    {
        printf("    Limb Node Size: ", lSkeleton->Size.Get());
    }
    else if (lSkeleton->GetSkeletonType() == FbxSkeleton::eRoot)
    {
        printf("    Limb Root Size: ", lSkeleton->Size.Get());
    }

    printf("    Color: ", lSkeleton->GetLimbNodeColor());*/
}

Model* ReadFBX(const char* path)
{

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

    FbxGeometryConverter converter(lSdkManager);

    converter.Triangulate(lScene, true);

    // The file is imported, so get rid of the importer.
    lImporter->Destroy();


    Model* new_model = new Model;
    std::vector<Mesh*> Meshes;

    /*
    std::cout << "/////////////////////////////////////////////////////\n";

    for (auto index = 0; index < lScene->GetTextureCount(); index++)
    {
        std::cout << index << "\n";
    }
    
    std::cout << "/////////////////////////////////////////////////////\n";
    */

    FbxNode* lRootNode = lScene->GetRootNode();

    FbxVector4 rotation = lRootNode->GetGeometricRotation(FbxNode::eSourcePivot);
    FbxVector4 translation = lRootNode->GetGeometricTranslation(FbxNode::eSourcePivot);
    FbxVector4 scaling = lRootNode->GetGeometricScaling(FbxNode::eSourcePivot);

    /*
    glm::mat4 T, R, S;
    S = glm::scale(glm::mat4(1), glm::vec3(scaling[0], scaling[1], scaling[2]));
    T = glm::translate(glm::mat4(1), glm::vec3(translation[0], translation[1], translation[2]));
    */
    glm::mat4 R;
    R = glm::rotate(glm::mat4(1), glm::radians((float)rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
    R = glm::rotate(R, glm::radians((float)rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
    R = glm::rotate(R, glm::radians((float)rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));

    new_model->Rotation = glm::toQuat(R);
    new_model->Position = glm::vec3(translation[0], translation[1], translation[2]);
    new_model->Scaling = glm::vec3(scaling[0], scaling[1], scaling[2]);


    //std::cout << "nodes total " << lScene->GetNodeCount() << "\n";
    


    buildSkellyBoi(lRootNode);

    if (lRootNode)
        for (int i = 0; i < lScene->GetNodeCount(); i++)
        {
            //std::cout << lScene->GetNode(i)->GetName() << "\n";


            for (auto attribIndex = 0; attribIndex < lScene->GetNode(i)->GetNodeAttributeCount(); attribIndex++)
            {
                FbxNodeAttribute* attrib = lScene->GetNode(i)->GetNodeAttributeByIndex(attribIndex);
                switch (attrib->GetAttributeType())
                {
                case FbxNodeAttribute::eMesh: new_model->meshes.push_back(getMeshData(lScene->GetNode(i))); break;
                case FbxNodeAttribute::eSkeleton: //std::cout << lScene->GetNode(i)->GetName() <<"\n"; 
                    break;
                }
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

    new_model->name = path;

    for (auto mesh : new_model->meshes)
        mesh->prepare();

    return new_model;
}

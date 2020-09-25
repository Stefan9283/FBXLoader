#include "App.h"

int App::isAlreadyLoaded(Texture tex)
{
    for (auto i=0;i<LoadedTextures.size();i++)
        if (LoadedTextures[i].filename == tex.filename)return i;
    return -1;
}

void fixPath(std::string* path)
{
    for (int i = 0; i < (*path).size()-1; i++)
    {
        if((*path)[i]=='\\' && (*path)[i+1]!=' ')
            (*path)[i]='/';

        //std::cout << (*path)[i] << "\n";
    }

}



void printFbxDouble3(FbxDouble3 v)
{
    std::cout << v[0] << " " << v[1] << " " << v[2] << "\n";
}

glm::vec3 FbxDouble3tovec3(FbxDouble3 v)
{
    return glm::vec3(v[0], v[1], v[2]);
}

int App::MaterialIsLoaded(Material m)
{
    for (auto i = 0; i < Materials.size(); i++)
        if (Materials[i].diff == m.diff &&
            Materials[i].emiss == m.emiss &&
            Materials[i].spec == m.spec)
            return i;

    return -1;

}

#define SHOWTESTUREFEEDBACK false

Mesh* App::getMeshData(FbxNode* pNode)
{

#pragma region Transformations
    const char* nodeName = pNode->GetName();

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
    Mesh* newMesh = new Mesh;
    newMesh->Transform = T * R2 * R3 * R1 * S;

#pragma endregion


#pragma region TEXTURES

    int materialCount = pNode->GetSrcObjectCount<FbxSurfaceMaterial>();
    

    std::vector<std::string> types;
    types.push_back(FbxSurfaceMaterial::sDiffuse);
    types.push_back(FbxSurfaceMaterial::sDiffuseFactor);

    types.push_back(FbxSurfaceMaterial::sEmissive);
    types.push_back(FbxSurfaceMaterial::sEmissiveFactor);

    types.push_back(FbxSurfaceMaterial::sAmbient);
    types.push_back(FbxSurfaceMaterial::sAmbientFactor);

    types.push_back(FbxSurfaceMaterial::sNormalMap);


    types.push_back(FbxSurfaceMaterial::sBump);
    types.push_back(FbxSurfaceMaterial::sBumpFactor);

    types.push_back(FbxSurfaceMaterial::sDisplacementColor);
    types.push_back(FbxSurfaceMaterial::sDisplacementFactor);

    types.push_back(FbxSurfaceMaterial::sReflection);
    types.push_back(FbxSurfaceMaterial::sReflectionFactor);

    types.push_back(FbxSurfaceMaterial::sVectorDisplacementColor);
    types.push_back(FbxSurfaceMaterial::sVectorDisplacementFactor);

    types.push_back(FbxSurfaceMaterial::sTransparentColor);
    types.push_back(FbxSurfaceMaterial::sTransparencyFactor);

    types.push_back(FbxSurfaceMaterial::sSpecular);
    types.push_back(FbxSurfaceMaterial::sSpecularFactor);

    types.push_back(FbxSurfaceMaterial::sShininess);



    for(auto type : types)
    for (int index = 0; index < materialCount; index++)
    {
        FbxSurfaceMaterial* material = (FbxSurfaceMaterial*)pNode->GetSrcObject<FbxSurfaceMaterial>(index);

        if (material != NULL)
        {
            // NOT ANYMORE This only gets the material of type sDiffuse, you probably need to traverse all Standard Material Property by its name to get all possible textures.
            FbxProperty prop = material->FindProperty(type.c_str());
            // Check if it's layeredtextures
            int layeredTextureCount = prop.GetSrcObjectCount<FbxLayeredTexture>();


            if (layeredTextureCount > 0)
            {
                if (SHOWTESTUREFEEDBACK)
                {
                    std::cout << type.c_str() << "\n";
                    std::cout << "\tNum of textures: " << layeredTextureCount << "\n";

                }
                
                for (int j = 0; j < layeredTextureCount; j++)
                {
                    FbxLayeredTexture* layered_texture = FbxCast<FbxLayeredTexture>(prop.GetSrcObject<FbxLayeredTexture>(j));
                    int lcount = layered_texture->GetSrcObjectCount<FbxTexture>();

                    for (int k = 0; k < lcount; k++)
                    {
                        FbxTexture* texture = FbxCast<FbxTexture>(layered_texture->GetSrcObject<FbxTexture>(k));
                        // Then, you can get all the properties of the texture, include its name
                        FbxFileTexture* f = FbxCast<FbxFileTexture>(texture);


                        Texture newTex;
                        newTex.filename = f->GetFileName();
                        newTex.type = type;

                        fixPath(&newTex.filename);

                        
                        if (SHOWTESTUREFEEDBACK)
                        {
                            std::cout << "\t\t" << newTex.filename.c_str() << " 1\n";

                        }


                        int index = isAlreadyLoaded(newTex);
                        if (index == -1)
                        {
                            LoadedTextures.push_back(newTex);
                            newMesh->texIndices.push_back(LoadedTextures.size()-1);
                        }
                        else
                            newMesh->texIndices.push_back(index);
                    }
                }
            }
            else
            {

                // Directly get textures
                int textureCount = prop.GetSrcObjectCount<FbxTexture>();
                if (SHOWTESTUREFEEDBACK && textureCount)
                {
                    std::cout << type.c_str() << "\n";
                    std::cout << "\tNum of textures: " << textureCount << "\n";
                }


                for (int j = 0; j < textureCount; j++)
                {
                    FbxTexture* texture = FbxCast<FbxTexture>(prop.GetSrcObject<FbxTexture>(j));
                    // Then, you can get all the properties of the texture, include its name

                    FbxFileTexture* f = FbxCast<FbxFileTexture>(texture);
                    //                     texture->GetTextureUse()
                    Texture newTex;
                    newTex.filename = f->GetFileName();
                    newTex.type = type;

                    
                    fixPath(&newTex.filename);

                    if(SHOWTESTUREFEEDBACK)
                        std::cout << "\t\t" << newTex.filename.c_str() << " 2\n";


                    int index = isAlreadyLoaded(newTex);
                    if (index == -1)
                    {
                        LoadedTextures.push_back(newTex);
                        newMesh->texIndices.push_back(LoadedTextures.size() - 1);
                    }
                    else
                        newMesh->texIndices.push_back(index);

                }
            }
        }
    }
#pragma endregion

#pragma region MATERIAL
    Material material;

    FbxSurfacePhong* phong_material = (FbxSurfacePhong*)pNode->GetSrcObject<FbxSurfacePhong>(0);

    FbxDouble3 diff = phong_material->Diffuse.Get(); //Kd
    //printFbxDouble3(diff);

    FbxDouble3 emiss = phong_material->Emissive.Get(); //Ke
    //printFbxDouble3(emiss);

    FbxDouble specf = phong_material->SpecularFactor.Get(); // spec - half of what Blender sees
    //std::cout << specf << "\n";
    /*
    FbxDouble3 disp = phong_material->DisplacementColor.Get();
    printFbxDouble3(disp);

    FbxDouble3 amb = phong_material->Ambient.Get();
    printFbxDouble3(amb);
    
    FbxDouble3 bump = phong_material->Bump.Get();
    printFbxDouble3(bump);
   
    FbxDouble3 spec = phong_material->Specular.Get();
    printFbxDouble3(spec);
   
    FbxDouble3 ref = phong_material->Reflection.Get();
    printFbxDouble3(ref);
    */
    
    material.diff = FbxDouble3tovec3(diff);
    material.emiss = FbxDouble3tovec3(emiss);
    material.spec = (float)specf;

    int matIndex= MaterialIsLoaded(material);
    if (matIndex == -1)
    {
        newMesh->matIndex = Materials.size();
        Materials.push_back(material);
    }
    else newMesh->matIndex = matIndex;
#pragma endregion


#pragma region VERTICES
    int currentIndex = 0;


    fbxsdk::FbxGeometryElementNormal* lNormalElement = mesh->GetElementNormal();
    fbxsdk::FbxGeometryElementTangent* lTangentElement = mesh->GetElementTangent();

    //fbxsdk::FbxGeometryElementBinormal* lBinormalElement = mesh->GetElementBinormal();

    if (!lTangentElement)
    {
        mesh->GenerateTangentsDataForAllUVSets(true);
        lTangentElement = mesh->GetElementTangent();
    }

    fbxsdk::FbxGeometryElementUV* lUVElement = mesh->GetElementUV();

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
            {
                lNormalIndex = lIndexByPolygonVertex;
            }
            //reference mode is index-to-direct, get normals by the index-to-direct
            if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
                lNormalIndex = lNormalElement->GetIndexArray().GetAt(lIndexByPolygonVertex);

            //Got normals of each polygon-vertex.
            FbxVector4 lNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
            //std::cout << "normals for polygon " << i << " vertex " << j << " " << lNormal[0] << " " << lNormal[1] << " " << lNormal[2] << " " << lNormal[3] << "\n";

            newVert.Normal = glm::vec3(lNormal[0], lNormal[1], lNormal[2]);











            /// /// WIP
            int lTangentIndex = 0;
            //reference mode is direct, the normal index is same as lIndexByPolygonVertex.
            if (lTangentElement->GetReferenceMode() == FbxGeometryElement::eDirect)
                lTangentIndex = lIndexByPolygonVertex;

            //reference mode is index-to-direct, get normals by the index-to-direct
            if (lTangentElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
                lTangentIndex = lTangentElement->GetIndexArray().GetAt(lIndexByPolygonVertex);

            //Got normals of each polygon-vertex.
            FbxVector4 lTangent = lTangentElement->GetDirectArray().GetAt(lTangentIndex);
            //std::cout << "normals for polygon " << i << " vertex " << j << " " << lNormal[0] << " " << lNormal[1] << " " << lNormal[2] << " " << lNormal[3] << "\n";

            newVert.Tangent = glm::vec3(lTangent[0], lTangent[1], lTangent[2]);

            /// /// WIP
 

















            int UVindex;


            UVindex = lUVElement->GetIndexArray().GetAt(lIndexByPolygonVertex);
            FbxVector2 TextureCoords = lUVElement->GetDirectArray().GetAt(UVindex);

            newVert.TextureCoords = glm::vec2(TextureCoords[0], TextureCoords[1]);

            newMesh->vertices.push_back(newVert);



            lIndexByPolygonVertex++;
        }
    }
    /*
    for (auto vert : newMesh->vertices)
        std::cout << glm::to_string(vert.Tangent) << "\n";
*/
#pragma endregion

    return newMesh;

}


Model* App::ReadFBX(const char* path)
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



    FbxNode* lRootNode = lScene->GetRootNode();

    FbxVector4 rotation = lRootNode->GetGeometricRotation(FbxNode::eSourcePivot);
    FbxVector4 translation = lRootNode->GetGeometricTranslation(FbxNode::eSourcePivot);
    FbxVector4 scaling = lRootNode->GetGeometricScaling(FbxNode::eSourcePivot);

    glm::mat4 R;
    R = glm::rotate(glm::mat4(1), glm::radians((float)rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
    R = glm::rotate(R, glm::radians((float)rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
    R = glm::rotate(R, glm::radians((float)rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));

    new_model->Rotation = glm::toQuat(R);
    new_model->Position = glm::vec3(translation[0], translation[1], translation[2]);
    new_model->Scaling = glm::vec3(scaling[0], scaling[1], scaling[2]);



    if (lRootNode)
        for (int i = 0; i < lScene->GetNodeCount(); i++)
        {

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

     // Destroy the SDK manager and all the other objects it was handling.
    lSdkManager->Destroy();

    new_model->name = path;

    for (auto mesh : new_model->meshes)
        mesh->prepare();

    return new_model;
}

#include "App.h"

int App::TexturelIsLoaded(Texture tex)
{
    for (auto i=0;i<LoadedTextures.size();i++)
        if (LoadedTextures[i].filename == tex.filename)return i;
    return -1;
}
int App::MaterialIsLoaded(Material m)
{
    for (auto i = 0; i < Materials.size(); i++)
        if (Materials[i].diff == m.diff &&
            Materials[i].emiss == m.emiss &&
            Materials[i].shininess == m.shininess &&
            Materials[i].amb == m.amb)
            return i;

    return -1;

}

/*
void fixPath(std::string* path)
{
    for (int i = 0; i < (*path).size()-1; i++)
    {
        if ((*path)[i] == '\\' && (*path)[i + 1] != ' ')
            (*path)[i]='/';

        //std::cout << (*path)[i] << "\n";
    }

}
*/

// LEGACY FUNC
Mesh* App::extractNodeMesh(FbxNode* pNode)
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

    glm::mat4 T, R1, R2, R3, S;

    T = glm::translate(glm::mat4(1), glm::vec3(translation[0], translation[1], translation[2]));

    R1 = glm::rotate(glm::mat4(1), (float)glm::radians(rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
    R2 = glm::rotate(glm::mat4(1), (float)glm::radians(rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
    R3 = glm::rotate(glm::mat4(1), (float)glm::radians(rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));

    S = glm::scale(glm::mat4(1), glm::vec3(scaling[0], scaling[1], scaling[2]));


    FbxMesh* mesh = pNode->GetMesh();

    Mesh* newMesh = new Mesh;
    newMesh->Transform = T * R2 * R3 * R1 * S;

    //std::cout << glm::to_string(newMesh->Transform) << "\n";

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



    for (auto type : types)
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
                    if (SHOWTEXTUREFEEDBACK)
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

                            //fixPath(&newTex.filename);


                            if (SHOWTEXTUREFEEDBACK)
                            {
                                std::cout << "\t\t" << newTex.filename.c_str() << " 1\n";

                            }


                            int index = TexturelIsLoaded(newTex);
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
                else
                {

                    // Directly get textures
                    int textureCount = prop.GetSrcObjectCount<FbxTexture>();
                    if (SHOWTEXTUREFEEDBACK && textureCount)
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


                        //fixPath(&newTex.filename);

                        if (SHOWTEXTUREFEEDBACK)
                            std::cout << "\t\t" << newTex.filename.c_str() << " 2\n";


                        int index = TexturelIsLoaded(newTex);
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


    /*
    for (int i = 0; i < mesh->GetNode()->GetSrcObjectCount<FbxSurfacePhong>(); i++)
    {
        FbxSurfacePhong* phong_material = (FbxSurfacePhong*)mesh->GetNode()->GetSrcObject<FbxSurfacePhong>(i);
        std::cout << nodeName << " phong";
        FbxDouble3 diff = phong_material->Diffuse.Get(); //Kd
        printFbxDouble3(diff);
    }*/

    /*for (int i = 0; i < mesh->GetNode()->GetSrcObjectCount<FbxSurfaceLambert>(); i++)
    {
        FbxSurfaceLambert* phong_material = (FbxSurfaceLambert*)mesh->GetNode()->GetSrcObject<FbxSurfaceLambert>();
        std::cout << nodeName << " lambert";
        FbxDouble3 diff = phong_material->Diffuse.Get(); //Kd
        printFbxDouble3(diff);
    }*/


    FbxSurfacePhong* phong_material = (FbxSurfacePhong*)mesh->GetNode()->GetSrcObject<FbxSurfacePhong>();



    //FbxSurfaceMaterial* materialSurface = (FbxSurfaceMaterial*)mesh->GetNode()->GetSrcObject<FbxSurfaceMaterial>();

    //phong_material.get

    FbxDouble3 diff = phong_material->Diffuse.Get(); //Kd
    //printFbxDouble3(diff);

    FbxDouble3 emiss = phong_material->Emissive.Get(); //Ke
    //printFbxDouble3(emiss);

    FbxDouble shininess = phong_material->Shininess.Get();

    FbxDouble specf = phong_material->SpecularFactor.Get(); // spec - half of what Blender sees
    //std::cout << specf << "\n";

    FbxDouble3 disp = phong_material->DisplacementColor.Get();
    //printFbxDouble3(disp);

    FbxDouble3 amb = phong_material->Ambient.Get();
    //printFbxDouble3(amb);

    FbxDouble3 bump = phong_material->Bump.Get();
    //printFbxDouble3(bump);

    FbxDouble3 spec = phong_material->Specular.Get();
    //printFbxDouble3(spec);

    FbxDouble3 ref = phong_material->Reflection.Get();
    //printFbxDouble3(ref);

    material.amb = FbxDouble3tovec3(amb);
    material.diff = FbxDouble3tovec3(diff);
    material.emiss = FbxDouble3tovec3(emiss);
    material.spec = FbxDouble3tovec3(spec);
    material.shininess = (float)shininess;

    int matIndex = MaterialIsLoaded(material);
    if (matIndex == -1)
    {
        newMesh->matIndex = Materials.size();
        Materials.push_back(material);
    }
    else newMesh->matIndex = matIndex;
#pragma endregion
#pragma region VERTICES
    int currentIndex = 0;

    fbxsdk::FbxGeometryElementNormal* lNormalElement = mesh->GetElementNormal(0);
    fbxsdk::FbxGeometryElementTangent* lTangentElement = mesh->GetElementTangent(0);

    //fbxsdk::FbxGeometryElementBinormal* lBinormalElement = mesh->GetElementBinormal();

    if (lNormalElement)
    {
        mesh->GenerateNormals(true);
        lNormalElement = mesh->GetElementNormal(0);
    }

    if (!lTangentElement)
    {
        mesh->GenerateTangentsDataForAllUVSets(true);
        lTangentElement = mesh->GetElementTangent(0);
    }

    fbxsdk::FbxGeometryElementUV* lUVElement = mesh->GetElementUV(0);

    int lIndexByPolygonVertex = 0;

    //std::cout << "lay " << mesh->GetLayerCount() << "\n"; // 1

    for (auto i = 0; i < mesh->GetPolygonCount(); i++)
    {

        for (auto j = 0; j < mesh->GetPolygonSize(i); j++)
        {
            // Indices
            int vertIndex = mesh->GetPolygonVertex(i, j);
            newMesh->indices.push_back(currentIndex++);
            

            Vertex newVert;
            
            // Space Coords
            fbxsdk::FbxVector4 Position = mesh->GetControlPointAt(vertIndex);
            newVert.Position = glm::vec3(Position[0], Position[1], Position[2]);




            // Normals
            int lNormalIndex = 0;
            if (!lNormalElement)
                throw std::runtime_error("AAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
            //reference mode is direct, the normal index is same as lIndexByPolygonVertex
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



            // Tangents and Texture coords
            if (mesh->GetUVLayerCount())
            {
                int lTangentIndex = 0;
                if (!lTangentElement)
                {
                    throw std::runtime_error("AAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
                    assert(0);
                }

                if (lTangentElement->GetReferenceMode() == FbxGeometryElement::eDirect)
                    lTangentIndex = lIndexByPolygonVertex;
                if (lTangentElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
                    lTangentIndex = lTangentElement->GetIndexArray().GetAt(lIndexByPolygonVertex);

                FbxVector4 lTangent = lTangentElement->GetDirectArray().GetAt(lTangentIndex);

                newVert.Tangent = glm::vec3(lTangent[0], lTangent[1], lTangent[2]);

                int UVindex;
                UVindex = lUVElement->GetIndexArray().GetAt(lIndexByPolygonVertex);
                FbxVector2 TextureCoords = lUVElement->GetDirectArray().GetAt(UVindex);

                newVert.TextureCoords = glm::vec2(TextureCoords[0], TextureCoords[1]);
            }
            else
            {
                newVert.Tangent = glm::vec3(0);
                newVert.TextureCoords = glm::vec2(0);
            }


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


// CURRENT FUNCS
Mesh* App::getMeshData(FbxMesh* mesh, int material_index)
{

    Mesh* newMesh = new Mesh;

#pragma region MATERIAL
    Material material;

    FbxSurfacePhong* phong_material = (FbxSurfacePhong*)mesh->GetNode()->GetSrcObject<FbxSurfacePhong>(material_index);

    FbxDouble3 diff = phong_material->Diffuse.Get(); //Kd
    //printFbxDouble3(diff);
    //printFbxDouble3(diff);
    FbxDouble3 emiss = phong_material->Emissive.Get(); //Ke
    //printFbxDouble3(emiss);

    FbxDouble specf = phong_material->SpecularFactor.Get(); // spec - half of what Blender sees
    //std::cout << specf << "\n";

    FbxDouble3 disp = phong_material->DisplacementColor.Get();
    //printFbxDouble3(disp);
    FbxDouble shininess = phong_material->Shininess.Get();

    FbxDouble3 amb = phong_material->Ambient.Get();
    //printFbxDouble3(amb);

    FbxDouble3 bump = phong_material->Bump.Get();
    //printFbxDouble3(bump);

    FbxDouble3 spec = phong_material->Specular.Get();
    //printFbxDouble3(spec);

    FbxDouble3 ref = phong_material->Reflection.Get();
    //printFbxDouble3(ref);

    material.amb = FbxDouble3tovec3(amb);
    material.diff = FbxDouble3tovec3(diff);
    material.emiss = FbxDouble3tovec3(emiss);
    material.spec = FbxDouble3tovec3(spec);
    material.shininess = (float)shininess;


    int matIndex = MaterialIsLoaded(material);
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

    if (!lNormalElement)
    {
        mesh->GenerateNormals(true);
        lTangentElement = mesh->GetElementTangent();
    }
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
            if (!lNormalElement)
                throw std::runtime_error("AAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
            if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
            {
                lNormalIndex = lIndexByPolygonVertex;
            }
            //reference mode is index-to-direct, get normals by the index-to-direct
            if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
                lNormalIndex = lNormalElement->GetIndexArray().GetAt(lIndexByPolygonVertex);

            //Got normals of each polygon-vertex.
            FbxVector4 lNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
            //std::cout << "normals for polygon " << i << " vertex " << lIndexByPolygonVertex << " " << lNormal[0] << " " << lNormal[1] << " " << lNormal[2] << " " << lNormal[3] << "\n";

            newVert.Normal = glm::vec3(lNormal[0], lNormal[1], lNormal[2]);
            










            if (mesh->GetUVLayerCount())
            {
                int lTangentIndex = 0;
                if (!lTangentElement)
                {
                    throw std::runtime_error("AAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
                    assert(0);
                }

                //reference mode is direct, the normal index is same as lIndexByPolygonVertex.
                if (lTangentElement->GetReferenceMode() == FbxGeometryElement::eDirect)
                    lTangentIndex = lIndexByPolygonVertex;

                //reference mode is index-to-direct, get normals by the index-to-direct
                if (lTangentElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
                    lTangentIndex = lTangentElement->GetIndexArray().GetAt(lIndexByPolygonVertex);

                //Got normals of each polygon-vertex.
                FbxVector4 lTangent = lTangentElement->GetDirectArray().GetAt(lTangentIndex);
                //std::cout << "tangent for polygon " << i << " vertex " << j << " " << lTangent[0] << " " << lTangent[1] << " " << lTangent[2] << " " << lTangent[3] << "\n";

                newVert.Tangent = glm::vec3(lTangent[0], lTangent[1], lTangent[2]);

                /// /// WIP

                int UVindex;

                UVindex = lUVElement->GetIndexArray().GetAt(lIndexByPolygonVertex);
                FbxVector2 TextureCoords = lUVElement->GetDirectArray().GetAt(UVindex);

                newVert.TextureCoords = glm::vec2(TextureCoords[0], TextureCoords[1]);

            }
            else
            {
                newVert.Tangent = glm::vec3(0);
                newVert.TextureCoords = glm::vec2(0);
            }




            newMesh->vertices.push_back(newVert);



            lIndexByPolygonVertex++;
        }
    }
#pragma endregion

    return newMesh;
}
void App::recursiveReadMeshes(FbxNode* node, std::vector<Mesh*>* meshes)
{

    bool hasMeshes = false;

    for (auto i = 0; i < node->GetNodeAttributeCount(); i++)
    {
        FbxMesh* nodeAttrib = (FbxMesh*)node->GetNodeAttributeByIndex(i);
        if (nodeAttrib->GetAttributeType() == 4)
        {
            hasMeshes = true;
            break;
        }
    }

    if (hasMeshes)
    {

        #pragma region TRANSFORM
        glm::mat4 transform_matrix = extractTransformFromNode(node);
        #pragma endregion
        #pragma region TEXTURES

        std::vector<unsigned int> textureIndices;

        int materialCount = node->GetSrcObjectCount<FbxSurfaceMaterial>();


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



        for (auto type : types)
            for (int index = 0; index < materialCount; index++)
            {
                FbxSurfaceMaterial* material = (FbxSurfaceMaterial*)node->GetSrcObject<FbxSurfaceMaterial>(index);

                //std::cout << type << "\n";

                if (material != NULL)
                {
                    // NOT ANYMORE This only gets the material of type sDiffuse, you probably need to traverse all Standard Material Property by its name to get all possible textures.
                    FbxProperty prop = material->FindProperty(type.c_str());
                    // Check if it's layeredtextures
                    int layeredTextureCount = prop.GetSrcObjectCount<FbxLayeredTexture>();


                    if (layeredTextureCount > 0)
                    {
                        if (SHOWTEXTUREFEEDBACK)
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

                                //fixPath(&newTex.filename);


                                if (SHOWTEXTUREFEEDBACK)
                                {
                                    std::cout << "\t\t" << newTex.filename.c_str() << " 1\n";

                                }


                                int index = TexturelIsLoaded(newTex);
                                if (index == -1)
                                {
                                    LoadedTextures.push_back(newTex);
                                    LoadedTextures[LoadedTextures.size() - 1].objectCount = 1;
                                    LoadedTextures[LoadedTextures.size() - 1].id = TextureFromFile(LoadedTextures[LoadedTextures.size() - 1].filename.c_str());
                                    textureIndices.push_back(LoadedTextures.size() - 1);
                                }
                                else
                                {
                                    LoadedTextures[LoadedTextures.size() - 1].objectCount++;
                                    textureIndices.push_back(index);
                                }
                            }
                        }
                    }
                    else
                    {

                        // Directly get textures
                        int textureCount = prop.GetSrcObjectCount<FbxTexture>();
                        if (SHOWTEXTUREFEEDBACK && textureCount)
                        {
                            std::cout << type.c_str() << "\n";
                            std::cout << "\tNum of textures: " << textureCount << "\n";
                        }


                        for (int j = 0; j < textureCount; j++)
                        {
                            FbxTexture* texture = FbxCast<FbxTexture>(prop.GetSrcObject<FbxTexture>(j));
                            // Then, you can get all the properties of the texture, include its name

                            FbxFileTexture* f = FbxCast<FbxFileTexture>(texture);
                            Texture newTex;
                            newTex.filename = f->GetFileName();
                            newTex.type = type;


                            //fixPath(&newTex.filename);

                            if (SHOWTEXTUREFEEDBACK)
                                std::cout << "\t\t" << newTex.filename.c_str() << " 2\n";


                            int index = TexturelIsLoaded(newTex);
                            if (index == -1)
                            {
                                LoadedTextures.push_back(newTex);
                                LoadedTextures[LoadedTextures.size() - 1].objectCount = 1;
                                LoadedTextures[LoadedTextures.size() - 1].id = TextureFromFile(LoadedTextures[LoadedTextures.size() - 1].filename.c_str());
                                textureIndices.push_back(LoadedTextures.size() - 1);
                            }
                            else
                            {
                                LoadedTextures[LoadedTextures.size() - 1].objectCount++;
                                textureIndices.push_back(index);
                            }

                        }
                    }
                }
            }
        #pragma endregion
        #pragma region MESH


        for (auto i = 0; i < node->GetNodeAttributeCount(); i++)
        {
            FbxMesh* nodeAttrib = (FbxMesh*)node->GetNodeAttributeByIndex(i);
            if (nodeAttrib->GetAttributeType() == 4)
            {

                FbxLayerElementArrayTemplate<int>* arr;
                nodeAttrib->GetMaterialIndices(&arr);

                meshes->push_back(getMeshData(nodeAttrib, arr->GetAt(0)));
                (*meshes)[meshes->size() - 1]->Transform = transform_matrix;

                for (auto j = 0; j < textureIndices.size(); j++)
                {
                    (*meshes)[meshes->size() - 1]->texIndices.push_back(textureIndices[j]);
                }
            }
        }
        #pragma endregion

    }
    for (auto i = 0; i < node->GetChildCount(); i++)
        recursiveReadMeshes(node->GetChild(i), meshes);
}


Model* App::ReadFBX(const char* path)
{
#pragma region FBX Importer
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
    
    
    //converter.RemoveBadPolygonsFromMeshes(lScene);
    
    if (!converter.SplitMeshesPerMaterial(lScene, true))
    {
        std::cout << "Failed to split by materials\n";
    }
    if (!converter.Triangulate(lScene, true))
    {
        std::cout << "Failed to tirnagulate all meshes\n";
    }
   


    // The file is imported, so get rid of the importer.
    lImporter->Destroy();
#pragma endregion
    
#pragma region Model Transform
    Model* new_model = new Model;
    new_model->name = path;
    
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
#pragma endregion 

    //printNode(lScene->GetRootNode());
    
    recursiveReadMeshes(lScene->GetRootNode(), &new_model->meshes);

    //LEGACY LOADING - Models won't have multiple materials on the same mesh
    /* 
    if (lRootNode)
    for (int i = 0; i < lScene->GetNodeCount(); i++)
    {
        for (auto attribIndex = 0; attribIndex < lScene->GetNode(i)->GetNodeAttributeCount(); attribIndex++)
        {
            FbxNodeAttribute* attrib = lScene->GetNode(i)->GetNodeAttributeByIndex(attribIndex);
            switch (attrib->GetAttributeType())
            {
            case FbxNodeAttribute::eMesh: //new_model->meshes.push_back(extractNodeMesh(lScene->GetNode(i))); break;
            case FbxNodeAttribute::eSkeleton: 
                //addToSkellyBoi(SkellyBoi, &Bones, lScene->GetNode(i));
                break;
            case FbxNodeAttribute::eNull: 
                //SkellyBoi = createSkellyboi(lScene->GetNode(i), &Bones);//std::cout << lScene->GetNode(i)->GetName() << "\n";
                //freeSkellyBoi(SkellyBoi);
                break;
            }
        }
    }
    */

    for (auto mesh : new_model->meshes)
        mesh->prepare();

    //WIP
    #pragma region ARMATURE/ANIMATIONS
    new_model->SkellyBoi = createSkellyboi(lScene->GetRootNode(), &(new_model->Bones));
    //std::cout << "Bones count " << new_model->Bones.size() << "\n";

    int numStacks = lScene->GetSrcObjectCount<FbxAnimStack>();//(FBX_TYPE(FbxAnimStack));
    //std::cout << "Anim Count " << numStacks << "\n";

    //std::vector<Animation> animations;
    for (auto i = 0; i < numStacks; i++)
    {
        FbxAnimStack* animStack = FbxCast<FbxAnimStack>(lScene->GetSrcObject<FbxAnimStack>(i));//lScene->GetCurrentAnimationStack();
        //std::cout << animStack->GetName() << "\n";

        int numLayers = animStack->GetMemberCount<FbxAnimLayer>();
        for (int j = 0; j < numLayers; j++)
        {
            FbxAnimLayer* lAnimLayer = animStack->GetMember<FbxAnimLayer>(j);
            
            Animation anim;
            anim.name = lAnimLayer->GetName();

            std::queue<FbxNode*> nodes;
            nodes.push(lScene->GetRootNode());

            while (nodes.size())
            {
                FbxNode* tmpNode = nodes.front();
                nodes.pop();
                
                for (auto k = 0; k < tmpNode->GetChildCount(); k++)
                {
                    nodes.push(tmpNode->GetChild(k));
                }


                for (auto k = 0; k < tmpNode->GetNodeAttributeCount(); k++)
                {

                    FbxNodeAttribute* nodeAttrib = tmpNode->GetNodeAttributeByIndex(k);
                    if (nodeAttrib->GetAttributeType() == FbxNodeAttribute::eSkeleton)
                    {
                        BoneKeyframes keyfr;
                        keyfr.boneName = tmpNode->GetName();
                        //std::cout << tmpNode->GetName() << "\n";

                    #pragma region Translation Keyframes
                        std::vector<std::pair<float, float>> TrX, TrY, TrZ;
                        FbxAnimCurve* lAnimCurveTrX = tmpNode->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
                        FbxAnimCurve* lAnimCurveTrY = tmpNode->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
                        FbxAnimCurve* lAnimCurveTrZ = tmpNode->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);

                        if (!lAnimCurveTrX)
                            std::cout << "X Transl null\n";
                        else
                        {
                            for (auto l = 0; l < lAnimCurveTrX->KeyGetCount(); l++)
                                TrX.push_back(std::make_pair(static_cast<float>(lAnimCurveTrX->KeyGetValue(l)), lAnimCurveTrX->KeyGetTime(l).Get()));
                            
                        }

                        if (!lAnimCurveTrY)
                            std::cout << "Y Transl null\n";
                        else
                        {
                            for (auto l = 0; l < lAnimCurveTrY->KeyGetCount(); l++)
                                TrY.push_back(std::make_pair(static_cast<float>(lAnimCurveTrY->KeyGetValue(l)), lAnimCurveTrY->KeyGetTime(l).Get()));
                            
                        }

                        if (!lAnimCurveTrZ)
                            std::cout << "Z Transl null\n";
                        else
                        {
                            for (auto l = 0; l < lAnimCurveTrZ->KeyGetCount(); l++)
                                TrZ.push_back(std::make_pair(static_cast<float>(lAnimCurveTrZ->KeyGetValue(l)), lAnimCurveTrZ->KeyGetTime(l).Get()));
                        }
                    #pragma endregion

                    #pragma region Rotation Keyframes 
                        std::vector < std::pair<float, signed long>> RoX, RoY, RoZ;
                        FbxAnimCurve* lAnimCurveRoX = tmpNode->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
                        FbxAnimCurve* lAnimCurveRoY = tmpNode->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
                        FbxAnimCurve* lAnimCurveRoZ = tmpNode->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
                        
                        if (!lAnimCurveRoX)
                            std::cout << "X Rot null\n";
                        else
                        {
                            for (auto l = 0; l < lAnimCurveRoX->KeyGetCount(); l++)
                               RoX.push_back(std::make_pair(static_cast<float>(lAnimCurveRoX->KeyGetValue(l)), static_cast<signed long>(lAnimCurveRoX->KeyGetTime(l).Get())));
                        }

                        if (!lAnimCurveRoY)
                            std::cout << "Y Rot null\n";
                        else
                        {
                            for (auto l = 0; l < lAnimCurveRoY->KeyGetCount(); l++)
                                RoY.push_back(std::make_pair(static_cast<float>(lAnimCurveRoY->KeyGetValue(l)), lAnimCurveRoY->KeyGetTime(l).Get()));

                        }

                        if (!lAnimCurveRoZ)
                            std::cout << "Z Rot null\n";
                        else
                        {
                            for (auto l = 0; l < lAnimCurveRoZ->KeyGetCount(); l++)
                                RoZ.push_back(std::make_pair(static_cast<float>(lAnimCurveRoZ->KeyGetValue(l)), lAnimCurveRoZ->KeyGetTime(l).Get()));
                        }
                    #pragma endregion

                    #pragma region Scaling Keyframes
                        std::vector < std::pair<float, signed long>> ScX, ScY, ScZ;
                        FbxAnimCurve* lAnimCurveScX = tmpNode->LclScaling.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
                        FbxAnimCurve* lAnimCurveScY = tmpNode->LclScaling.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
                        FbxAnimCurve* lAnimCurveScZ = tmpNode->LclScaling.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
                        if (!lAnimCurveScX)
                            std::cout << "X Scaling null\n";
                        else
                        {
                            for (auto l = 0; l < lAnimCurveScX->KeyGetCount(); l++)
                                ScX.push_back(std::make_pair(static_cast<float>(lAnimCurveScX->KeyGetValue(l)), static_cast<signed long>(lAnimCurveScX->KeyGetTime(l).Get())));
                        }

                        if (!lAnimCurveScY)
                            std::cout << "Y Scaling null\n";
                        else
                        {
                            for (auto l = 0; l < lAnimCurveScY->KeyGetCount(); l++)
                                ScY.push_back(std::make_pair(static_cast<float>(lAnimCurveScY->KeyGetValue(l)), lAnimCurveScY->KeyGetTime(l).Get()));

                        }

                        if (!lAnimCurveScZ)
                            std::cout << "Z Scaling null\n";
                        else
                        {
                            for (auto l = 0; l < lAnimCurveScZ->KeyGetCount(); l++)
                                ScZ.push_back(std::make_pair(static_cast<float>(lAnimCurveScZ->KeyGetValue(l)), lAnimCurveScZ->KeyGetTime(l).Get()));
                        }
                    #pragma endregion

                        keyfr.TrX = TrX;
                        keyfr.TrY = TrY;
                        keyfr.TrZ = TrZ;

                        keyfr.RoX = RoX;
                        keyfr.RoY = RoY;
                        keyfr.RoZ = RoZ;
                       
                        keyfr.ScX = ScX;
                        keyfr.ScY = ScY;
                        keyfr.ScZ = ScZ;

                        int ID = -1;
                        for (auto bone : new_model->Bones)
                        {
                            if (!strcmp(bone->name.c_str(), keyfr.boneName.c_str()))
                            {
                                ID = bone->ID;
                                break;
                            }
                        }


                        //std::cout << ID << "\n";

                        if (anim.keyframes.size() < ID + 1)
                        {
                            anim.keyframes.resize(ID + 1);
                        }
                        
                        anim.keyframes[ID] = keyfr;

                        break;

                    }
                }

                //std::cout << lAnimCurveTrX->KeyGetCount() << " " << lAnimCurveTrY->KeyGetCount() << " " << lAnimCurveTrZ->KeyGetCount() << "\n";
                
            }
        
            new_model->animations.push_back(anim);
        }
    }
    #pragma endregion
    //WIP


    

     // Destroy the SDK manager and all the other objects it was handling.
    lSdkManager->Destroy();

    return new_model;
}

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

    newMesh->Transform = T * R2 * R3 * R1  *  S;

    int currentIndex = 0;

    FbxGeometryElementNormal* lNormalElement = mesh->GetElementNormal();

    if (lNormalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
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

                newMesh->vertices.push_back(newVert);

                lIndexByPolygonVertex++;
            }
        }
    }
    else {
        delete newMesh;
        std::runtime_error("Add control point method\n");
        }
    return newMesh;

    
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

    new_model->name = path;

    for (auto mesh : new_model->meshes)
        mesh->prepare();

    return new_model;
}
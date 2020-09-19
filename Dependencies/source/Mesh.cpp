#include "Mesh.h"

Mesh* getMeshData(FbxNode* pNode)
{
    const char* nodeName = pNode->GetName();
    FbxDouble3 translation = pNode->LclTranslation.Get();
    FbxDouble3 rotation = pNode->LclRotation.Get();
    FbxDouble3 scaling = pNode->LclScaling.Get();

    glm::mat4 T, R, S;
    S = glm::scale(glm::mat4(1), glm::vec3(scaling[0], scaling[1], scaling[2]));
    T = glm::translate(glm::mat4(1), glm::vec3(translation[0], translation[1], translation[2]));
    R = glm::rotate(glm::mat4(1), glm::radians((float)rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
    R = glm::rotate(           R, glm::radians((float)rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
    R = glm::rotate(           R, glm::radians((float)rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));

    FbxMesh* mesh = pNode->GetMesh();



    Mesh* newMesh = new Mesh;




    newMesh->Transform = T * R * S;

    int currentIndex = 0;

    for (auto i = 0; i < mesh->GetPolygonCount(); i++)
    {
        for (auto j = 0; j < mesh->GetPolygonSize(i); j++)
        {
            int vertIndex = mesh->GetPolygonVertex(i, j);
            newMesh->indices.push_back(currentIndex++);
            Vertex newVert;

            fbxsdk::FbxVector4 Position = mesh->GetControlPointAt(vertIndex);
            newVert.Position = glm::vec3(Position[0], Position[1], Position[2]);

            
            fbxsdk::FbxVector4 Normals;
            mesh->GetPolygonVertexNormal(i, vertIndex, Normals);
            newVert.Normal = glm::vec3(Normals[0], Normals[1], Normals[2]);
          
            newMesh->vertices.push_back(newVert);
            //std::cout << i << " " << j << " " << vertIndex << " " << "\n";


        }
    }

    return newMesh;

    
}

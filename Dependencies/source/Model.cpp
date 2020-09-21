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


// WIP
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

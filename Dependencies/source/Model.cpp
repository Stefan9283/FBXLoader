#include "Model.h"

void Model::Draw(Shader* shader, std::vector<Texture> *textures, std::vector<Material>* materials, glm::mat4 additionanTransform)
{
  
    glm::mat4 transform = additionanTransform * getModelMatrix();
    shader->setMat4("model", &transform);
    for (auto mesh : meshes)
        mesh->Draw(shader, textures, materials);
}

void Model::DrawNoTex(Shader* shader, glm::mat4 additionanTransform)
{
    glm::mat4 transform = additionanTransform * getModelMatrix();
    shader->setMat4("model", &transform);
    for (auto mesh : meshes)
        mesh->DrawNoTex(shader);
}


Model::~Model()
{
    for (auto mesh : meshes)
        delete mesh;
    if (SkellyBoi)
        freeSkellyBoi(SkellyBoi);
}
Model::Model()
{
    SkellyBoi = NULL;
    axis_rotations = glm::vec3(0);
    old_axis_rotations = glm::vec3(0);
    Position = glm::vec3(0.0f);
    Scaling = glm::vec3(1.0f);
    Rotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
}
glm::mat4 Model::getModelMatrix()
{
    glm::mat4 T, R, S;
    T = glm::translate(glm::mat4(1), Position);

    Rotation = glm::rotate(Rotation, glm::radians(axis_rotations.x - old_axis_rotations.x), glm::vec3(1.0f, 0.0f, 0.0f));
    Rotation = glm::rotate(Rotation, glm::radians(axis_rotations.y - old_axis_rotations.y), glm::vec3(0.0f, 1.0f, 0.0f));
    Rotation = glm::rotate(Rotation, glm::radians(axis_rotations.z - old_axis_rotations.z), glm::vec3(0.0f, 0.0f, 1.0f));
    old_axis_rotations = axis_rotations;

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

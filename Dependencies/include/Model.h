#pragma once

#include "Common.h"
#include "Animations.h"
#include "Shader.h"
#include "Mesh.h"



class Model
{
public:
    std::vector<Mesh*> meshes;
    std::string name;
    glm::vec3 Position, Scaling;
    glm::quat Rotation;
    glm::vec3 axis_rotations, old_axis_rotations;
    glm::mat4 getModelMatrix();

    Bone* SkellyBoi;
    std::vector<Bone*> Bones;
    std::vector<Animation> animations;

    void Model::Draw(Shader* shader, std::vector<Texture> *textures, std::vector<Material> *materials, glm::mat4 additionanTransform = glm::mat4(1));

    Model();
    ~Model();

};






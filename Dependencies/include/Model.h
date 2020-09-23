#pragma once

#include "Common.h"
#include "Shader.h"
#include "Mesh.h"



class Model
{
public:
    std::vector<Mesh*> meshes;
    std::string name;
    glm::vec3 Position, Scaling;
    glm::quat Rotation;

    glm::mat4 getModelMatrix();

    void Model::Draw(Shader* shader, std::vector<Texture>* textures);

    Model();
    ~Model();

};






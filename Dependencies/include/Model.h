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


    void Draw(Shader* shader);


    glm::mat4 getModelMatrix();

    ~Model();
};

Mesh* getMeshData(FbxNode* pNode);
Model* ReadFBX(const char* path);



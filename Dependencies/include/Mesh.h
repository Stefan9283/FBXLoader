#pragma once

#include "Common.h"
#include "Shader.h"
#include "Structs.h"


class Mesh
{
public:
    glm::mat4 Transform;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<unsigned int> texIndices;
    bool prepared;
    unsigned int matIndex;

    void Mesh::prepare();
    void Mesh::Draw(Shader* shader, std::vector<Texture> *textures, std::vector<Material> *materials);

    Mesh();
    ~Mesh();

private:
    unsigned int VAO, VBO, EBO;
};

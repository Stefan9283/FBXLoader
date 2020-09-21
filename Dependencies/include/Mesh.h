#pragma once

#include "Common.h"
#include "Shader.h"


struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TextureCoords;
};


class Mesh
{
public:
    glm::mat4 Transform;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    bool prepared;

    void Mesh::prepare();
    void Draw(Shader* shader);
    
    Mesh();
    ~Mesh();

private:
    unsigned int VAO, VBO, EBO;
};

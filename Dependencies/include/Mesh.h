#pragma once

#include "Common.h"

#include "OpenGL/Shader.h"

class Mesh
{
public:
    glm::mat4 Transform;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<unsigned int> texIndices;
    bool prepared;
    unsigned int matIndex;

    void prepare();
    void Draw(Shader* shader, std::vector<Texture> *textures, std::vector<Material> *materials);
    void DrawNoTex(Shader* shader);
    void DrawtoDepthMap(Shader* shader);

    Mesh();
    ~Mesh();

private:
    unsigned int VAO, VBO, EBO;
};

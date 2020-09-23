#pragma once
#include "Common.h"

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TextureCoords;
};


struct Texture
{
    std::string filename;
    unsigned int id;
};
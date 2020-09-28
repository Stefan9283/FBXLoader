#pragma once
#include "Common.h"

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal, Tangent;
    glm::vec2 TextureCoords;
};


struct Texture
{
    std::string filename, type;
    unsigned int id;
};

struct Material
{
    glm::vec3 amb, diff, emiss, spec;
    float shininess;
};

struct Bone
{
    std::string name;
    std::vector<Bone*> children;
    Bone* parent;
};
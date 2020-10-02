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
    unsigned int id, objectCount;
};

struct Material
{
    glm::vec3 amb, diff, emiss, spec;
    float shininess;
};

struct Bone
{
    std::string name;
    unsigned int ID;
    std::vector<Bone*> children;
    glm::mat4 Transform;
    Bone* parent;
};

struct BoneKeyframes
{
    std::string boneName;
    std::vector<std::pair<float, float>> TrX, TrY, TrZ;
    std::vector<std::pair<float, signed long>> RoX, RoY, RoZ;
    std::vector<std::pair<float, signed long>> ScX, ScY, ScZ;

};

struct Animation
{
    std::string name;
    std::vector<BoneKeyframes> keyframes;
};
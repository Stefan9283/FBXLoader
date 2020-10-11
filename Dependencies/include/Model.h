#pragma once

#include "Common.h"
#include "Mesh.h"
#include "OpenGL/Shader.h"
#include "OpenGL/Animations.h"

class Model
{
public:
    std::vector<Mesh*> meshes;
    std::string name;
    glm::vec3 Position, Scaling;
    glm::quat Rotation;
    glm::vec3 axis_rotations, old_axis_rotations;
    
    Model();
    ~Model();

    glm::mat4 getModelMatrix();

    void Draw(Shader* shader, std::vector<Texture> *textures, std::vector<Material> *materials, glm::mat4 additionanTransform = glm::mat4(1));

    void DrawNoTex(Shader* shader, glm::mat4 additionanTransform = glm::mat4(1));


    Bone* SkellyBoi;
    std::vector<Bone*> Bones;
    std::vector<Animation> animations;
};






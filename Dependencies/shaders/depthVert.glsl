#version 330 core
layout (location = 0) in vec3 positions;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;
uniform mat4 mesh_model;

void main()
{
    gl_Position = lightSpaceMatrix * model * mesh_model * vec4(positions, 1.0);
}

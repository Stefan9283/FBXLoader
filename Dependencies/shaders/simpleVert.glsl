#version 330 core
layout(location = 0) in vec3 positions;


uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = proj * view * model * vec4(positions, 1.0f);
};

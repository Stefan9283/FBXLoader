#version 330 core
layout(location = 0) in vec3 positions;
layout(location = 1) in vec3 normals;


uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform mat4 mesh_model;

out vec3 Norm;
out vec3 FragPos;

void main()
{
    vec4 vertPos = model * mesh_model * vec4(positions, 1.0f);
    gl_Position = proj * view * vertPos;
    Norm = mat3(transpose(inverse(model * mesh_model))) * normals;
    FragPos = vec3(vertPos);
};

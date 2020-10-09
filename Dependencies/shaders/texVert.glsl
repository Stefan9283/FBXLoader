#version 330 core
layout(location = 0) in vec3 positions;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec3 tangents;
layout(location = 3) in vec2 texcoords;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform mat4 mesh_model;
uniform vec3 cameraPos;

out vec3 Norm;
out vec3 FragPos;
out vec2 texCoords;

out VS_OUT{
    vec3 FragPos;
    mat3 TBN;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;



out vec3 debug_color;

void main()
{
    vec4 vertPos = model * mesh_model * vec4(positions, 1.0f);


    Norm = mat3(transpose(inverse(model * mesh_model))) * normals;
    FragPos = vec3(vertPos);


    vec3 T = normalize(vec3(model * mesh_model * vec4(tangents, 0.0)));
    vec3 N = normalize(vec3(model * mesh_model * vec4(normals, 0.0)));
    // re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    // then retrieve perpendicular vector B with the cross product of T and N
    vec3 B = cross(N, T);

    vs_out.TBN = transpose(mat3(T, B, N));
    vs_out.FragPos = FragPos;
    vs_out.TangentViewPos = vs_out.TBN * cameraPos;
    vs_out.TangentFragPos = vs_out.TBN * FragPos;

    debug_color = tangents;

    texCoords = texcoords;

    gl_Position = proj * view * vertPos;
};

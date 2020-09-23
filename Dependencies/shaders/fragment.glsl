#version 330 core

in vec3 Norm;
in vec3 FragPos;


struct LightSource
{
    vec3 color;
    vec3 position;
};

uniform vec3 cameraPos;
in vec2 texCoords;

uniform sampler2D texture_diffuse;


void main()
{
    LightSource Light;
    Light.color = vec3(1.0f);
    Light.position = cameraPos; // vec3(0.0f, 5.0f, -10.0f);

    vec3 lightDir = normalize(Light.position - FragPos);
    vec3 norm = normalize(Norm);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, norm);


    gl_FragColor = texture(texture_diffuse, texCoords); // *diff* vec4(Light.color, 1.0f);
};

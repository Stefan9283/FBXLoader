#version 330 core

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform mat4 mesh_model;
uniform vec3 cameraPos;



in VS_OUT{
    vec3 FragPos;
    mat3 TBN;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

struct LightSource
{
    vec3 color;
    vec3 position;
};


uniform sampler2D DiffuseColor[3];
uniform int DiffuseColorCount;

uniform sampler2D NormalMap[3];
uniform int NormalMapCount;

uniform sampler2D ReflectionFactor[3];
uniform int ReflectionFactorCount;

uniform sampler2D ShininessExponent[3];
uniform int ShininessExponentCount;


in vec3 Norm;
in vec3 FragPos;
in vec2 texCoords;

in vec3 debug_color;

uniform vec3 LightPos;

void main()
{
    LightSource Light;
    Light.color = vec3(1.0f);
     // vec3(0.0f, 5.0f, -10.0f);

    vec3 normal = Norm;
    
    if (NormalMapCount > 0)
    {
        Light.position = fs_in.TBN * LightPos;


        normal = texture(NormalMap[0], texCoords).rgb;
        normal = 2.0 * normal - 1.0;
        
        // normal
        vec3 lightDir = normalize(Light.position - fs_in.TangentFragPos);
        vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

        //color
        vec3 color = texture(DiffuseColor[0], texCoords).rgb;

        // ambient
        vec3 ambient = color * 0.1f;

        // diffuse
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = color * diff;

        // specular
        vec3 reflectDir = reflect(-lightDir, normal);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
        vec3 specular = vec3(0.0001) * spec;

        gl_FragColor = vec4(ambient + diffuse + specular, 1.0f);
        //gl_FragColor = vec4(ambient + diffuse + specular, 1.0f);
    }
    else
    { // diffuse only
        Light.position = LightPos;
        vec3 lightDir = normalize(Light.position - FragPos);
        vec3 norm = normalize(Norm);
        float diff = max(dot(norm, lightDir), 0.0);

        //vec3 reflectDir = reflect(-lightDir, norm);
        
        //color
        vec3 color = texture(DiffuseColor[0], texCoords).rgb;

        // diffuse
        vec3 diffuse = color * diff;


        // ambient
        vec3 ambient = color * 0.1f;

        gl_FragColor = vec4(ambient + diffuse, 1.0f);
        
    }

   
    
};

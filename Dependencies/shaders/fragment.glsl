#version 330 core


uniform sampler2D DiffuseColor[3];
uniform int DiffuseColorCount;

uniform sampler2D NormalMap[3];
uniform int NormalMapCount;

uniform sampler2D ReflectionFactor[3];
uniform int ReflectionFactorCount;

uniform sampler2D ShininessExponent[3];
uniform int ShininessExponentCount;

uniform sampler2D SpecularColor[3];
uniform int SpecularColorCount;


uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform mat4 mesh_model;
uniform vec3 cameraPos;

struct LightSource
{
    vec3 color;
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;
    vec3 position;
};

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 emissive;
    vec3 specular;
    float shininess;
};

in VS_OUT{
    vec3 FragPos;
    mat3 TBN;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

in vec3 Norm;
in vec3 FragPos;
in vec2 texCoords;

in vec3 debug_color;

uniform Material material;

uniform LightSource lights[32];
uniform int NUM_OF_LIGHTS;

vec3 calculateColor(LightSource Light)
{


    vec3 lightpos = fs_in.TBN * Light.position;

    vec3 normal = Norm;

    normal = texture(NormalMap[0], texCoords).rgb;
    normal = 2.0 * normal - 1.0;

    // normal
    vec3 lightDir = normalize(lightpos - fs_in.TangentFragPos);
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

    //color
    vec3 color = texture(DiffuseColor[0], texCoords).rgb;

    // ambient
    vec3 ambient = color * Light.ambient;

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = color * diff * Light.diffuse;

    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    vec3 specular = spec * Light.specular;
    
    return (ambient + diffuse + specular) * Light.color;
}

vec3 calculateColorNoTex(LightSource Light)
{


    //Light.position = LightPos;
    vec3 lightDir = normalize(Light.position - FragPos);
    vec3 norm = normalize(Norm);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, norm);


    vec3 color;
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;

    if (DiffuseColorCount <= 0)
    {
        color = material.diffuse;
        diffuse = color * diff;
        ambient = Light.ambient * material.ambient;
    }
    else
    {
        color = texture(DiffuseColor[0], texCoords).rgb;
        diffuse = color * diff;
        ambient = Light.ambient * color;

        float spec = pow(max(dot(norm, reflectDir), 0.0), material.shininess);
        specular = Light.specular * diffuse * spec;
    }

    diffuse = diffuse * Light.diffuse;

    return (ambient + diffuse + specular) * Light.color;
}




vec3 calculateColorNEWFUNC(LightSource Light)
{
    if (NormalMapCount > 0)
    {
        vec3 lightpos = fs_in.TBN * Light.position;

        vec3 normal = Norm;

        normal = texture(NormalMap[0], texCoords).rgb;
        normal = 2.0 * normal - 1.0;

        // normal
        vec3 lightDir = normalize(lightpos - fs_in.TangentFragPos);
        vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

        //color
        vec3 color;
        if (DiffuseColorCount>0)
            color = texture(DiffuseColor[0], texCoords).rgb;
        else color = material.diffuse;

        // ambient
        vec3 ambient = color * Light.ambient;

        // diffuse
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = color * diff * Light.diffuse;

        // specular
        vec3 reflectDir = reflect(-lightDir, normal);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

        vec3 specular = spec * Light.specular;

        return (ambient + diffuse + specular) * Light.color;
    }
    else
    {

        vec3 lightDir = normalize(Light.position - FragPos);
        vec3 norm = normalize(Norm);
        float diff = max(dot(norm, lightDir), 0.0);

        vec3 reflectDir = reflect(-lightDir, norm);

        vec3 color;
        vec3 diffuse;
        vec3 ambient;
        vec3 specular;

        if (DiffuseColorCount > 0)
            color = texture(DiffuseColor[0], texCoords).rgb;
        else color = material.diffuse;

        diffuse = color * diff;
        ambient = Light.ambient * material.ambient;
        float spec = pow(max(dot(norm, reflectDir), 0.0), material.shininess);
        specular = Light.specular * diffuse * spec;

        diffuse = diffuse * Light.diffuse;


        return (ambient + diffuse + specular) * Light.color;
    }

}

void main()
{

    /*
    if (NormalMapCount > 0)
    {

        vec3 result = vec3(0.0f, 0.0f, 0.0f);

        for (int i = 0; i < NUM_OF_LIGHTS; i++)
        {
            result += calculateColor(lights[i]);
        }
       

        gl_FragColor = vec4(result, 1.0f);
    }
    else
    { // diffuse only

        vec3 result = vec3(0.0f, 0.0f, 0.0f);

        for (int i = 0; i < NUM_OF_LIGHTS; i++)
        {
            result += calculateColorNoTex(lights[i]);
        }

        if(NUM_OF_LIGHTS == 0)
            result = material.diffuse;

       
        gl_FragColor = vec4(result, 1.0f);
        
    }*/

    if (NUM_OF_LIGHTS == 0)
        gl_FragColor = vec4(material.diffuse, 1.0f);
    else
    {
        vec3 result = vec3(0.0f, 0.0f, 0.0f);

        for (int i = 0; i < NUM_OF_LIGHTS; i++)
        {
            result += calculateColorNEWFUNC(lights[i]);
        }

        gl_FragColor = vec4(result, 1.0f);
    }


};

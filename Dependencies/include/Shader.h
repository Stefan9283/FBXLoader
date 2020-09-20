#pragma once



#include "Common.h"

typedef struct ShaderProgramSource
{
    char* vertexShader;
    char* fragmentShader;
}ShaderProgramSource;



class Shader
{
public:
	unsigned int id;


    void bind()
    {
        glUseProgram(id);
    }
    void unbind()
    {
        glUseProgram(0);
    }

    void setMat4(const std::string& name, const glm::mat4* mat, int how_many = 1)
    {
        bind();
        int loc;
        loc = glGetUniformLocation(id, name.c_str());
        glUniformMatrix4fv(loc, how_many, GL_FALSE, (const GLfloat*)mat);
    }

    void setVec3(const std::string& name, const glm::vec3 vec)
    {
        bind();
        glUniform3f(glGetUniformLocation(id, name.c_str()), vec.x, vec.y, vec.z);
    }


    Shader(const char* filepath_v, const char* filepath_f)  // new version
    {
        ShaderProgramSource source = ParseShader(filepath_v, filepath_f);
        this->id = CreateShader(source.vertexShader, source.fragmentShader);
        free(source.fragmentShader);
        free(source.vertexShader);
    }
    ~Shader(){}

    // reads the shaders from the files and returns a ShaderProgramSource struct
    ShaderProgramSource Shader::ParseShader(const char* filepath_v, const char* filepath_f);
    
    /* compiles the shader
    - type: GL_VERTEX_SHADER/GL_FRAGMENT_SHADER
    - source: char array containing the source code of the shader
    */
    unsigned int Shader::CompileShader(unsigned int type, const char* source);
    // Links the shaders together and returns the internal id of the group
    unsigned int Shader::CreateShader(const char* vertexShader, const char* fragmentShader);
   
};
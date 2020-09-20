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
        //unbind();
    }

    void setVec3(const std::string& name, const glm::vec3 vec)
    {
        bind();
        glUniform3f(glGetUniformLocation(id, name.c_str()), vec.x, vec.y, vec.z);
        //unbind();
    }


    Shader(const char* filepath_v, const char* filepath_f)  // new version
    {
        ShaderProgramSource source = ParseShader(filepath_v, filepath_f);
        this->id = CreateShader(source.vertexShader, source.fragmentShader);
        free(source.fragmentShader);
        free(source.vertexShader);
    }
    ~Shader(){}

    unsigned int Shader::CompileShader(unsigned int type, const char* source)
    {
        unsigned int id = glCreateShader(type);
        const char* src = source;
        glShaderSource(id, 1, &src, NULL);
        glCompileShader(id);

        int result;
        glGetShaderiv(id, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE)
        {
            int length;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
            char* message = (char*)alloca(length * sizeof(char));
            glGetShaderInfoLog(id, length, &length, message);
            std::cout << message << std::endl;
        }


        return id;
    }
    unsigned int Shader::CreateShader(const char* vertexShader, const char* fragmentShader)
    {
        unsigned int program = glCreateProgram();
        unsigned int vs;
        vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
        unsigned int fs;
        fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);



        int result;
        glGetProgramiv(program, GL_LINK_STATUS, &result);
        if (result == GL_FALSE)
        {
            int infologLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength);
            GLchar buffer[8888];
            GLsizei charsWritten = 0;
            glGetProgramInfoLog(program, infologLength, &charsWritten, buffer);
            std::cout << buffer << std::endl;
        }



        glValidateProgram(program);
        glDeleteShader(vs);
        glDeleteShader(fs);
        return program;
    }
    ShaderProgramSource Shader::ParseShader(const char* filepath_v, const char* filepath_f)
    {
        ShaderProgramSource source;
        source.fragmentShader = (char*)malloc(sizeof(char) * 999999);
        source.vertexShader = (char*)malloc(sizeof(char) * 999999);

        char line[1000];

        FILE* f = fopen(filepath_v, "r");
        memset(source.vertexShader, 0, 999999);
        while (fgets(line, 1000, f))
        {
            line[strlen(line) - 1] = '\0';
            strcat(source.vertexShader, line);
            strcat(source.vertexShader, "\n");
            memset(line, 0, 1000);
        }
        fclose(f);

        f = fopen(filepath_f, "r");
        memset(source.fragmentShader, 0, 999999);
        while (fgets(line, 1000, f))
        {
            line[strlen(line) - 1] = '\0';
            strcat(source.fragmentShader, line);
            strcat(source.fragmentShader, "\n");
            memset(line, 0, 1000);
        }
        fclose(f);

        source.vertexShader[strlen(source.vertexShader)] = '\0';
        source.fragmentShader[strlen(source.fragmentShader)] = '\0';

        return source;
    }
};
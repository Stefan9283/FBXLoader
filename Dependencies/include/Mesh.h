#pragma once

#include "Common.h"

#include "Shader.h"

typedef struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
};





class Mesh
{
public:
    glm::mat4 Transform;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    bool prepared;

    void Mesh::prepare()
    {

        std::cout << "This mesh has\n\t"<< vertices.size() << " vertices\n\t" << indices.size() << " indices\n";

        prepared = true;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        
        glBindVertexArray(0);
    }

    void Draw(Shader *shader)
    {
        shader->setMat4("mesh_model", &Transform);
        shader->bind();
        
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
                
        glBindVertexArray(0);
        shader->unbind();
    }

    Mesh()
    {
        prepared = false;
    }
    ~Mesh()
    {
        if (prepared)
        {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
        }
    }

private:
    unsigned int VAO, VBO, EBO;
};

Mesh* getMeshData(FbxNode* pNode);

class Model
{
public:
    std::vector<Mesh*> meshes;
    std::string name;
    glm::mat4 transform;

    void Draw(Shader* shader)
    {
        for (auto mesh : meshes)
            mesh->Draw(shader);
    }
    ~Model()
    {
        for (auto mesh : meshes)
            delete mesh;
    }
};


Model* ReadFBX(const char* path);



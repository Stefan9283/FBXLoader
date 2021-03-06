#include "Mesh.h"

void Mesh::prepare()
{
    assert(vertices.size() != 0);
    if(SHOWPREPAREFEEDBACK)
        std::cout << "This mesh has\n\t" << vertices.size() << " vertices\n\t" << indices.size() << " indices\n";

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

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TextureCoords));

    glBindVertexArray(0);
}

#define MAX_NUM_OF_TEXTURES 3

void Mesh::Draw(Shader* shader, std::vector<Texture> *textures, std::vector<Material> *materials)
{

    shader->setMat4("mesh_model", &Transform);

    shader->setVec3("material.diffuse", (*materials)[matIndex].diff);
    shader->setVec3("material.emissive", (*materials)[matIndex].emiss);
    shader->setVec3("material.ambient", (*materials)[matIndex].amb);
    shader->setVec3("material.specular", (*materials)[matIndex].spec);
    shader->setFloat("material.shininess", (*materials)[matIndex].shininess);
    std::unordered_map<std::string, int> count;


    // add textures
    unsigned int total = 1; // don't touch texture 0
    for (auto tex_index : texIndices)
    {

        Texture tex = (*textures)[tex_index];

        glActiveTexture(GL_TEXTURE0 + total);
        
        std::string uniform_name = tex.type;
        uniform_name.append("[");
        uniform_name.append(std::to_string(count[tex.type]));
        uniform_name.append("]");
        
        shader->setInt(uniform_name.c_str(), total);

        glBindTexture(GL_TEXTURE_2D, tex.id);

        count[tex.type]++;
        total++;

        if (count[tex.type] > MAX_NUM_OF_TEXTURES)
        {
            std::cout << "Too many textures of type: " << tex.type.c_str() << "\n";
            assert(0);
        }

    }

    // add texture count
    for (auto x : count)
    {
        std::string textures_count_name;
        textures_count_name = x.first;
        textures_count_name.append("Count");
        shader->setInt(textures_count_name.c_str(), x.second);
    }

   
    shader->bind();

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);



    ///std::cout << "Mesh \n";

    ///std::cout << total << " t\n";
    
    // Clean textures
    for (int i = 1; i < total; i++)
    {
        ///std::cout << i << "\n";
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    for (auto x : count)
    {
        std::string textures_count_name;
        textures_count_name = x.first;
        textures_count_name.append("Count");
        shader->setInt(textures_count_name.c_str(), 0);
    }
        

    ///std::cout << "\n";

    shader->unbind();
}
void Mesh::DrawNoTex(Shader* shader)
{
    shader->setMat4("mesh_model", &Transform);
    shader->bind();

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    shader->unbind();
}


Mesh::Mesh()
{
    matIndex = -1;
    VAO = 0;
    VBO = 0;
    EBO = 0;
    prepared = false;
    Transform = glm::mat4(1);
}
Mesh::~Mesh()
{
    if (prepared)
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
}
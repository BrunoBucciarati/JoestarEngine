#pragma once
#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Material.h"

#include <string>
#include <vector>
using namespace std;

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh {
public:
    // mesh Data
    vector<Vertex>       vertices_;
    vector<unsigned int> indices_;
    vector<Texture>      textures_;
    unsigned int cachedVAO;
    bool cachedDirty;

    // constructor
    Mesh():
        cachedDirty(true),
        cachedVAO(0)
    {
        //this->vertices_ = vertices;
        //this->indices_ = indices;
        //this->textures_ = textures;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        //SetupMesh();
    }

    // render the mesh
    void Draw()
    {
        //UpdateMaterial();
        // bind appropriate textures
        //unsigned int diffuseNr = 1;
        //unsigned int specularNr = 1;
        //unsigned int normalNr = 1;
        //unsigned int heightNr = 1;
        //for (unsigned int i = 0; i < textures.size(); i++)
        //{
        //    glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        //    // retrieve texture number (the N in diffuse_textureN)
        //    string number;
        //    string name = textures[i].type;
        //    if (name == "texture_diffuse")
        //        number = std::to_string(diffuseNr++);
        //    else if (name == "texture_specular")
        //        number = std::to_string(specularNr++); // transfer unsigned int to stream
        //    else if (name == "texture_normal")
        //        number = std::to_string(normalNr++); // transfer unsigned int to stream
        //    else if (name == "texture_height")
        //        number = std::to_string(heightNr++); // transfer unsigned int to stream

        //    // now set the sampler to the correct texture unit
        //    glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
        //    // and finally bind the texture
        //    glBindTexture(GL_TEXTURE_2D, textures[i].id);
        //}

        //// draw mesh
        //glBindVertexArray(VAO);
        //glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        //glBindVertexArray(0);

        //// always good practice to set everything back to defaults once configured.
        //glActiveTexture(GL_TEXTURE0);
    }

private:
    // render data 
};
#endif
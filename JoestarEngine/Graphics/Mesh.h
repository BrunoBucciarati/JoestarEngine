#pragma once
#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>
#include "VertexData.h"

namespace Joestar {

class Mesh {
public:
    // mesh Data
    //std::vector<Vertex>       vertices;
    //std::vector<unsigned int> indices;
    //std::vector<Texture>      textures;
    VertexBuffer* vb;
    IndexBuffer* ib;
    unsigned int cachedVAO;
    bool cachedDirty;

    // constructor
    ~Mesh()
    {
        delete vb;
        delete ib;
    }

    inline VertexBuffer* GetVB() { return vb; }
    inline IndexBuffer* GetIB() { return ib; }

    void Load(std::string path);

    // render the mesh
    void Draw()
    {
    }

private:
    // render data 
};
}
#endif
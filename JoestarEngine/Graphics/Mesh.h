#pragma once
#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>
#include <map>
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

    VertexBuffer* GetVB() { return vb; }
    VertexBuffer* GetVB(uint32_t flag);
    IndexBuffer* GetIB() { return ib; }

    void Load(std::string path);

private:
    // render data 
    std::map<uint32_t, VertexBuffer*> CustomVBs;
};
}
#endif
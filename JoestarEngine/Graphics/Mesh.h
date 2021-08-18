#pragma once
#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>
#include "VertexData.h"

namespace Joestar {
struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    // mesh Data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;
    unsigned int cachedVAO;
    bool cachedDirty;

    // constructor
    Mesh()
    {
    }

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
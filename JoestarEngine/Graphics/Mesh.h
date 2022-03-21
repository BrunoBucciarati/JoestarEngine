#pragma once
#ifndef MESH_H
#define MESH_H

#include "../Container/Str.h"
#include "../Container/HashMap.h"
#include "VertexData.h"
#include "../Base/Object.h"

namespace Joestar {
    enum MeshTopology {
        MESH_TOPOLOGY_TRIANGLE = 0,
        MESH_TOPOLOGY_TRIANGLE_STRIP,
        MESH_TOPOLOGY_LINE,
        MESH_TOPOLOGY_LINE_STRIP,
    };
    struct VertexLoad {
        Vector3f pos;
        Vector3f normal;
        Vector3f color;
        Vector2f texCoord;

        bool operator ==(const VertexLoad& v) {
            return pos == v.pos && color == v.color && texCoord == v.texCoord;
        }
    };

class Mesh : public Object {
    REGISTER_OBJECT(Mesh, Object)
public:
    VertexBuffer* vb;
    IndexBuffer* ib;
    InstanceBuffer* instanceBuffer;
    unsigned int cachedVAO;
    bool cachedDirty;
    explicit Mesh(EngineContext* ctx);

    VertexBuffer* GetVB() { return vb; }
    VertexBuffer* GetVB(uint32_t flag);
    IndexBuffer* GetIB() { return ib; }

    MeshTopology GetTopology() {
        return mTopology;
    }

    void SetTopology(MeshTopology t) {
        mTopology = t;
    }

    void Load(String path);

private:
    // render data 
    HashMap<uint32_t, VertexBuffer*> customVBs;
    MeshTopology mTopology;
};
}
#endif
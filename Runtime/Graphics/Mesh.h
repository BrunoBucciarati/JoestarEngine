#pragma once
#ifndef MESH_H
#define MESH_H

#include "../Container/Str.h"
#include "../Container/HashMap.h"
//#include "VertexData.h"
#include "../Core/Object.h"
#include "../Math/Vector2.h"
#include "../Math/AABB.h"
#include "../Resource/Resource.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace Joestar {

class Mesh : public Resource {
    REGISTER_OBJECT(Mesh, Resource)
public:
    explicit Mesh(EngineContext* ctx);

    VertexBuffer* GetVertexBuffer() { return mVertexBuffer; }
    IndexBuffer* GetIndexBuffer() { return mIndexBuffer; }
    void SetVertexBuffer(VertexBuffer* vb)
    {
        mVertexBuffer = vb;
    }
    void SetIndexBuffer(IndexBuffer* ib)
    {
        mIndexBuffer = ib;
    }
    U32 GetIndexCount()
    {
        return mIndexBuffer->GetIndexCount();
    }

    AABB& GetBoundingBox()
    {
        if (!mBoundingBox.IsValid())
        {
            BuildBoundingBox();
        }
        return mBoundingBox;
    }

    void BuildBoundingBox();

    bool LoadFile(const String& path);

private:
    // render data 
    SharedPtr<VertexBuffer> mVertexBuffer;
    SharedPtr<IndexBuffer> mIndexBuffer;
    AABB mBoundingBox;
};
}
#endif
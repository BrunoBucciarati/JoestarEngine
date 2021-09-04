#pragma once
#include "../Math/Vector3.h"
#include "../Math/Vector2.h"
#include "VulkanHeader.h"
#include "GraphicDefines.h"
#include "../Base/ObjectDefines.h"
#include <vector>

namespace Joestar {
    class VertexBuffer {
    public:
        VertexBuffer();
        void SetFlag(U32 f) { flags = f; }
        U32 GetFlag() { return flags; }
        void CopyBuffer(U8* data, U32 sz) {
            if (!buffer) SetSize(sz);
            memcpy(buffer, data, sz);
        }
        U8* GetBuffer() { return buffer; }
        void SetSize(U32 s) {
            size = s;
            buffer = new U8[size];
        }
        U32 GetSize() { return size; }
        U32 GetVertexCount();
        void AppendVertexAttr(VERTEX_ATTRIBUTE v);
        ~VertexBuffer() {
            delete buffer;
        }
        U32 id;
        U32 flags; //flag of vertex channel
        U8* buffer; //raw data
        U32 size; //size of data
        U32 instanceCount;
        std::vector<VERTEX_ATTRIBUTE> attrs; //size of vertex channels
    };

    class InstanceBuffer : public VertexBuffer {
    public:
        InstanceBuffer();
        void SetCount(U32 count) { instanceCount = count; }
        U32 GetCount() { return instanceCount; }
        void PrepareInstanceData(U32 flag);
        void SetElementData(U8* data, U32 idx) {
            memcpy(buffer + idx * elementSize, data, elementSize);
        }
        ~InstanceBuffer() {
            delete buffer;
        }
        U32 elementSize;
    };

    class IndexBuffer {
    public:
        IndexBuffer();
        U8* GetBuffer() { return buffer; }
        void CopyBuffer(U8* data, U32 sz) {
            if (!buffer) SetSize(sz);
            memcpy(buffer, data, sz);
        }
        void SetSize(U32 s) { size = s; buffer = new U8[size]; }
        U32 GetSize() { return size; }
        U32 GetIndexCount() { return size / 2; }
        ~IndexBuffer() {
            delete buffer;
        }
        U32 id;
    private:
        U8* buffer;
        U32 size = 0; //size of data
    };

    class ComputeBuffer {

    };
}
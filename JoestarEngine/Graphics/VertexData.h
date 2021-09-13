#pragma once
#include "../Math/Vector3.h"
#include "../Math/Vector2.h"
#include "VulkanHeader.h"
#include "GraphicDefines.h"
#include "../Base/ObjectDefines.h"
#include <vector>
#include "../Base/StringHash.h"

namespace Joestar {
    class BaseBuffer {
    public:
        ~BaseBuffer() {
            delete buffer;
        }
        U32 GetSize() { return size; }
        void SetSize(U32 s) {
            size = s;
            buffer = new U8[size];
            memset(buffer, 0, size);
        }
        void CopyBuffer(U8* data, U32 sz) {
            if (!buffer) SetSize(sz);
            memcpy(buffer, data, sz);
        }
        U8* GetBuffer() { return buffer; }
        U32 id;
        U8* buffer; //raw data
        U32 size; //size of data
    };

    class VertexBuffer : public BaseBuffer {
    public:
        VertexBuffer();
        void SetFlag(U32 f) { flags = f; }
        U32 GetFlag() { return flags; }
        U32 GetVertexCount();
        void AppendVertexAttr(VERTEX_ATTRIBUTE v);
        U32 flags; //flag of vertex channel
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
        U32 elementSize;
    };

    class IndexBuffer : public BaseBuffer {
    public:
        IndexBuffer();
        U32 GetIndexCount() { return size / 2; }
    };

    class ComputeBuffer : public BaseBuffer {
    public:
        ComputeBuffer(const char* name);
        void SetName(std::string& n) { name = n; id = hashString(name.c_str()); }
        void SetName(const char* n) { name = n; id = hashString(name.c_str()); }

    private:
        std::string name;
    };
}
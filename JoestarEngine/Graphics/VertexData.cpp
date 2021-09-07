#include "VertexData.h"
namespace Joestar {
	static U32 gVBID = 0;
	VertexBuffer::VertexBuffer() {
		id = ++gVBID;
		instanceCount = 0;
	}
	void VertexBuffer::AppendVertexAttr(VERTEX_ATTRIBUTE v) {
		flags = flags | 1 << v;
		attrs.push_back(v);
	}

	U32 VertexBuffer::GetVertexCount() {
		U32 elementSize = 0;
		for (int i = 0; i < VERTEX_ATTRIB_COUNT; ++i) {
			if (flags & 1 << i) {
				elementSize += VERTEX_ATTRIBUTE_SIZE[i] * sizeof(float);
			}
		}

		return size / elementSize;
	}

	static U32 gIBID = 0;
	IndexBuffer::IndexBuffer() {
		id = ++gIBID;
	}

	InstanceBuffer::InstanceBuffer() {
		//instance as vb buffer
		id = ++gVBID;
		instanceCount = 1;
	}

	void InstanceBuffer::PrepareInstanceData(U32 flag) {
		SetFlag(flag);
		elementSize = 0;
		for (int i = 0; i < VERTEX_ATTRIB_COUNT; ++i) {
			if (flag & 1 << i) {
				elementSize += VERTEX_ATTRIBUTE_SIZE[i] * sizeof(float);
				attrs.push_back((VERTEX_ATTRIBUTE)i);
			}
		}
		SetSize(elementSize * instanceCount);
	}

	ComputeBuffer::ComputeBuffer(const char* name) {
		SetName(name);
	}

}
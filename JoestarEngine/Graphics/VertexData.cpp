#include "VertexData.h"
namespace Joestar {
	static U32 gVBID = 0;
	VertexBuffer::VertexBuffer() {
		id = ++gVBID;
	}
	void VertexBuffer::AppendVertexAttr(VERTEX_ATTRIBUTE v) {
		flags = flags | 1 << v;
		attrs.push_back(v);
	}

	static U32 gIBID = 0;
	IndexBuffer::IndexBuffer() {
		id = ++gIBID;
	}
}
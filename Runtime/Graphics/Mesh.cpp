#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include "Mesh.h"
#include "../IO/Log.h"
#include "../Math/Vector3.h"
#include "../Math/Vector2.h"
#include "../Container/Vector.h"
#include "../Container/Str.h"
#include <algorithm>

namespace Joestar {
    Mesh::Mesh(EngineContext* ctx) : Super(ctx)
    {
    }

    Mesh::~Mesh() {
    }
 
	bool Mesh::LoadFile(const String& path) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.CString())) {
            LOGERROR((warn + err).c_str());
        }
        Vector<U16> indices;
        Vector<float> vertices;

        int count = 0;
        for (const tinyobj::shape_t& shape : shapes) {
            for (const tinyobj::index_t& index : shape.mesh.indices) {
                vertices.Push(attrib.vertices[3 * index.vertex_index + 0]);
                vertices.Push(attrib.vertices[3 * index.vertex_index + 1]);
                vertices.Push(attrib.vertices[3 * index.vertex_index + 2]);
                //color
                vertices.Push(1.f);
                vertices.Push(1.f);
                vertices.Push(1.f);
                vertices.Push(attrib.normals[3 * index.normal_index + 0]);
                vertices.Push(attrib.normals[3 * index.normal_index + 1]);
                vertices.Push(attrib.normals[3 * index.normal_index + 2]);
                vertices.Push(attrib.texcoords[2 * index.texcoord_index + 0]);
                vertices.Push(1.F - attrib.texcoords[2 * index.texcoord_index + 1]);
            }
        }
        SharedPtr<VertexBuffer> vb(JOJO_NEW(VertexBuffer(mContext), MEMORY_GFX_STRUCT));
        SharedPtr<IndexBuffer> ib(JOJO_NEW(IndexBuffer(mContext), MEMORY_GFX_STRUCT));

        PODVector<VertexElement> elements;
        elements.Push({ VertexSemantic::POSITION, VertexType::VEC3 });
        elements.Push({ VertexSemantic::NORMAL, VertexType::VEC3 });
        elements.Push({ VertexSemantic::COLOR, VertexType::VEC3 });
        elements.Push({ VertexSemantic::TEXCOORD0, VertexType::VEC2 });
        vb->SetSize(vertices.Size() * sizeof(float), elements);
        vb->SetData((U8*)vertices.Buffer());

        ib->SetSize(indices.Size() * sizeof(U16));
        ib->SetData((U8*)indices.Buffer());

        mVertexBuffer = vb;
        mIndexBuffer = ib;
        return true;
	}
}
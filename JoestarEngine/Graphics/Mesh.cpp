#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include "Mesh.h"
#include "../IO/Log.h"
#include "../Math/Vector3.h"
#include "../Math/Vector2.h"
#include <map>
#include <algorithm>

namespace Joestar {
    struct Vertex {
        Vector3f pos;
        Vector3f color;
        Vector2f texCoord;
    };
	void Mesh::Load(std::string path) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
            LOGERROR((warn + err).c_str());
        }
        std::vector<uint16_t> indices;
        std::vector<Vertex> vertices;
        for (const tinyobj::shape_t& shape : shapes) {
            for (const tinyobj::index_t& index : shape.mesh.indices) {
                Vertex vertex{};
                vertex.pos.Set(attrib.vertices[3 * index.vertex_index + 0], attrib.vertices[3 * index.vertex_index + 1], attrib.vertices[3 * index.vertex_index + 2]);
                vertex.color.Set(1.f, 1.f, 1.f);
                vertex.texCoord.Set(attrib.texcoords[2 * index.texcoord_index + 0], 1.0f - attrib.texcoords[2 * index.texcoord_index + 1]);
                    vertices.push_back(vertex);
                    indices.push_back(vertices.size());
                //std::vector<Vertex>::iterator res = std::find(vertices.begin(), vertices.end(), vertex);
                //if (res == vertices.end()) {
                //} else {
                //    indices.push_back(res - vertices.begin());
                //}

            }
        }

        vb = new VertexBuffer;
        vb->AppendVertexAttr(VERTEX_POS);
        vb->AppendVertexAttr(VERTEX_COLOR);
        vb->AppendVertexAttr(VERTEX_TEXCOORD);
        vb->SetSize(vertices.size() * sizeof(Vertex));
        memcpy(vb->GetBuffer(), vertices.data(), vertices.size() * sizeof(Vertex));

        ib = new IndexBuffer;
        ib->SetSize(indices.size() * sizeof(uint16_t));
        memcpy(ib->GetBuffer(), indices.data(), indices.size() * sizeof(uint16_t));
	}
}
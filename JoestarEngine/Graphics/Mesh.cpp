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
        Vector3f normal;
        Vector3f color;
        Vector2f texCoord;

        bool operator ==(const Vertex v) {
            return pos == v.pos && color == v.color && texCoord == v.texCoord;
        }
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

        int count = 0;
        for (const tinyobj::shape_t& shape : shapes) {
            for (const tinyobj::index_t& index : shape.mesh.indices) {
                Vertex vertex{};
                vertex.pos.Set(attrib.vertices[3 * index.vertex_index + 0], attrib.vertices[3 * index.vertex_index + 1], attrib.vertices[3 * index.vertex_index + 2]);
                vertex.color.Set(1.f, 1.f, 1.f);
                vertex.normal.Set(attrib.normals[3 * index.normal_index + 0], attrib.normals[3 * index.normal_index + 1], attrib.normals[3 * index.normal_index + 2]);
                vertex.texCoord.Set(attrib.texcoords[2 * index.texcoord_index + 0], 1.f - attrib.texcoords[2 * index.texcoord_index + 1]);
                std::vector<Vertex>::const_iterator it = std::find(vertices.begin(), vertices.end(), vertex);
                if (it == vertices.end()) {
                    indices.push_back(vertices.size());
                    vertices.push_back(vertex);
                }
                else {
                    indices.push_back(it - vertices.begin());
                }
                //for test
                ++count;
                if (count == 300000) break;
            }
        }

        vb = new VertexBuffer;
        vb->AppendVertexAttr(VERTEX_POS);
        vb->AppendVertexAttr(VERTEX_NORMAL);
        vb->AppendVertexAttr(VERTEX_COLOR);
        vb->AppendVertexAttr(VERTEX_TEXCOORD);
        vb->SetSize(vertices.size() * sizeof(Vertex));
        memcpy(vb->GetBuffer(), vertices.data(), vertices.size() * sizeof(Vertex));

        ib = new IndexBuffer;
        ib->SetSize(indices.size() * sizeof(uint16_t));
        memcpy(ib->GetBuffer(), indices.data(), indices.size() * sizeof(uint16_t));
	}

    VertexBuffer* Mesh::GetVB(uint32_t flag) {
        if (vb->GetFlag() == flag) {
            return vb;
        }
        if (CustomVBs.find(flag) != CustomVBs.end()) {
            return CustomVBs[flag];
        }
        CustomVBs[flag] = new VertexBuffer();

        uint32_t oldSize = 0, newSize = 0;
        bool oldHas, newHas;
        for (int i = 0; i < VERTEX_ATTRIB_COUNT; ++i) {
            oldHas = vb->GetFlag() & 1 << i;
            if (oldHas) {
                oldSize += VERTEX_ATTRIBUTE_SIZE[i];
            }
            if (flag & 1 << i) {
                newSize += VERTEX_ATTRIBUTE_SIZE[i];
                CustomVBs[flag]->AppendVertexAttr((VERTEX_ATTRIBUTE)i);
            }
        }
        newSize *= sizeof(float);
        oldSize *= sizeof(float);
        int vertexCount = vb->GetSize() / (oldSize);
        CustomVBs[flag]->SetSize(newSize * vertexCount);
        uint8_t* newBuffer = CustomVBs[flag]->GetBuffer();
        uint8_t* oldBuffer = vb->GetBuffer();
        uint32_t oldOffset = 0, newOffset = 0, attribSize = 0;

        for (int i = 0; i < VERTEX_ATTRIB_COUNT; ++i) {
            oldHas = vb->GetFlag() & 1 << i;
            attribSize = VERTEX_ATTRIBUTE_SIZE[i] * sizeof(float);
            newHas = flag & 1 << i;
            if (newHas) {
                for (int j = 0; j < vertexCount; ++j) {
                    if (oldHas) {
                        memcpy(newBuffer + j * newSize + newOffset, oldBuffer + j * oldSize + oldOffset, attribSize);
                    } else {
                        memset(newBuffer + j * newSize + newOffset, 0, attribSize);
                    }
                }
                newOffset += attribSize;
            }
            if (oldHas)
                oldOffset += attribSize;
        }

        return CustomVBs[flag];
    }
}
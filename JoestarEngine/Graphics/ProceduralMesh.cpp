#include "ProceduralMesh.h"

namespace Joestar {
    Mesh* ProceduralMesh::GenUVSphere() {
        Mesh* mesh = NEW_OBJECT(Mesh);
        std::vector<VertexLoad> vertices;
        std::vector<uint16_t> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359;
        vertices.resize((X_SEGMENTS + 1) * (Y_SEGMENTS + 1));
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                vertices[y * (X_SEGMENTS + 1) + x].pos.Set(xPos, yPos, zPos);
                vertices[y * (X_SEGMENTS + 1) + x].normal.Set(xPos, yPos, zPos);
                vertices[y * (X_SEGMENTS + 1) + x].texCoord.Set(xSegment, ySegment);
                vertices[y * (X_SEGMENTS + 1) + x].color.Set(1.f, 1.f, 1.f);
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
            if (!oddRow) {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else {
                for (int x = X_SEGMENTS; x >= 0; --x) {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }

        VertexBuffer* vb = mesh->GetVB();
        vb->SetSize(vertices.size() * sizeof(VertexLoad));
        vb->CopyBuffer((uint8_t*)vertices.data(), vertices.size() * sizeof(VertexLoad));
        vb->AppendVertexAttr(VERTEX_POS);
        vb->AppendVertexAttr(VERTEX_NORMAL);
        vb->AppendVertexAttr(VERTEX_COLOR);
        vb->AppendVertexAttr(VERTEX_TEXCOORD);


        IndexBuffer* ib = mesh->GetIB();
        ib->SetSize(indices.size() * sizeof(uint16_t));
        ib->CopyBuffer((uint8_t*)indices.data(), indices.size() * sizeof(uint16_t));

        mesh->SetTopology(MESH_TOPOLOGY_TRIANGLE_STRIP);

        return mesh;
    }

    Mesh* ProceduralMesh::GenPlaneSphere() {
        Mesh* mesh = NEW_OBJECT(Mesh);
        std::vector<VertexLoad> vertices;
        std::vector<uint16_t> indices;

        const unsigned int X_SEGMENTS = 16;
        const unsigned int Y_SEGMENTS = 16;
        vertices.resize(X_SEGMENTS * Y_SEGMENTS);
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
            for (unsigned int x = 0; x < X_SEGMENTS; ++x) {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = 10 * xSegment - 5;
                float yPos = 0;
                float zPos = 10 * ySegment - 5;
                vertices[y * X_SEGMENTS + x].pos.Set(xPos, yPos, zPos);
                vertices[y * X_SEGMENTS + x].normal.Set(0, 1, 0);
                vertices[y * X_SEGMENTS + x].texCoord.Set(xSegment, ySegment);
                vertices[y * X_SEGMENTS + x].color.Set(1.f, 1.f, 1.f);
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS - 1; ++y) {
            if (!oddRow) {
                for (unsigned int x = 0; x < X_SEGMENTS; ++x) {
                    indices.push_back(y * X_SEGMENTS + x);
                    indices.push_back((y + 1) * X_SEGMENTS + x);
                }
            }
            else {
                for (int x = X_SEGMENTS - 1; x >= 0; --x) {
                    indices.push_back((y + 1) * X_SEGMENTS + x);
                    indices.push_back(y * X_SEGMENTS + x);
                }
            }
            oddRow = !oddRow;
        }

        VertexBuffer* vb = mesh->GetVB();
        vb->SetSize(vertices.size() * sizeof(VertexLoad));
        vb->CopyBuffer((uint8_t*)vertices.data(), vertices.size() * sizeof(VertexLoad));
        vb->AppendVertexAttr(VERTEX_POS);
        vb->AppendVertexAttr(VERTEX_NORMAL);
        vb->AppendVertexAttr(VERTEX_COLOR);
        vb->AppendVertexAttr(VERTEX_TEXCOORD);


        IndexBuffer* ib = mesh->GetIB();
        ib->SetSize(indices.size() * sizeof(uint16_t));
        ib->CopyBuffer((uint8_t*)indices.data(), indices.size() * sizeof(uint16_t));

        mesh->SetTopology(MESH_TOPOLOGY_TRIANGLE_STRIP);

        return mesh;
    }

	ProceduralMesh::ProceduralMesh(EngineContext* ctx) : Super(ctx) {
        uvSphere = GenUVSphere();
        planeSphere = GenPlaneSphere();
	}
}
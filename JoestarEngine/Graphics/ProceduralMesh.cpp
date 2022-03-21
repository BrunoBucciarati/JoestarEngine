#include "ProceduralMesh.h"

namespace Joestar {
    ProceduralMesh::~ProceduralMesh(){}
    Mesh* ProceduralMesh::GenUVSphere() {
        Mesh* mesh = NEW_OBJECT(Mesh);
        Vector<VertexLoad> vertices;
        Vector<uint16_t> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359;
        vertices.Resize((X_SEGMENTS + 1) * (Y_SEGMENTS + 1));
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = Cos(xSegment * 2.0f * PI) * Sin(ySegment * PI);
                float yPos = Cos(ySegment * PI);
                float zPos = Sin(xSegment * 2.0f * PI) * Sin(ySegment * PI);
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
                    indices.Push(y * (X_SEGMENTS + 1) + x);
                    indices.Push((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else {
                for (int x = X_SEGMENTS; x >= 0; --x) {
                    indices.Push((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.Push(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }

        VertexBuffer* vb = mesh->GetVB();
        vb->SetSize(vertices.Size() * sizeof(VertexLoad));
        vb->CopyBuffer((uint8_t*)vertices.Buffer(), vertices.Size() * sizeof(VertexLoad));
        vb->AppendVertexAttr(VERTEX_POS);
        vb->AppendVertexAttr(VERTEX_NORMAL);
        vb->AppendVertexAttr(VERTEX_COLOR);
        vb->AppendVertexAttr(VERTEX_TEXCOORD);


        IndexBuffer* ib = mesh->GetIB();
        ib->SetSize(indices.Size() * sizeof(uint16_t));
        ib->CopyBuffer((uint8_t*)indices.Buffer(), indices.Size() * sizeof(uint16_t));

        mesh->SetTopology(MESH_TOPOLOGY_TRIANGLE_STRIP);

        return mesh;
    }


    Mesh* ProceduralMesh::GenPlane() {
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
                vertices[y * X_SEGMENTS + x].color.Set(.7f, .7f, .7f);
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

    Mesh* ProceduralMesh::GenLine() {
        Mesh* mesh = NEW_OBJECT(Mesh);
        std::vector<float> attrs = {
            0.f, 0.f, 0.f, 1.f, 1.f, 1.f,
            0.f, 0.f, 10.f, 1.f, 1.f, 1.f
        };
        VertexBuffer* vb = mesh->GetVB();
        vb->SetSize(attrs.size() * sizeof(float));
        vb->CopyBuffer((uint8_t*)attrs.data(), attrs.size() * sizeof(float));
        vb->AppendVertexAttr(VERTEX_POS);
        vb->AppendVertexAttr(VERTEX_COLOR);

        mesh->SetTopology(MESH_TOPOLOGY_LINE);

        return mesh;
    }

	ProceduralMesh::ProceduralMesh(EngineContext* ctx) : Super(ctx) {
        uvSphere = GenUVSphere();
        plane = GenPlane();
        line = GenLine();
	}
}
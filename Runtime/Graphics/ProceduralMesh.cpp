#include "ProceduralMesh.h"

namespace Joestar {
    ProceduralMesh::~ProceduralMesh(){}
    Mesh* ProceduralMesh::GenUVSphere() {
        Mesh* mesh = NEW_OBJECT(Mesh);
        PODVector<VertexElement> elements;        
        elements.Push({ VertexSemantic::POSITION, VertexType::VEC3 });
        elements.Push({ VertexSemantic::NORMAL, VertexType::VEC3 });
        elements.Push({ VertexSemantic::COLOR, VertexType::VEC3 });
        elements.Push({ VertexSemantic::TEXCOORD0, VertexType::VEC2 });

        Vector<float> vertices;
        Vector<U16> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359;
        vertices.Reserve((X_SEGMENTS + 1) * (Y_SEGMENTS + 1) * 11);
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = Cos(xSegment * 2.0f * PI) * Sin(ySegment * PI);
                float yPos = Cos(ySegment * PI);
                float zPos = Sin(xSegment * 2.0f * PI) * Sin(ySegment * PI);
                vertices.Push(xPos);
                vertices.Push(yPos);
                vertices.Push(zPos);
                vertices.Push(xPos);
                vertices.Push(yPos);
                vertices.Push(zPos);
                vertices.Push(1.F);
                vertices.Push(1.F);
                vertices.Push(1.F);
                vertices.Push(xSegment);
                vertices.Push(1.0 - ySegment);
            }
        }

        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
                indices.Push(y * (X_SEGMENTS + 1) + x);
                indices.Push((y + 1) * (X_SEGMENTS + 1) + x + 1);
                indices.Push((y + 1) * (X_SEGMENTS + 1) + x);
                indices.Push(y * (X_SEGMENTS + 1) + x);
                indices.Push(y * (X_SEGMENTS + 1) + x + 1);
                indices.Push((y + 1) * (X_SEGMENTS + 1) + x + 1);
            }
        }

        VertexBuffer* vb = NEW_OBJECT(VertexBuffer);
        vb->SetSize(vertices.Size() / 11, elements);
        vb->SetData((U8*)vertices.Buffer());
        mesh->SetVertexBuffer(vb);

        IndexBuffer* ib = NEW_OBJECT(IndexBuffer);
        ib->SetSize(indices.Size());
        ib->SetData((U8*)indices.Buffer());
        mesh->SetIndexBuffer(ib);


        return mesh;
    }


    Mesh* ProceduralMesh::GenPlane() {
        Mesh* mesh = NEW_OBJECT(Mesh);
        Vector<float> vertices;
        Vector<U16> indices;
        PODVector<VertexElement> elements;
        elements.Push({ VertexSemantic::POSITION, VertexType::VEC3 });
        elements.Push({ VertexSemantic::NORMAL, VertexType::VEC3 });
        elements.Push({ VertexSemantic::COLOR, VertexType::VEC3 });
        elements.Push({ VertexSemantic::TEXCOORD0, VertexType::VEC2 });

        const unsigned int X_SEGMENTS = 16;
        const unsigned int Y_SEGMENTS = 16;
        vertices.Reserve(X_SEGMENTS * Y_SEGMENTS * 11);
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            for (unsigned int x = 0; x < X_SEGMENTS; ++x)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = 10 * xSegment - 5;
                float yPos = 0;
                float zPos = 10 * ySegment - 5;
                vertices.Push(xPos);
                vertices.Push(yPos);
                vertices.Push(zPos);
                vertices.Push(0.F);
                vertices.Push(1.F);
                vertices.Push(0.F);
                vertices.Push(.7F);
                vertices.Push(.7F);
                vertices.Push(.7F);
                vertices.Push(xSegment);
                vertices.Push(1.F - ySegment);
            }
        }

        for (U32 y = 0; y < Y_SEGMENTS - 1; ++y)
        {
            for (U32 x = 0; x < X_SEGMENTS - 1; ++x)
            {
                indices.Push(y * X_SEGMENTS + x);
                indices.Push((y + 1) * X_SEGMENTS + x);
                indices.Push(y * X_SEGMENTS + x + 1);
                indices.Push(y * X_SEGMENTS + x + 1);
                indices.Push((y + 1) * X_SEGMENTS + x);
                indices.Push((y + 1) * X_SEGMENTS + x + 1);
            }
        }

        VertexBuffer* vb = NEW_OBJECT(VertexBuffer);
        vb->SetSize(vertices.Size() / 11, elements);
        vb->SetData((U8*)vertices.Buffer());
        mesh->SetVertexBuffer(vb);

        IndexBuffer* ib = NEW_OBJECT(IndexBuffer);
        ib->SetSize(indices.Size());
        ib->SetData((U8*)indices.Buffer());
        mesh->SetIndexBuffer(ib);

        return mesh;
    }

    Mesh* ProceduralMesh::GenLine() {
        Mesh* mesh = NEW_OBJECT(Mesh);

        return mesh;
    }

    Mesh* ProceduralMesh::GenTriangle() {
        Mesh* mesh = NEW_OBJECT(Mesh);
        Vector<F32> vertices = {
            -0.5F, 0.0F, 0.5F, 0.5F, 0.0F, 0.5F, 0.5F, -0.5F, 0.5F
        };
        Vector<U16> indices = {
            0, 1, 2
        };
        PODVector<VertexElement> elements;
        elements.Push({ VertexSemantic::POSITION, VertexType::VEC3 });

        VertexBuffer* vb = NEW_OBJECT(VertexBuffer);
        vb->SetSize(vertices.Size() / 3, elements);
        vb->SetData((U8*)vertices.Buffer());
        mesh->SetVertexBuffer(vb);

        IndexBuffer* ib = NEW_OBJECT(IndexBuffer);
        ib->SetSize(indices.Size());
        ib->SetData((U8*)indices.Buffer());
        mesh->SetIndexBuffer(ib);

        return mesh;
    }
	ProceduralMesh::ProceduralMesh(EngineContext* ctx) : Super(ctx) {
        uvSphere = GenUVSphere();
        plane = GenPlane();
        line = GenLine();
        mTriangle = GenTriangle();
	}
}
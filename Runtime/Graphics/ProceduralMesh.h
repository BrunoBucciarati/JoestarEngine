#pragma once
#include "Mesh.h"
#include "../Math/Vector3.h"
#include "../Math/Vector2.h"
#include "../Core/SubSystem.h"
#include <cmath>
namespace Joestar {
    //test sphere
    class ProceduralMesh : public SubSystem {
        REGISTER_SUBSYSTEM(ProceduralMesh)
    public:
        explicit ProceduralMesh(EngineContext* ctx);
        Mesh* GetUVSphere() { return uvSphere; }
        Mesh* GetPlane() { return plane; }
        Mesh* GetTriangle() { return mTriangle; }
        Mesh* GetLine() { return line; }
    private:
        Mesh* uvSphere;
        Mesh* plane;
        Mesh* line;
        Mesh* mTriangle;
        Mesh* GenUVSphere();
        Mesh* GenPlane();
        Mesh* GenLine();
        Mesh* GenTriangle();
    };
}
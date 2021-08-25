#pragma once
#include "Mesh.h"
#include "../Math/Vector3.h"
#include "../Math/Vector2.h"
#include "../Base/SubSystem.h"
#include <cmath>
namespace Joestar {
    //test sphere
    class ProceduralMesh : public SubSystem {
        REGISTER_SUBSYSTEM(ProceduralMesh)
    public:
        explicit ProceduralMesh(EngineContext* ctx);
        Mesh* GetUVSphere() { return uvSphere; }
    private:
        Mesh* uvSphere;
        Mesh* planeSphere;
        Mesh* GenUVSphere();
    };
}
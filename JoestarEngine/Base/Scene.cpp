#include "Scene.h"
#include "../IO/HID.h"
#include "../Graphics/Graphics.h"
#include <vector>
#include "../IO/FileSystem.h"
namespace Joestar {
    //test sphere
    Mesh* GenUVSphere()
    {
        Mesh* mesh = new Mesh();
        //std::vector<Vertex> vertices;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359;
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }

        return mesh;
    }


    Scene::Scene(EngineContext* ctx) : Super(ctx) {
        //for test
        GameObject* go = new GameObject;
        gameObjects.push_back(go);
        Renderer* render = new Renderer(ctx);
        go->render = render;
        render->mesh = new Mesh;

        FileSystem* fs = GetSubsystem<FileSystem>();
        std::string path = fs->GetResourceDir();
        path += "Models/";
        char workDir[260];
        if (_getcwd(workDir, 260))
            path = workDir + ("/" + path);
        render->mesh->Load(path + "viking_room/viking_room.obj");
     // render->mat_ = new Material;
     // render->mat_->SetDefault();
    }

    void Scene::Update() {
        HID* hid = GetSubsystem<HID>();
        camera.ProcessHID(hid, 0.01f);

        Graphics* graphics = GetSubsystem<Graphics>();
        RenderScene();
    }

    void Scene::RenderScene() {
        Graphics* graphics = GetSubsystem<Graphics>();
        graphics->Clear();
        graphics->UpdateBuiltinMatrix(BUILTIN_MATRIX_PROJECTION, camera.GetProjectionMatrix());
        graphics->UpdateBuiltinMatrix(BUILTIN_MATRIX_VIEW, camera.GetViewMatrix());
        for (std::vector<GameObject*>::const_iterator iter = gameObjects.begin(); iter != gameObjects.end(); iter++) {
            if ((*iter)->render) {
                (*iter)->render->Render(camera);
            }
        }
    }
}
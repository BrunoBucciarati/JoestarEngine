#include "Scene.h"
#include "../IO/HID.h"
#include "../Graphics/Graphics.h"
#include <vector>
#include "../IO/FileSystem.h"
#include "../Math/Vector2.h"
#include "../Graphics/ProceduralMesh.h"
#include "../Math/Vector3.h"
namespace Joestar {
    Scene::Scene(EngineContext* ctx) : Super(ctx) {
        //for test
        GameObject* go = new GameObject;
        gameObjects.push_back(go);
        Renderer* render = NEW_OBJECT(Renderer);
        go->render = render;
        render->mesh = NEW_OBJECT(Mesh);

        FileSystem* fs = GetSubsystem<FileSystem>();
        std::string path = fs->GetModelDir();
        render->mesh->Load(path + "viking_room/viking_room.obj");
        render->mat = NEW_OBJECT(Material);
        render->mat->SetDefault();
     // render->mat_ = new Material;
     // render->mat_->SetDefault();

        GameObject* sphere = new GameObject;
        //gameObjects.push_back(sphere);
        sphere->render = NEW_OBJECT(Renderer);
        sphere->render->mesh = GetSubsystem<ProceduralMesh>()->GetUVSphere();
        sphere->render->mat = NEW_OBJECT(Material);
        sphere->render->mat->SetDefault();
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
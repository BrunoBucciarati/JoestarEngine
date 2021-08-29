#include "Scene.h"
#include "../IO/HID.h"
#include "../Graphics/Graphics.h"
#include <vector>
#include "../IO/FileSystem.h"
#include "../Math/Vector2.h"
#include "../Graphics/ProceduralMesh.h"
#include "../Math/Vector3.h"
#include "../Component/Renderer.h"
namespace Joestar {
    Scene::Scene(EngineContext* ctx) : Super(ctx) {
        //for test
        GameObject* go = NEW_OBJECT(GameObject);
        gameObjects.push_back(go);
        Renderer* render = go->GetComponent<Renderer>();
        render->mesh = NEW_OBJECT(Mesh);

        FileSystem* fs = GetSubsystem<FileSystem>();
        std::string path = fs->GetModelDir();
        render->mesh->Load(path + "viking_room/viking_room.obj");
        render->mat = NEW_OBJECT(Material);
        render->mat->SetDefault();
     // render->mat_ = new Material;
     // render->mat_->SetDefault();

        GameObject* sphere = NEW_OBJECT(GameObject);
        gameObjects.push_back(sphere);
        Renderer* sr = sphere->GetComponent<Renderer>();
        sr->mesh = GetSubsystem<ProceduralMesh>()->GetUVSphere();
        sr->mat = NEW_OBJECT(Material);
        sr->mat->SetDefault();

        selection = sphere;
    }

    void Scene::Update() {
        HID* hid = GetSubsystem<HID>();
        camera.ProcessHID(hid, 0.01f);

        //Test Selection Movement
        if (selection) {
            Vector3f p = selection->GetPosition();
            if (hid->CheckKeyboardInput(KEY_UP)) {
                p.y = p.y + 0.01;
                selection->SetPosition(p);
            }
            if (hid->CheckKeyboardInput(KEY_LEFT)) {
                p.x = p.x - 0.01;
                selection->SetPosition(p);
            }
            if (hid->CheckKeyboardInput(KEY_RIGHT)) {
                p.x = p.x + 0.01;
                selection->SetPosition(p);
            }
            if (hid->CheckKeyboardInput(KEY_DOWN)) {
                p.y = p.y - 0.01;
                selection->SetPosition(p);
            }
        }

        Graphics* graphics = GetSubsystem<Graphics>();
        RenderScene();
    }

    void Scene::RenderScene() {
        Graphics* graphics = GetSubsystem<Graphics>();
        graphics->BeginRenderPass("Scene");
        graphics->Clear();
        graphics->UpdateBuiltinMatrix(BUILTIN_MATRIX_PROJECTION, camera.GetProjectionMatrix());
        graphics->UpdateBuiltinMatrix(BUILTIN_MATRIX_VIEW, camera.GetViewMatrix());
        Renderer* render;
        for (std::vector<GameObject*>::const_iterator iter = gameObjects.begin(); iter != gameObjects.end(); iter++) {
            render = (*iter)->HasComponent<Renderer>();
            if (render) {
                render->Render(camera);
            }
        }
        graphics->EndRenderPass("Scene");
    }
}
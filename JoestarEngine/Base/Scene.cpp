#include "Scene.h"
#include "../IO/HID.h"
#include "../Graphics/Graphics.h"
#include <vector>
#include "../IO/FileSystem.h"
#include "../Math/Vector2.h"
#include "../Graphics/ProceduralMesh.h"
#include "../Math/Vector3.h"
#include "../Graphics/TextureCube.h"
#include "../Component/Renderer.h"
#include "../IO/MemoryManager.h"
#include "../Misc/TimeManager.h"
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

        //GameObject* sphere = NEW_OBJECT(GameObject);
        //gameObjects.push_back(sphere);
        //Renderer* sr = sphere->GetComponent<Renderer>();
        //sr->mesh = GetSubsystem<ProceduralMesh>()->GetUVSphere();
        //sr->mat = NEW_OBJECT(Material);
        //sr->mat->SetPBR();
        //sphere->SetPosition(0, 0, 1.5);
        //selection = sphere;

        GameObject* plane = NEW_OBJECT(GameObject);
        gameObjects.push_back(plane);
        Renderer* pr = plane->GetComponent<Renderer>();
        pr->mesh = GetSubsystem<ProceduralMesh>()->GetPlane();
        pr->mat = NEW_OBJECT(Material);
        pr->mat->SetDefault();


        lightBatch = NEW_OBJECT(LightBatch);

        if (!skyboxMat) {
            skyboxMat = NEW_OBJECT(Material);
            Shader* shader = NEW_OBJECT(Shader);
            shader->SetShader("skybox");
            skyboxMat->SetShader(shader);

            TextureCube* cube = NEW_OBJECT(TextureCube);
            //cube->hasMipMap = false;
            std::string skydir = "Textures/skybox/";
            cube->TextureFromImage(skydir);
            skyboxMat->SetTexture(cube);
        }

        CreateLights();

        CreateCompute();
    }

    Scene::~Scene() {}

    void Scene::CreateCompute() {
        shComputeShader = NEW_OBJECT(Shader);
        shComputeShader->SetShader("computeSH", kComputeShader);

        shComputeBuffer = JOJO_NEW(ComputeBuffer("SHCoef"), MEMORY_GFX_STRUCT);
        shComputeBuffer->SetSize(128);


        shCube = NEW_OBJECT(TextureCube);
        std::string skydir = "Textures/shcube/";
        shCube->TextureFromImage(skydir);
    }

    void Scene::CreateLights() {
        mainLight = NEW_OBJECT(DirectionalLight);
        mainLight->SetDirection(0.0, -1.0, 0.0);
        mainLight->SetPosition(0.0, 3.0, 0.0);

        std::vector<Vector3f> lightPos = {
            {5.0, 4.0, 4.0},
            {-5.0, 4.0, 5.0},
            {-5.0, 4.0, -4.0},
            {5.0, 4.0, -4.0}
        };

        for (int i = 0; i < lightPos.size(); ++i) {
            PointLight* pointLight = NEW_OBJECT(PointLight);
            pointLight->SetPosition(lightPos[i]);
            lights.push_back(pointLight);
        }

        lightMat = NEW_OBJECT(Material);
        Shader* shader = NEW_OBJECT(Shader);
        shader->SetShader("light");
        lightMat->SetShader(shader);

        lightBatch->SetLights(lights);
    }

    void Scene::Update(float dt) {
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

        if (GetSubsystem<TimeManager>()->GetFrame() > 10) {
            PreRenderCompute();
        }
        RenderScene();
    }

    void Scene::PreRenderCompute() {
        //compute sh
        Graphics* graphics = GetSubsystem<Graphics>();
        graphics->BeginCompute("SH COMPUTE");
        graphics->UseShader(shComputeShader);
        Texture* tex = shCube;
        computeSHConsts.sizeAndLevel[0] = tex->GetWidth();
        computeSHConsts.sizeAndLevel[1] = tex->GetHeight();
        computeSHConsts.sizeAndLevel[2] = 1;
        graphics->UpdatePushConstant(&computeSHConsts, sizeof(ComputeSHConsts));
        graphics->UpdateTexture(tex, 1);
        graphics->UpdateComputeBuffer(shComputeBuffer, 0);
        U32 group[3] = { tex->GetWidth() / 8, tex->GetHeight() / 8, 1 };
        graphics->DispatchCompute(group);
        graphics->WriteBackComputeBuffer();
        graphics->EndCompute("SH COMPUTE");
    }

    void Scene::RenderScene() {
        Graphics* graphics = GetSubsystem<Graphics>();
        graphics->BeginRenderPass("Scene");
        graphics->Clear();
        GetSubsystem<Graphics>()->SetDepthCompare(DEPTH_COMPARE_LESS);
        graphics->UpdateBuiltinMatrix(BUILTIN_MATRIX_PROJECTION, camera.GetProjectionMatrix());
        graphics->UpdateBuiltinMatrix(BUILTIN_MATRIX_VIEW, camera.GetViewMatrix());
        graphics->UpdateBuiltinVec3(BUILTIN_VEC3_CAMERAPOS, camera.Position);

        RenderLights();

        Renderer* render;
        for (std::vector<GameObject*>::const_iterator iter = gameObjects.begin(); iter != gameObjects.end(); iter++) {
            render = (*iter)->HasComponent<Renderer>();
            if (render) {
                render->Render(camera);
            }
        }


        RenderSkybox();
        graphics->EndRenderPass("Scene");
    }

    void Scene::RenderLights() {
        //Draw Main Light as Wireframe //maybe don't need
        //Draw Point Light as Sphere
        Graphics* graphics = GetSubsystem<Graphics>();
        lightBlocks = {};
        for (int i = 0; i < lights.size(); ++i) {
            //graphics->UpdateBuiltinMatrix(BUILTIN_MATRIX_MODEL, lights[i]->GetModelMatrix());
            if (lights[i]->GetType() == DIRECTIONAL_LIGHT) {
                graphics->UpdateBuiltinVec3(BUILTIN_VEC3_SUNDIRECTION, lights[i]->GetDirection());
                graphics->UpdateBuiltinVec3(BUILTIN_VEC3_SUNCOLOR, lights[i]->GetColor());
            } else {
                lightBlocks.lightPos[lightBlocks.lightCount] = Vector4f(lights[i]->GetDirection());
                lightBlocks.lightColors[lightBlocks.lightCount] = Vector4f(lights[i]->GetColor());
                ++lightBlocks.lightCount;
            }
        }

        graphics->UpdateLightBlock(lightBlocks);

        graphics->SetPolygonMode(POLYGON_MODE_FILL);
        graphics->UpdateMaterial(lightBatch->GetMaterial());
        graphics->UpdateBuiltinMatrix(BUILTIN_MATRIX_MODEL, lightBatch->GetModelMatrix());
        graphics->DrawMeshInstanced(lightBatch->GetMesh(), lightBatch->GetMaterial(), lightBatch->GetInstanceBuffer());
    }

    void Scene::RenderSkybox() {
        GetSubsystem<Graphics>()->SetDepthCompare(DEPTH_COMPARE_LESSEQUAL);
        GetSubsystem<Graphics>()->UpdateMaterial(skyboxMat);
        GetSubsystem<Graphics>()->DrawMesh(GetSubsystem<ProceduralMesh>()->GetUVSphere(), skyboxMat);
    }
}
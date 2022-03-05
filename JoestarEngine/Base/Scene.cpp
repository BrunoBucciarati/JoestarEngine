#include "Scene.h"
#include "../IO/HID.h"
#include <vector>
#include "../IO/FileSystem.h"
#include "../Math/Vector2.h"
#include "../Graphics/ProceduralMesh.h"
#include "../Math/Vector3.h"
#include "../Graphics/TextureCube.h"
#include "../Component/Renderer.h"
#include "../IO/MemoryManager.h"
#include "../Misc/TimeManager.h"
const U32 SH_LEVEL = 1;

namespace Joestar {
    Scene::Scene(EngineContext* ctx) : Super(ctx) {
        //for test
        //GameObject* go = NEW_OBJECT(GameObject);
        //gameObjects.push_back(go);
        //Renderer* render = go->GetComponent<Renderer>();
        //render->mesh = NEW_OBJECT(Mesh);

        //FileSystem* fs = GetSubsystem<FileSystem>();
        //std::string path = fs->GetModelDir();
        //render->mesh->Load(path + "viking_room/viking_room.obj");
        //render->mat = NEW_OBJECT(Material);
        //render->mat->SetDefault();
        camera = NEW_OBJECT(Camera);

        graphics = GetSubsystem<Graphics>();

        GameObject* sphere = NEW_OBJECT(GameObject);
        gameObjects.push_back(sphere);
        Renderer* sr = sphere->GetComponent<Renderer>();
        sr->mesh = GetSubsystem<ProceduralMesh>()->GetUVSphere();
        sr->mat = NEW_OBJECT(Material);
        sr->mat->SetPBR();
        sphere->SetPosition(0, 1.2, 0);
        selection = sphere;

        GameObject* plane = NEW_OBJECT(GameObject);
        gameObjects.push_back(plane);
        Renderer* pr = plane->GetComponent<Renderer>();
        pr->mesh = GetSubsystem<ProceduralMesh>()->GetPlane();
        pr->mat = NEW_OBJECT(Material);
        pr->mat->SetPlaneMat();


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
        //shComputeShader->SetShader("computeTest", kComputeShader);
        shComputeShader->SetShader("computeSH", kComputeShader);

        shComputeBuffer = JOJO_NEW(ComputeBuffer("SHCoef"), MEMORY_GFX_STRUCT);
        shComputeBuffer->SetSize(128);

        shCube = NEW_OBJECT(TextureCube);
        std::string skydir = "Textures/shcube/";
        shCube->TextureFromImage(skydir);
        shCube->hasMipMap = false;


        PreRenderCompute();
    }

    void Scene::CreateLights() {
        mainLight = NEW_OBJECT(DirectionalLight);
        mainLight->SetDirection(0.0, -1.0, 0.0);
        mainLight->SetPosition(0.0, 3.0, 0.0);
        mainLight->SetIntensity(20.f);

        std::vector<Vector3f> lightPos = {
            {5.0, 4.0, 4.0},
            {-5.0, 4.0, 5.0},
            {-5.0, 4.0, -4.0},
            {5.0, 4.0, -4.0}
        };

        for (int i = 0; i < lightPos.size(); ++i) {
            PointLight* pointLight = NEW_OBJECT(PointLight);
            pointLight->SetPosition(lightPos[i]);
            pointLight->SetIntensity(50.f);
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
        camera->ProcessHID(hid, 0.01f);

        //Test Selection Movement
        if (selection) {
            Vector3f p = selection->GetPosition();
            if (hid->CheckKeyboardInput(KEY_UP)) {
                p.y = p.y + 0.1;
                selection->SetPosition(p);
            }
            if (hid->CheckKeyboardInput(KEY_LEFT)) {
                p.x = p.x - 0.1;
                selection->SetPosition(p);
            }
            if (hid->CheckKeyboardInput(KEY_RIGHT)) {
                p.x = p.x + 0.1;
                selection->SetPosition(p);
            }
            if (hid->CheckKeyboardInput(KEY_DOWN)) {
                p.y = p.y - 0.1;
                selection->SetPosition(p);
            }
        }

        RenderScene();
    }

    void Scene::PreRenderCompute() {
        //compute sh
        graphics->BeginCompute("SH COMPUTE");
        graphics->UseShader(shComputeShader);
        Texture* tex = shCube;
        computeSHConsts.sizeAndLevel[0] = tex->GetWidth();
        computeSHConsts.sizeAndLevel[1] = tex->GetHeight();
        computeSHConsts.sizeAndLevel[2] = SH_LEVEL;
        graphics->UpdatePushConstant(&computeSHConsts, sizeof(ComputeSHConsts));
        graphics->UpdateTexture(tex, 1);
        graphics->UpdateComputeBuffer(shComputeBuffer, 0);
        U32 group[3] = { 1, 1, 6 };
        graphics->WriteBackComputeBuffer();
        graphics->DispatchCompute(group);
        graphics->EndCompute("SH COMPUTE");
    }

    void Scene::RenderShadowMap() {
        if (!shadowShader) {
            shadowShader = NEW_OBJECT(Shader);
            shadowShader->SetShader("shadowMap");
            shadowCam = NEW_OBJECT(Camera);
            shadowCam->SetOrthographic(10);

            shadowMapFB = new FrameBufferDef();
            shadowMapFB->width = 1024;
            shadowMapFB->height= 1024;
            shadowMapFB->colorEnabled = false;
        }
        graphics->BeginRenderPass("Shadow Map");
        graphics->Clear();
        graphics->SetFrameBuffer(shadowMapFB);
        graphics->SetDepthCompare(DEPTH_COMPARE_LESS);
        graphics->UpdateBuiltinMatrix(BUILTIN_MATRIX_PROJECTION, shadowCam->GetProjectionMatrix());
        graphics->UpdateBuiltinMatrix(BUILTIN_MATRIX_VIEW, shadowCam->GetViewMatrix());
        graphics->FlushUniformBuffer(STR_STRUCT(UniformBufferObject));
        graphics->UseShader(shadowShader);
        Renderer* render;
        for (std::vector<GameObject*>::const_iterator iter = gameObjects.begin(); iter != gameObjects.end(); iter++) {
            render = (*iter)->HasComponent<Renderer>();
            if (render) {
                render->RenderToShadowMap();
            }
        }
        graphics->EndRenderPass("Shadow Map");
        DELETE_OBJECT(shadowCam);
    }

    void Scene::RenderScene() {
        RenderShadowMap();

        graphics->BeginRenderPass("Scene");
        graphics->Clear();
        graphics->SetDepthCompare(DEPTH_COMPARE_LESS);
        graphics->UpdateBuiltinMatrix(BUILTIN_MATRIX_PROJECTION, camera->GetProjectionMatrix());
        graphics->UpdateBuiltinMatrix(BUILTIN_MATRIX_VIEW, camera->GetViewMatrix());
        graphics->UpdateBuiltinVec3(BUILTIN_VEC3_CAMERAPOS, camera->Position);
        graphics->UpdateBuiltinVec3(BUILTIN_VEC3_SUNDIRECTION, mainLight->GetDirection());
        graphics->UpdateBuiltinVec3(BUILTIN_VEC3_SUNCOLOR, mainLight->GetIntensityMixColor());
        graphics->FlushUniformBuffer();
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
        lightBlocks = {};
        for (int i = 0; i < lights.size(); ++i) {
            //graphics->UpdateBuiltinMatrix(BUILTIN_MATRIX_MODEL, lights[i]->GetModelMatrix());
            if (lights[i]->GetType() == POINT_LIGHT) {
                lightBlocks.lightPos[lightBlocks.lightCount] = Vector4f(lights[i]->GetPosition());
                lightBlocks.lightColors[lightBlocks.lightCount] = Vector4f(lights[i]->GetIntensityMixColor());
                ++lightBlocks.lightCount;
            }
        }

        //read compute data
        U32* data = reinterpret_cast<U32*>(shComputeBuffer->GetBuffer());
        float pdf = (shCube->GetWidth() * shCube->GetHeight());
        for (int i = 0; i < (SH_LEVEL + 1) * (SH_LEVEL + 1); ++i) {
            U32 r = *data;
            ++data;
            U32 g = *data;
            ++data;
            U32 b = *data;
            ++data;
            //LOG("sh data, r: %d, g: %d, b: %d\n", r, g, b);
            //in shader, lack of atomic float support, we scale with 10000 and store in int
            lightBlocks.shCoef[i].Set(float(r) / pdf, float(g) / pdf, float(b) / pdf, 0.f);
        }

        graphics->UpdateLightBlock(lightBlocks);

        graphics->SetPolygonMode(POLYGON_MODE_FILL);
        graphics->UpdateMaterial(lightBatch->GetMaterial());
        graphics->UpdateBuiltinMatrix(BUILTIN_MATRIX_MODEL, lightBatch->GetModelMatrix());
        graphics->DrawMeshInstanced(lightBatch->GetMesh(), lightBatch->GetMaterial(), lightBatch->GetInstanceBuffer());
    }

    void Scene::RenderSkybox() {
        graphics->SetDepthCompare(DEPTH_COMPARE_LESSEQUAL);
        graphics->UpdateMaterial(skyboxMat);
        graphics->DrawMesh(GetSubsystem<ProceduralMesh>()->GetUVSphere(), skyboxMat);
    }
}
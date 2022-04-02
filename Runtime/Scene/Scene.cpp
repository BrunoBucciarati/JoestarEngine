#include "Scene.h"
#include "../IO/HID.h"
#include <vector>
#include "../IO/FileSystem.h"
#include "../Math/Vector2.h"
#include "../Graphics/ProceduralMesh.h"
#include "../Graphics/PipelineState.h"
#include "../Math/Vector3.h"
#include "../Graphics/TextureCube.h"
#include "../Component/MeshRenderer.h"
#include "../IO/MemoryManager.h"
#include "../Misc/TimeManager.h"
#include "../Base/Camera.h"

namespace Joestar {
    const U32 SH_LEVEL = 1;
    Scene::Scene(EngineContext* ctx) : Super(ctx) {
        mGraphics = GetSubsystem<Graphics>();

        //GameObject* sphere = NEW_OBJECT(GameObject);
        //gameObjects.Push(sphere);
        //MeshRenderer* sr = sphere->GetComponent<MeshRenderer>();
        //sr->SetMesh(GetSubsystem<ProceduralMesh>()->GetUVSphere());
        //sr->SetMaterial(NEW_OBJECT(Material));
        //sr->GetMaterial()->SetPBR();
        //sphere->SetPosition(0, 1.2, 0);
        //selection = sphere;
        //pr->SetMaterial(NEW_OBJECT(Material));
        //pr->GetMaterial()->SetPlaneMat();


        //lightBatch = NEW_OBJECT(LightBatch);

        //if (!skyboxMat) {
        //    skyboxMat = NEW_OBJECT(Material);
        //    Shader* shader = NEW_OBJECT(Shader);
        //    shader->SetShader("skybox");
        //    skyboxMat->SetShader(shader);

        //    TextureCube* cube = NEW_OBJECT(TextureCube);
        //    //cube->hasMipMap = false;
        //    String skydir = "Textures/skybox/";
        //    cube->TextureFromImage(skydir);
        //    skyboxMat->SetTexture(cube);
        //}

        //CreateLights();

        //CreateCompute();

        GameObject* plane = NEW_OBJECT(GameObject);
        gameObjects.Push(plane);
        MeshRenderer* pr = plane->GetComponent<MeshRenderer>();
        pr->SetMesh(GetSubsystem<ProceduralMesh>()->GetPlane());
        pr->SetShader("vertex", ShaderStage::VS);
        pr->SetShader("fragment", ShaderStage::PS);

        //GameObject* tri = NEW_OBJECT(GameObject);
        //gameObjects.Push(tri);
        //MeshRenderer* tr = tri->GetComponent<MeshRenderer>();
        //tr->SetMesh(GetSubsystem<ProceduralMesh>()->GetTriangle());
        //tr->SetShader("triangle", ShaderStage::VS);
        //tr->SetShader("triangle", ShaderStage::PS);


        mMainRenderPass = JOJO_NEW(RenderPass, MEMORY_GFX_STRUCT);
        mMainRenderPass->SetLoadOp(AttachmentLoadOp::DONT_CARE);
        mMainRenderPass->SetStoreOp(AttachmentStoreOp::DONT_CARE);
        mGraphics->CreateRenderPass(mMainRenderPass);
    }

    Scene::~Scene() {}

    void Scene::CreateCompute() {
        //shComputeShader = NEW_OBJECT(Shader);
        ////shComputeShader->SetShader("computeTest", kComputeShader);
        //shComputeShader->SetShader("computeSH", kComputeShader);

        //shComputeBuffer = JOJO_NEW(ComputeBuffer("SHCoef"), MEMORY_GFX_STRUCT);
        //shComputeBuffer->SetSize(128);

        //shCube = NEW_OBJECT(TextureCube);
        //String skydir = "Textures/shcube/";
        //shCube->TextureFromImage(skydir);
        //shCube->hasMipMap = false;


        //PreRenderCompute();
    }

    void Scene::CreateLights() {
        //mainLight = NEW_OBJECT(DirectionalLight);
        //mainLight->SetDirection(0.0, -1.0, 0.0);
        //mainLight->SetPosition(0.0, 3.0, 0.0);
        //mainLight->SetIntensity(20.f);

        //Vector<Vector3f> lightPos;
        //lightPos.Push({ 5.0, 4.0, 4.0 });
        //lightPos.Push({ -5.0, 4.0, 5.0 });
        //lightPos.Push({ -5.0, 4.0, -4.0 });
        //lightPos.Push({ 5.0, 4.0, -4.0 });

        //for (int i = 0; i < lightPos.Size(); ++i) {
        //    PointLight* pointLight = NEW_OBJECT(PointLight);
        //    pointLight->SetPosition(lightPos[i]);
        //    pointLight->SetIntensity(50.f);
        //    lights.Push(pointLight);
        //}

        //lightMat = NEW_OBJECT(Material);
        //Shader* shader = NEW_OBJECT(Shader);
        //shader->SetShader("light");
        //lightMat->SetShader(shader);

        //lightBatch->SetLights(lights);
    }

    void Scene::Update(float dt) {
        HID* hid = GetSubsystem<HID>();
        //camera->ProcessHID(hid, 0.01f);

        ////Test Selection Movement
        //if (selection) {
        //    Vector3f p = selection->GetPosition();
        //    if (hid->CheckKeyboardInput(KEY_UP)) {
        //        p.y = p.y + 0.1;
        //        selection->SetPosition(p);
        //    }
        //    if (hid->CheckKeyboardInput(KEY_LEFT)) {
        //        p.x = p.x - 0.1;
        //        selection->SetPosition(p);
        //    }
        //    if (hid->CheckKeyboardInput(KEY_RIGHT)) {
        //        p.x = p.x + 0.1;
        //        selection->SetPosition(p);
        //    }
        //    if (hid->CheckKeyboardInput(KEY_DOWN)) {
        //        p.y = p.y - 0.1;
        //        selection->SetPosition(p);
        //    }
        //}

        //RenderScene();
    }

    void Scene::PreRenderCompute() {
        //compute sh
        //graphics->BeginCompute("SH COMPUTE");
        //graphics->UseShader(shComputeShader);
        //Texture* tex = shCube;
        //computeSHConsts.sizeAndLevel[0] = tex->GetWidth();
        //computeSHConsts.sizeAndLevel[1] = tex->GetHeight();
        //computeSHConsts.sizeAndLevel[2] = SH_LEVEL;
        //graphics->UpdatePushConstant(&computeSHConsts, sizeof(ComputeSHConsts));
        //graphics->UpdateTexture(tex, 1);
        //graphics->UpdateComputeBuffer(shComputeBuffer, 0);
        //U32 group[3] = { 1, 1, 6 };
        //graphics->WriteBackComputeBuffer();
        //graphics->DispatchCompute(group);
        //graphics->EndCompute("SH COMPUTE");
    }

    void Scene::RenderShadowMap() {
        //if (!shadowShader) {
        //    shadowShader = NEW_OBJECT(Shader);
        //    shadowShader->SetShader("shadowMap");
        //    shadowCam = NEW_OBJECT(Camera);
        //    shadowCam->SetOrthographic(10);

        //    shadowMapFB = new FrameBufferDef();
        //    shadowMapFB->width = 1024;
        //    shadowMapFB->height= 1024;
        //    shadowMapFB->colorEnabled = false;
        //}
        //graphics->BeginRenderPass("Shadow Map");
        //graphics->Clear();
        //graphics->SetFrameBuffer(shadowMapFB);
        //graphics->SetDepthCompare(DEPTH_COMPARE_LESS);
        //graphics->UpdateBuiltinMatrix(BUILTIN_MATRIX_PROJECTION, shadowCam->GetProjectionMatrix());
        //graphics->UpdateBuiltinMatrix(BUILTIN_MATRIX_VIEW, shadowCam->GetViewMatrix());
        //graphics->FlushUniformBuffer(STR_STRUCT(UniformBufferObject));
        //graphics->UseShader(shadowShader);
        //Renderer* render;
        //for (Vector<GameObject*>::ConstIterator iter = gameObjects.Begin(); iter != gameObjects.End(); ++iter) {
        //    render = (*iter)->HasComponent<Renderer>();
        //    if (render) {
        //        render->RenderToShadowMap();
        //    }
        //}
        //graphics->EndRenderPass("Shadow Map");
        //DELETE_OBJECT(shadowCam);
    }

    void Scene::RenderScene(CommandBuffer* cb) {
        gameObjects[0]->GetComponent<MeshRenderer>()->Render(cb);
    }

    void Scene::RenderLights() {
        //Draw Main Light as Wireframe //maybe don't need
        //Draw Point Light as Sphere
        //lightBlocks = {};
        //for (int i = 0; i < lights.Size(); ++i) {
        //    //graphics->UpdateBuiltinMatrix(BUILTIN_MATRIX_MODEL, lights[i]->GetModelMatrix());
        //    if (lights[i]->GetType() == POINT_LIGHT) {
        //        lightBlocks.lightPos[lightBlocks.lightCount] = Vector4f(lights[i]->GetPosition());
        //        lightBlocks.lightColors[lightBlocks.lightCount] = Vector4f(lights[i]->GetIntensityMixColor());
        //        ++lightBlocks.lightCount;
        //    }
        //}

        //read compute data
        //U32* data = reinterpret_cast<U32*>(shComputeBuffer->GetBuffer());
        //float pdf = (shCube->GetWidth() * shCube->GetHeight());
        //for (int i = 0; i < (SH_LEVEL + 1) * (SH_LEVEL + 1); ++i) {
        //    U32 r = *data;
        //    ++data;
        //    U32 g = *data;
        //    ++data;
        //    U32 b = *data;
        //    ++data;
        //    //LOG("sh data, r: %d, g: %d, b: %d\n", r, g, b);
        //    //in shader, lack of atomic float support, we scale with 10000 and store in int
        //    lightBlocks.shCoef[i].Set(float(r) / pdf, float(g) / pdf, float(b) / pdf, 0.f);
        //}

        //graphics->UpdateLightBlock(lightBlocks);

        ////graphics->SetPolygonMode(POLYGON_MODE_FILL);
        //graphics->UpdateMaterial(lightBatch->GetMaterial());
        //graphics->UpdateBuiltinMatrix(BUILTIN_MATRIX_MODEL, lightBatch->GetModelMatrix());
        ////graphics->DrawMeshInstanced(lightBatch->GetMesh(), lightBatch->GetMaterial(), lightBatch->GetInstanceBuffer());
    }

    void Scene::RenderSkybox() {
        //graphics->SetDepthCompare(DEPTH_COMPARE_LESSEQUAL);
        //graphics->UpdateMaterial(skyboxMat);
        //graphics->DrawMesh(GetSubsystem<ProceduralMesh>()->GetUVSphere(), skyboxMat);
    }
}
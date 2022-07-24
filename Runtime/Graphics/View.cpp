#include "View.h"
#include "../Graphics/Camera.h"
#include "../Scene/Scene.h"
#include "GraphicDefines.h"
#include "Shader/Shader.h"
#include "SwapChain.h"
#include "Descriptor.h"
#include "UniformBuffer.h"
#include "../Component/MeshRenderer.h"
#include "../IO/HID.h"
#include "Texture2D.h"
#include "../Math/Frustum.h"

namespace Joestar
{
	View::View(EngineContext* ctx) : Super(ctx)
	{
		mShadowCameraNode = NEW_OBJECT(GameObject);
		mCameraNode = NEW_OBJECT(GameObject);
		mCameraNode->SetPosition(0.0, 10.0, 10.0);
		mCamera = mCameraNode->GetComponent<Camera>();
		mScene = NEW_OBJECT(Scene);
		mGraphics = GetSubsystem<Graphics>();
		CreatePassDescriptor();
		//mCamera->SetOrthographic(10.0F);
		mCamera->SetPerspective();
		mHID = GetSubsystem<HID>();
	}

	View::~View()
	{}

	void View::CreatePassDescriptor()
	{
		mDescriptorSetLayout = JOJO_NEW(DescriptorSetLayout, MEMORY_GFX_STRUCT);
		mDescriptorSetLayout->SetNumBindings(1);
		//目前是View + Proj
		auto* binding = mDescriptorSetLayout->GetLayoutBinding(0);
		binding->binding = 0;
		binding->type = DescriptorType::UNIFORM_BUFFER;
		binding->count = 1;
		binding->stage = (U32)ShaderStage::VS | (U32)ShaderStage::DS;
		binding->size = 128;
		binding->members.Resize(2);
		binding->members[0].ID = (U32)PerPassUniforms::VIEW_MATRIX;
		binding->members[0].offset = 0;
		binding->members[0].size = 64;
		binding->members[1].ID = (U32)PerPassUniforms::PROJECTION_MATRIX;
		binding->members[1].offset = 64;
		binding->members[1].size = 64;
		mGraphics->SetDescriptorSetLayout(mDescriptorSetLayout);

		mAllDescriptorSets.Resize((U32)Pass::Count);
		for (U32 i = 0; i < (U32)Pass::Count; ++i)
		{
			mAllDescriptorSets[i] = JOJO_NEW(DescriptorSets, MEMORY_GFX_STRUCT);
			mAllDescriptorSets[i]->AllocFromLayout(mDescriptorSetLayout);
			mGraphics->CreateDescriptorSets(mAllDescriptorSets[i]);
		}

		mAllUniformBuffers.Resize((U32)Pass::Count);
		for (U32 i = 0; i < (U32)Pass::Count; ++i)
		{
			UniformBuffer* ub = JOJO_NEW(UniformBuffer, MEMORY_GFX_STRUCT);
			ub->SetFrequency(UniformFrequency::PASS);
			ub->AllocFromBinding(binding);
			mGraphics->CreateUniformBuffer(ub);

			Vector<SharedPtr<UniformBuffer>>& passBuffer = mAllUniformBuffers[i];
			mAllDescriptorSets[i]->SetBindingUniformBuffer(0, ub);
			passBuffer.Push(ub);
		}
	}

	void View::Update(float dt)
	{
		if (mCamera)
		{
			mCamera->ProcessHID(mHID, dt);
		}
		if (mScene)
		{
			mScene->Update(dt);
		}
	}

	bool View::Render()
	{
		//持有交换链的就是mainView
		bool bRecord = false;
		if (mSwapChain)
		{
			if (!mSwapChain->IsReady())
			{
				bRecord = false;
			}
			else
			{
				mViewport.SetSize(mSwapChain->width, mSwapChain->height);
				bRecord = true;
			}
		}
		CommandBuffer* cb = mGraphics->GetMainCommandBuffer();
		cb->Begin();
		cb->SetViewport(&mViewport);
		//if (bRecord)
		ForwardRender(cb);
		return bRecord;
	}

	void View::SetUniformBuffer(PerPassUniforms uniform, U8* data, Pass pass)
	{
		DescriptorSetLayoutBinding::Member member;
		U32 binding = mDescriptorSetLayout->GetUniformMemberAndBinding((U32)uniform, member);
		Vector<SharedPtr<UniformBuffer>>& passBuffer = mAllUniformBuffers[(U32)pass];
		passBuffer[binding]->SetData(member.offset, member.size, data);
	}

	void View::CollectBatches()
	{
		mBatches.Clear();
		auto& gameObjects = mScene->GetGameObjects();
		Frustum viewFrustum = mCamera->GetFrustum();
		for (auto go : gameObjects)
		{
			MeshRenderer* renderer = go->HasComponent<MeshRenderer>();
			auto& aabb = renderer->GetBoundingBox();
			if (Intersection::OUTSIDE == viewFrustum.IsInside(aabb))
				continue;
			Batch batch(renderer);
			batch.CalculateKey();
			mBatches.Push(batch);
		}
	}

	void View::CollectShadowBatches()
	{
		mShadowBatches.Clear();
		auto& gameObjects = mScene->GetGameObjects();
		if (!mShadowMaterial)
		{
			Material* mat = NEW_OBJECT(Material);
			mat->SetShader("shadow");
			mShadowMaterial = NEW_OBJECT(MaterialInstance, mat);
		}
		Frustum viewFrustum = mShadowCamera->GetFrustum();
		for (auto go : gameObjects)
		{
			MeshRenderer* renderer = go->HasComponent<MeshRenderer>();
			if (!renderer->GetCastShadow())
				continue;
			auto& aabb = renderer->GetBoundingBox();
			if (Intersection::OUTSIDE == viewFrustum.IsInside(aabb))
				continue;
			Batch batch(renderer);
			batch.mMaterial = mShadowMaterial;
			batch.mShaderProgram = mShadowMaterial->GetShaderProgram();
			batch.CalculateKey();
			mShadowBatches.Push(batch);
		}
	}

	void View::InitShadowPass()
	{
		if (mShadowPass)
		{
			return;
		}
		mShadowCamera = mShadowCameraNode->GetComponent<Camera>();
		mShadowPass = JOJO_NEW(RenderPass, MEMORY_GFX_STRUCT);
		mShadowPass->SetClear(true);
		mShadowPass->SetLoadOp(AttachmentLoadOp::CLEAR);
		mShadowPass->SetStoreOp(AttachmentStoreOp::STORE);
		mShadowPass->SetColorAttachmentCount(0);
		mGraphics->CreateRenderPass(mShadowPass);

		mShadowMap = NEW_OBJECT(Texture2D);
		mShadowMap->SetFormat(ImageFormat::D32);
		mShadowMap->SetWidth(2048);
		mShadowMap->SetHeight(2048);
		mShadowMap->SetRenderTarget();

		mShadowFrameBuffer = JOJO_NEW(FrameBuffer, MEMORY_GFX_STRUCT);
		mShadowFrameBuffer->SetWidth(2048);
		mShadowFrameBuffer->SetHeight(2048);
		mShadowFrameBuffer->SetRenderPass(mShadowPass);
		mShadowFrameBuffer->SetDepthStencil(mShadowMap->GetImageView());
		mGraphics->CreateFrameBuffer(mShadowFrameBuffer);
	}

	void View::PrepareCameraMatrix(Camera* cam, Matrix4x4f& view, Matrix4x4f& proj)
	{
		view = cam->GetViewMatrix();
		proj = cam->GetProjectionMatrix();
		if (!mGraphics->IsColumnMajor())
		{
			view.Transponse();
			proj.Transponse();
		}
		if (!mGraphics->IsOriginBottomLeft())
		{
			proj.FlipY();
		}
	}

	void View::DoShadowPass(CommandBuffer* cb)
	{
		Light* mainLight = mScene->GetMainLight();
		mShadowCamera->SetPosition(mainLight->GetPosition());
		mShadowCamera->SetOrthographic(100.F);
		//mShadowCamera->SetWorldRotation(mainLight->GetWorldRotation());
		Matrix4x4f view, proj;
		PrepareCameraMatrix(mShadowCamera, view, proj);
		SetUniformBuffer(PerPassUniforms::VIEW_MATRIX, (U8*)&view);
		SetUniformBuffer(PerPassUniforms::PROJECTION_MATRIX, (U8*)&proj);
		mGraphics->SetUniformBuffer(mAllUniformBuffers[(U32)Pass::Shadow][0]);
		mGraphics->UpdateDescriptorSets(mAllDescriptorSets[(U32)Pass::Shadow]);
		//要增加shadow的pass和FrameBuffer
		cb->BeginRenderPass(mShadowPass, mShadowFrameBuffer);
		cb->SetPassDescriptorSets(mAllDescriptorSets[(U32)Pass::Shadow]);
		for (auto& batch : mShadowBatches)
		{
			batch.Render(this, cb);
		}
		cb->EndRenderPass(mShadowPass);
	}

	void View::ForwardRender(CommandBuffer* cb)
	{
		//Check if Shadow Pass inited
		InitShadowPass();
		CollectShadowBatches();
		CollectBatches();

		if (!mShadowBatches.Empty())
			DoShadowPass(cb);
		//Scene Pass
		Matrix4x4f view, proj;
		PrepareCameraMatrix(mCamera, view, proj);
		SetUniformBuffer(PerPassUniforms::VIEW_MATRIX, (U8*)&view);
		SetUniformBuffer(PerPassUniforms::PROJECTION_MATRIX, (U8*)&proj);
		mGraphics->SetUniformBuffer(mAllUniformBuffers[(U32)Pass::Scene][0]);
		mGraphics->UpdateDescriptorSets(mAllDescriptorSets[(U32)Pass::Scene]);
		cb->BeginRenderPass(mGraphics->GetMainRenderPass(), mGraphics->GetBackBuffer());
		cb->SetPassDescriptorSets(mAllDescriptorSets[(U32)Pass::Scene]);
		for (auto& batch : mBatches)
		{
			batch.Render(this, cb);
		}

		//Skybox
		if (mScene)
		{
			GameObject* skybox = mScene->GetSkybox();
			Batch skyboxBatch(skybox->GetComponent<MeshRenderer>());
			skyboxBatch.Render(this, cb);
		}
		cb->EndRenderPass(mGraphics->GetMainRenderPass());
	}
}
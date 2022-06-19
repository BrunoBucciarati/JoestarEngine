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

namespace Joestar
{
	View::View(EngineContext* ctx) : Super(ctx)
	{
		mShadowCameraNode = NEW_OBJECT(GameObject);
		mCameraNode = NEW_OBJECT(GameObject);
		mCameraNode->SetPosition(0.0, 1.0, 3.0);
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
		binding->stage = (U32)ShaderStage::VS;
		binding->size = 128;
		binding->members.Resize(2);
		binding->members[0].ID = (U32)PerPassUniforms::VIEW_MATRIX;
		binding->members[0].offset = 0;
		binding->members[0].size = 64;
		binding->members[1].ID = (U32)PerPassUniforms::PROJECTION_MATRIX;
		binding->members[1].offset = 64;
		binding->members[1].size = 64;
		mGraphics->SetDescriptorSetLayout(mDescriptorSetLayout);

		mDescriptorSets = JOJO_NEW(DescriptorSets, MEMORY_GFX_STRUCT);
		mDescriptorSets->AllocFromLayout(mDescriptorSetLayout);
		mGraphics->CreateDescriptorSets(mDescriptorSets);

		UniformBuffer* ub = JOJO_NEW(UniformBuffer, MEMORY_GFX_STRUCT);
		ub->SetFrequency(UniformFrequency::PASS);
		ub->AllocFromBinding(binding);
		mGraphics->CreateUniformBuffer(ub);
		mDescriptorSets->SetBindingUniformBuffer(0, ub);

		mUniformBuffers.Push(ub);
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
		cb->EndRenderPass(mGraphics->GetMainRenderPass());
		//cb->End();
		return bRecord;
	}

	void View::RenderScene(CommandBuffer* cb)
	{
		cb->BeginRenderPass(mGraphics->GetMainRenderPass(), mGraphics->GetBackBuffer());
		cb->SetPassDescriptorSets(mDescriptorSets);
		if (mScene)
		{
			mScene->RenderScene(cb);
		}
	}

	void View::RenderSkybox(CommandBuffer* cb)
	{
		if (mScene)
			mScene->RenderSkybox(cb);
	}

	void View::SetUniformBuffer(PerPassUniforms uniform, U8* data)
	{
		DescriptorSetLayoutBinding::Member member;
		U32 binding = mDescriptorSetLayout->GetUniformMemberAndBinding((U32)uniform, member);
		mUniformBuffers[binding]->SetData(member.offset, member.size, data);
	}

	void View::CollectBatches()
	{
		mBatches.Clear();
		auto& gameObjects = mScene->GetGameObjects();
		for (auto go : gameObjects)
		{
			MeshRenderer* renderer = go->HasComponent<MeshRenderer>();
			Batch batch(renderer);
			batch.CalculateKey();
			mBatches.Push(batch);
		}
	}

	void View::CollectShadowBatches()
	{
		mShadowBatches.Clear();
		auto& gameObjects = mScene->GetGameObjects();
		if (!mShadowProgram)
		{
			mShadowProgram = NEW_OBJECT(ShaderProgram);
			mShadowProgram->SetShader(ShaderStage::VS, "shadow_vs");
			mShadowProgram->SetShader(ShaderStage::PS, "shadow_ps");
		}
		for (auto go : gameObjects)
		{
			//MeshRenderer* renderer = go->HasComponent<MeshRenderer>();
			//Batch batch(renderer);
			//batch.mPipelineLayout = mShadowProgram->GetPipelineLayout();
			//batch.CalculateKey();
			//mBatches.Push(batch);
		}
	}

	void View::ForwardRender(CommandBuffer* cb)
	{
		CollectBatches();

		//Scene Pass
		SetUniformBuffer(PerPassUniforms::VIEW_MATRIX, (U8*)mCamera->GetViewMatrix());
		SetUniformBuffer(PerPassUniforms::PROJECTION_MATRIX, (U8*)mCamera->GetProjectionMatrix());
		mGraphics->SetUniformBuffer(mUniformBuffers[0]);
		mGraphics->UpdateDescriptorSets(mDescriptorSets);
		cb->BeginRenderPass(mGraphics->GetMainRenderPass(), mGraphics->GetBackBuffer());
		cb->SetPassDescriptorSets(mDescriptorSets);
		for (auto& batch : mBatches)
		{
			batch.Render(this, cb);
		}
		//Shadow Pass
		//if (!mShadowCamera)
		//{
		//	mShadowCamera = mShadowCameraNode->GetComponent<Camera>();
		//}
		//Light* mainLight = mScene->GetMainLight();
		//mShadowCamera->SetPosition(mainLight->GetPosition());
		//mShadowCamera->SetOrthographic(100.F);
		//mShadowCamera->SetWorldRotation(mainLight->GetWorldRotation());
		//SetUniformBuffer(PerPassUniforms::VIEW_MATRIX, (U8*)mShadowCamera->GetViewMatrix());
		//SetUniformBuffer(PerPassUniforms::PROJECTION_MATRIX, (U8*)mShadowCamera->GetViewMatrix());
		//RenderScene(cb);
		////Scene Pass
		//SetUniformBuffer(PerPassUniforms::VIEW_MATRIX, (U8*)mCamera->GetViewMatrix());
		//SetUniformBuffer(PerPassUniforms::PROJECTION_MATRIX, (U8*)mCamera->GetProjectionMatrix());
		//mGraphics->SetUniformBuffer(mUniformBuffers[0]);
		//mGraphics->UpdateDescriptorSets(mDescriptorSets);
		//RenderScene(cb);
		//RenderSkybox(cb);
	}
}
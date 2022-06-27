#include "Graphics.h"
#include "../Scene/Scene.h"
#include "../Misc/GlobalConfig.h"
#include "../Thread/RenderThreadVulkan.h"
//#include "../Thread/RenderThreadGL.h"
#include "../Thread/RenderThreadD3D11.h"
#include "../IO/MemoryManager.h"
#include "CommandBuffer.h"
#include "GFXCommandList.h"
#include "SwapChain.h"
#include "Window.h"
#include "GPUCreateInfos.h"
#include "PipelineState.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

#define CREATE_NEW_HANDLE_VEC(_VAR, _TYP, _VECTORNAME) \
	GPUResourceHandle handle = _VECTORNAME.Size(); \
	_TYP* _VAR = JOJO_NEW(_TYP, MEMORY_GFX_STRUCT); \
	_VAR->handle = handle; \
	_VECTORNAME.Push(_VAR);

#define CREATE_NEW_HANDLE(_VAR, _TYP) \
	CREATE_NEW_HANDLE_VEC(_VAR, _TYP, m##_TYP##s)

#define ASSIGN_NEW_HANDLE(_VAR, _VEC) \
	GPUResourceHandle handle = _VEC.Size(); \
	_VAR->SetHandle(handle); \
	_VEC.Push(_VAR);

namespace Joestar {
	Graphics::Graphics(EngineContext* context) : Super(context) {
		cmdBuffers.Resize(MAX_CMDLISTS_IN_FLIGHT);
		for (auto& cmdBuffer : cmdBuffers)
			cmdBuffer = JOJO_NEW(GFXCommandBuffer(1024));
		cmdBuffer = cmdBuffers[0];

		computeCmdBuffers.Resize(MAX_CMDLISTS_IN_FLIGHT);
		for (auto& cmdBuffer : computeCmdBuffers)
			cmdBuffer = JOJO_NEW(GFXCommandBuffer(256));
		computeCmdBuffer = computeCmdBuffers[0];
		defaultClearColor.Set(0.0f, 0.0f, 0.0f, 1.0f);
	}
	Graphics::~Graphics()
	{
	}

	void Graphics::Init()
	{
		//创建MainCmdList
		ThreadCommandList cmdLists;
		for (int i = 0; i < MAX_CMDLISTS_IN_FLIGHT; ++i)
		{
			cmdLists.cmdList[i] = JOJO_NEW(GFXCommandList);
		}
		mThreadCommandLists.Push(cmdLists);
		//创建交换链
		CreateSwapChain();
		//CreateSyncObjects();
		//创建RenderCommandBuffer
		CommandPool* cp = JOJO_NEW(CommandPool, MEMORY_GFX_STRUCT);
		cp->SetQueue(GPUQueue::GRAPHICS);
		CreateCommandPool(cp);
		CommandBuffer* cb = JOJO_NEW(CommandBuffer, MEMORY_GFX_STRUCT);
		cb->SetQueue(GPUQueue::GRAPHICS);
		cb->SetPool(cp);
		CreateCommandBuffer(cb);
		//创建TransferCommandBuffer
		cp = JOJO_NEW(CommandPool, MEMORY_GFX_STRUCT);
		cp->SetQueue(GPUQueue::TRANSFER);
		CreateCommandPool(cp);
		cb = JOJO_NEW(CommandBuffer, MEMORY_GFX_STRUCT);
		cb->SetQueue(GPUQueue::TRANSFER);
		cb->SetPool(cp);
		CreateCommandBuffer(cb);
		//创建BackBuffer
		CreateBackBuffer();
		CreateDescriptorPool();
		//创建内建的Uniform
		CreatePerPassUniforms();
		//创建主RenderPass
		CreateMainRenderPass();
		//创建一些default的状态
		CreateDefaultStates();

		renderThread = new RenderThread(mContext, cmdBuffers, computeCmdBuffers);
		renderThread->SetGFXCommandList(cmdLists.cmdList);

		GFX_API gfxAPI = (GFX_API)GetSubsystem<GlobalConfig>()->GetConfig<U32>(CONFIG_GFX_API);
		bStagingBuffer = gfxAPI == GFX_API_VULKAN;
	}

	void Graphics::CreateDefaultStates()
	{
		ColorBlendState* cs = JOJO_NEW(ColorBlendState, MEMORY_GFX_STRUCT);
		ColorAttachmentState attachmentState;
		cs->GetAttachments().Push(attachmentState);
		CreateColorBlendState(cs);

		DepthStencilState* ds = JOJO_NEW(DepthStencilState, MEMORY_GFX_STRUCT);
		CreateDepthStencilState(ds);

		RasterizationState* rs = JOJO_NEW(RasterizationState, MEMORY_GFX_STRUCT);
		CreateRasterizationState(rs);

		MultiSampleState* ms = JOJO_NEW(MultiSampleState, MEMORY_GFX_STRUCT);
		CreateMultiSampleState(ms);
	}

	void Graphics::CreateMainRenderPass()
	{
		RenderPass* rp = JOJO_NEW(RenderPass, MEMORY_GFX_STRUCT);
		rp->SetClear(true);
		rp->SetLoadOp(AttachmentLoadOp::CLEAR);
		rp->SetStoreOp(AttachmentStoreOp::STORE);
		CreateRenderPass(rp);
	}

	void Graphics::SetUniformBuffer(UniformBuffer* uniform)
	{
		GetMainCmdList()->WriteCommand(GFXCommand::SetUniformBuffer);
		GetMainCmdList()->WriteBuffer(uniform->handle);
		U32 sz = uniform->GetSize();
		GetMainCmdList()->WriteBuffer(sz);
		GetMainCmdList()->WriteBufferPtr(uniform->GetBuffer(), sz);

		if (bStagingBuffer)
		{
			GetTransferCommandBuffer()->CopyBuffer(CopyBufferType::UB, uniform->handle);
		}
	}

	void Graphics::SetDescriptorSetLayout(DescriptorSetLayout* setLayout)
	{
		U32 hash = setLayout->Hash();
		if (mDescriptorSetLayouts.Contains(hash))
		{
			setLayout->SetHandle(mDescriptorSetLayouts[hash]->GetHandle());
		}
		else
		{
			U32 handle = mDescriptorSetLayouts.Size();
			setLayout->SetHandle(handle);
			mDescriptorSetLayouts.Insert(hash, setLayout);
			GetMainCmdList()->WriteCommand(GFXCommand::CreateDescriptorSetLayout);
			GetMainCmdList()->WriteBuffer(handle);
			GetMainCmdList()->WriteBuffer(setLayout->GetNumBindings());
			for (U32 i = 0; i < setLayout->GetNumBindings(); ++i)
			{
				DescriptorSetLayoutBinding* binding = setLayout->GetLayoutBinding(i);
				GPUDescriptorSetLayoutBinding bindingInfo
				{
					binding->binding,
					binding->type,
					binding->count,
					binding->stage,
					binding->size,
					binding->members.Size()
				};
				GetMainCmdList()->WriteBuffer(bindingInfo);
				for (U32 j = 0; j < binding->members.Size(); ++j)
				{ 
					GPUDescriptorSetLayoutBindingMember memberInfo
					{
						binding->members[j].ID, binding->members[j].offset, binding->members[j].size
					};
					GetMainCmdList()->WriteBuffer(memberInfo);
				}
			}
		}
	}

	void Graphics::CreatePipelineLayout(PipelineLayout* layout)
	{
		ASSIGN_NEW_HANDLE(layout, mPipelineLayouts);
		GetMainCmdList()->WriteCommand(GFXCommand::CreatePipelineLayout);
		GetMainCmdList()->WriteBuffer(handle);
		GPUPipelineLayoutCreateInfo createInfo{
			layout->GetLayoutsSize(),
			layout->GetPushConstantSize(),
		};
		GetMainCmdList()->WriteBuffer(createInfo);
		for (U32 i = 0; i < layout->GetLayoutsSize(); ++i)
		{
			GetMainCmdList()->WriteBuffer(layout->GetSetLayout(i)->GetHandle());
		}
	}

	void Graphics::CreateDescriptorSets(DescriptorSets* sets)
	{
		U32 handle = mDescriptorSets.Size();
		sets->SetHandle(handle);
		mDescriptorSets.Push(sets);
		GetMainCmdList()->WriteCommand(GFXCommand::CreateDescriptorSets);
		GetMainCmdList()->WriteBuffer(handle);
		GPUDescriptorSetsCreateInfo createInfo{
			sets->GetLayout()->GetHandle(),
		};
		GetMainCmdList()->WriteBuffer(createInfo);
	}

	void Graphics::UpdateDescriptorSets(DescriptorSets* sets)
	{
		GetMainCmdList()->WriteCommand(GFXCommand::UpdateDescriptorSets);
		GetMainCmdList()->WriteBuffer(sets->GetHandle());
		GPUDescriptorSetsUpdateInfo updateInfo{
			sets->Size()
		};
		GetMainCmdList()->WriteBuffer(updateInfo);
		for (U32 i = 0; i < sets->Size(); ++i)
		{
			DescriptorSet& set = sets->GetDescriptorSet(i);
			GPUDescriptorSetsUpdateInfo::Entry entry{
				sets->GetHandle(),
				set.binding,
				set.type,
				set.count,
				set.ub ? set.ub->GetHandle() : GPUResource::INVALID_HANDLE,
				set.texture ? set.texture->GetHandle() : GPUResource::INVALID_HANDLE
			};
			GetMainCmdList()->WriteBuffer(entry);
		}

	}

	GFXCommandList* Graphics::GetMainCmdList()
	{
		return mThreadCommandLists[0].cmdList[frameIdx % MAX_CMDLISTS_IN_FLIGHT];
	}

	void Graphics::WaitForRender()
	{
		while (GetMainCmdList()->readFlag)
		{
			Sleep(10);
		}
	}

	void Graphics::SubmitTransfer()
	{
		WaitForRender();
		CommandBuffer* cb = GetTransferCommandBuffer();
		if (cb->IsRecording())
		{
			cb->End();
		}
		Submit(cb);
	}

	void Graphics::SubmitRender()
	{
		WaitForRender();
		CommandBuffer* cb = GetMainCommandBuffer();
		if (cb->IsRecording())
		{
			cb->End();
		}
		QueueSubmit(cb);
	}

	void Graphics::Present()
	{
		WaitForRender();
		GetMainCmdList()->WriteCommand(GFXCommand::Present);
		return;
	}

	void Graphics::Flush() {
		WaitForRender();
		GetMainCmdList()->Flush();
		++frameIdx;
		return;
	}

	void Graphics::Clear() {
		cmdBuffer->WriteCommandType(RenderCMD_Clear);
		cmdBuffer->WriteBuffer<Vector4f>(defaultClearColor);
	}

	CommandBuffer* Graphics::GetMainCommandBuffer()
	{
		return mCommandBuffers[0];
	}

	CommandBuffer* Graphics::GetTransferCommandBuffer()
	{
		return mCommandBuffers[1];
	}

	void Graphics::CreateCommandPool(CommandPool* pool)
	{
		ASSIGN_NEW_HANDLE(pool, mCommandPools);
		GetMainCmdList()->WriteCommand(GFXCommand::CreateCommandPool);
		GetMainCmdList()->WriteBuffer<GPUResourceHandle>(handle);
		GetMainCmdList()->WriteBuffer(pool->GetQueue());
	}

	void Graphics::CreateCommandBuffer(CommandBuffer* cb)
	{
		GPUResourceHandle handle = mCommandBuffers.Size();
		cb->handle = handle;
		mCommandBuffers.Push(cb);
		GetMainCmdList()->WriteCommand(GFXCommand::CreateCommandBuffer);
		GetMainCmdList()->WriteBuffer<GPUResourceHandle>(handle);
		GPUCommandBufferCreateInfo createInfo
		{
			cb->GetQueue(),
			cb->GetPool()->GetHandle(),
			MAX_CMDLISTS_IN_FLIGHT
		};
		GetMainCmdList()->WriteBuffer(createInfo);
	}

	FrameBuffer* Graphics::GetBackBuffer()
	{
		return mFrameBuffers[0];
	}

	FrameBuffer* Graphics::CreateBackBuffer()
	{
		GPUResourceHandle handle = mFrameBuffers.Size();
		FrameBuffer* fb = JOJO_NEW(FrameBuffer);
		fb->handle = handle;
		mFrameBuffers.Push(fb);
		GetMainCmdList()->WriteCommand(GFXCommand::CreateFrameBuffer);
		GetMainCmdList()->WriteBuffer<GPUResourceHandle>(handle);

		GPUFrameBufferCreateInfo createInfo{
			1
		};
		GetMainCmdList()->WriteBuffer<GPUFrameBufferCreateInfo>(createInfo);
		return fb;
	}

	void Graphics::CreateFrameBuffer(FrameBuffer* fb)
	{
		ASSIGN_NEW_HANDLE(fb, mFrameBuffers);
		GetMainCmdList()->WriteCommand(GFXCommand::CreateFrameBuffer);
		GetMainCmdList()->WriteBuffer<GPUResourceHandle>(handle);

		U32 numColorAttachments = fb->GetNumColorAttachments();
		GPUFrameBufferCreateInfo createInfo{
			fb->GetMultiSample(),
			fb->GetWidth(),
			fb->GetHeight(),
			fb->GetLayers(),
			fb->GetRenderPass()->GetHandle(),
			fb->GetDepthStencil()->GetHandle(),
			numColorAttachments,
		};
		GetMainCmdList()->WriteBuffer<GPUFrameBufferCreateInfo>(createInfo);
		for (U32 i = 0; i < numColorAttachments; ++i)
		{
			GetMainCmdList()->WriteBuffer<GPUResourceHandle>(fb->GetColorAttachment(i)->GetHandle());
		}
	}

	GPUMemory* Graphics::CreateGPUMemory()
	{
		GPUResourceHandle handle = mGPUMemories.Size();
		GPUMemory* mem = JOJO_NEW(GPUMemory, MEMORY_GFX_STRUCT);
		mem->handle = handle;
		mGPUMemories.Push(mem);
		return mem;
	}

	void Graphics::RemoveGPUVertexBuffer(GPUVertexBuffer* vb)
	{

	}

	void Graphics::RemoveGPUIndexBuffer(GPUIndexBuffer* vb)
	{

	}

	GPUVertexBuffer* Graphics::CreateGPUVertexBuffer(VertexBuffer* vertexBuffer)
	{
		CREATE_NEW_HANDLE(vb, GPUVertexBuffer);
		vertexBuffer->SetGPUBuffer(vb);

		GPUMemory* mem = CreateGPUMemory();
		mem->size = vertexBuffer->GetSize();
		mem->data = vertexBuffer->GetData();
		SetGPUMemory(mem);

		PODVector<VertexElement>& elements = vertexBuffer->GetVertexElements();
		GPUVertexBufferCreateInfo createInfo
		{
			vertexBuffer->GetVertexCount(),
			vertexBuffer->GetVertexSize(),
			elements.Size(),
			mem->handle,
			bStagingBuffer
		};
		GetMainCmdList()->WriteCommand(GFXCommand::CreateVertexBuffer);
		GetMainCmdList()->WriteBuffer<GPUResourceHandle>(handle);
		GetMainCmdList()->WriteBuffer<GPUVertexBufferCreateInfo>(createInfo);

		for (auto& element : elements)
		{
			GetMainCmdList()->WriteBuffer<VertexElement>(element);
		}

		if (bStagingBuffer)
		{
			GetTransferCommandBuffer()->CopyBuffer(CopyBufferType::VB, handle);
		}
		
		return vb;
	}

	void Graphics::SetGPUMemory(GPUMemory* mem)
	{
		GetMainCmdList()->WriteCommand(GFXCommand::CreateMemory);
		GetMainCmdList()->WriteBuffer<U32>(mem->handle);
		GetMainCmdList()->WriteBuffer<U32>(mem->size);
		GetMainCmdList()->WriteBufferPtr(mem->data, mem->size);

	}

	GPUIndexBuffer* Graphics::CreateGPUIndexBuffer(IndexBuffer* indexBuffer)
	{
		CREATE_NEW_HANDLE(ib, GPUIndexBuffer);
		indexBuffer->SetGPUBuffer(ib);

		GPUMemory* mem = CreateGPUMemory();
		mem->size = indexBuffer->GetSize();
		mem->data = indexBuffer->GetData();
		SetGPUMemory(mem);

		GPUIndexBufferCreateInfo createInfo
		{
			indexBuffer->GetIndexCount(),
			indexBuffer->GetIndexSize(),
			mem->handle
		};
		GetMainCmdList()->WriteCommand(GFXCommand::CreateIndexBuffer);
		GetMainCmdList()->WriteBuffer<GPUResourceHandle>(handle);
		GetMainCmdList()->WriteBuffer<GPUIndexBufferCreateInfo>(createInfo);

		if (bStagingBuffer)
		{
			GetTransferCommandBuffer()->CopyBuffer(CopyBufferType::IB, handle);
		}

		return ib;
	}

	void Graphics::CreatePerPassUniforms()
	{
		//mPerPassUniformBuffers.Resize((U32)PerPassUniforms::COUNT);
	}

	void Graphics::CreateUniformBuffer(UniformBuffer* ub)
	{
		Vector<SharedPtr<UniformBuffer>>& buffers = mUniformBuffers;
		U32 handle = buffers.Size();
		buffers.Push(ub);
		ub->SetHandle(handle);

		GetMainCmdList()->WriteCommand(GFXCommand::CreateUniformBuffer);
		GetMainCmdList()->WriteBuffer<GPUResourceHandle>(handle);

		GPUUniformBufferCreateInfo createInfo{
			ub->Hash(),
			ub->GetSize()
		};
		GetMainCmdList()->WriteBuffer<GPUUniformBufferCreateInfo>(createInfo);
	}

	bool Graphics::GetGraphicsPipelineState(GraphicsPipelineState* pso)
	{
		pso->Rehash();
		for (auto& curPso : mGraphicsPSOs)
		{
			if (curPso->Hash() == pso->Hash())
			{
				pso->SetHandle(curPso->GetHandle());
				return true;
			}
		}
		return false;
	}

	void Graphics::CreateGraphicsPipelineState(GraphicsPipelineState* pso)
	{
		pso->Rehash();
		pso->handle = mGraphicsPSOs.Size();
		mGraphicsPSOs.Push(pso);
		GetMainCmdList()->WriteCommand(GFXCommand::CreateGraphicsPipelineState);
		GetMainCmdList()->WriteBuffer<GPUResourceHandle>(pso->handle);
		GPUGraphicsPipelineStateCreateInfo createInfo{};
		createInfo.shaderProramHandle = pso->GetShaderProgram()->GetHandle();
		createInfo.renderPassHandle = pso->GetRenderPass()->GetHandle();
		createInfo.colorBlendStateHandle = pso->GetColorBlendState()->GetHandle();
		createInfo.depthStencilStateHandle = pso->GetDepthStencilState()->GetHandle();
		createInfo.rasterizationStateHandle = pso->GetRasterizationState()->GetHandle();
		createInfo.multiSampleStateHandle = pso->GetMultiSampleState()->GetHandle();
		createInfo.pipelineLayoutHandle = pso->GetPipelineLayout()->GetHandle();
		//其实只要Elements定义应该就行了，这样复用率可以很高 --todo
		createInfo.numInputAttributes = pso->GetNumInputAttributes();
		createInfo.numInputBindings = pso->GetNumInputBindings();
		createInfo.viewport = *pso->GetViewport();

		GetMainCmdList()->WriteBuffer<GPUGraphicsPipelineStateCreateInfo>(createInfo);
		for (U32 i = 0; i < createInfo.numInputBindings; ++i)
		{
			InputBinding& binding = pso->GetInputBinding(i);
			GetMainCmdList()->WriteBuffer(binding);
		}

		for (U32 i = 0; i < createInfo.numInputAttributes; ++i)
		{
			InputAttribute& attr = pso->GetInputAttribute(i);
			GetMainCmdList()->WriteBuffer(attr);
		}
	}

	void Graphics::CreateColorBlendState(ColorBlendState* state)
	{
		state->Rehash();
		state->handle = mColorBlendStates.Size();
		mColorBlendStates.Push(state);
		GetMainCmdList()->WriteCommand(GFXCommand::CreateColorBlendState);
		GetMainCmdList()->WriteBuffer<GPUResourceHandle>(state->handle);
		U32 num = state->GetAttachments().Size();
		GPUColorBlendStateCreateInfo createInfo{
			state->GetLogicOpEnable(),
			num,
		};
		GetMainCmdList()->WriteBuffer<GPUColorBlendStateCreateInfo>(createInfo);

		for (U32 i = 0; i < num; ++i)
		{
			ColorAttachmentState attachmentState = state->GetAttachments()[i];
			GPUColorAttachmentStateCreateInfo attachCreateInfo
			{
				attachmentState.GetBlendEnable(),
				attachmentState.GetSrcColorBlendFactor(),
				attachmentState.GetDstColorBlendFactor(),
				attachmentState.GetColorBlendOp(),
				attachmentState.GetSrcAlphaBlendFactor(),
				attachmentState.GetDstAlphaBlendFactor(),
				attachmentState.GetAlphaBlendOp(),
				attachmentState.GetColorWriteMask()
			};
			GetMainCmdList()->WriteBuffer<GPUColorAttachmentStateCreateInfo>(attachCreateInfo);
		}
	}

	void Graphics::CreateDepthStencilState(DepthStencilState* state)
	{
		state->Rehash();
		//这种状态全局不会很多，就线性存储查询了。
		for (U32 i = 0; i < mDepthStencilStates.Size(); ++i)
		{
			if (mDepthStencilStates[i]->Hash() == state->Hash())
			{
				state->handle = mDepthStencilStates[i]->handle;
				return;
			}
		}
		state->handle = mDepthStencilStates.Size();
		mDepthStencilStates.Push(state);
		GetMainCmdList()->WriteCommand(GFXCommand::CreateDepthStencilState);
		GetMainCmdList()->WriteBuffer<GPUResourceHandle>(state->handle);
		GPUDepthStencilStateCreateInfo createInfo{
			state->GetDepthTest(),
			state->GetDepthWrite(),
			state->GetDepthCompareOp(),
			state->GetStencilTest(),
			state->GetStencilFront(),
			state->GetStencilBack()
		};
		GetMainCmdList()->WriteBuffer<GPUDepthStencilStateCreateInfo>(createInfo);
	}

	void Graphics::CreateRasterizationState(RasterizationState* state)
	{
		state->Rehash();
		state->handle = mRasterizationStates.Size();
		mRasterizationStates.Push(state);
		GetMainCmdList()->WriteCommand(GFXCommand::CreateRasterizationState);
		GetMainCmdList()->WriteBuffer<GPUResourceHandle>(state->handle);
		GPURasterizationStateCreateInfo createInfo{
			state->GetDepthClamp(),
			state->GetDiscardEnable(),
			state->GetPolygonMode(),
			state->GetLineWidth(),
			state->GetCullMode(),
			state->GetFrontFace(),
			state->GetDepthBias(),
			state->GetDepthBiasConstantFactor(),
			state->GetDepthBiasClamp(),
			state->GetDepthBiasSlopeFactor()
		};
		GetMainCmdList()->WriteBuffer<GPURasterizationStateCreateInfo>(createInfo);
	}

	void Graphics::CreateMultiSampleState(MultiSampleState* state)
	{
		state->Rehash();
		state->handle = mMultiSampleStates.Size();
		mMultiSampleStates.Push(state);
		GetMainCmdList()->WriteCommand(GFXCommand::CreateMultiSampleState);
		GetMainCmdList()->WriteBuffer<GPUResourceHandle>(state->handle);
		GPUMultiSampleStateCreateInfo createInfo{
			state->GetSampleShading(),
			state->GetRasterizationSamples(),
			state->GetMinSampleShading(),
			state->GetAlphaToCoverage(),
			state->GetAlphaToOne()
		};
		GetMainCmdList()->WriteBuffer<GPUMultiSampleStateCreateInfo>(createInfo);
	}

	void Graphics::CreateRenderPass(RenderPass* pass)
	{
		pass->Rehash();
		pass->handle = mRenderPasses.Size();
		mRenderPasses.Push(pass);
		GetMainCmdList()->WriteCommand(GFXCommand::CreateRenderPass);
		GetMainCmdList()->WriteBuffer<GPUResourceHandle>(pass->handle);
		GPURenderPassCreateInfo createInfo
		{
			pass->GetColorAttachmentCount(),
			pass->GetHasDepthStencil(),
			pass->GetDepthStencilFormat(),
			pass->GetDepthLoadOp(),
			pass->GetStencilLoadOp(),
			pass->GetDepthStoreOp(),
			pass->GetStencilStoreOp(),
			pass->GetClear(),
			1
		};
		GetMainCmdList()->WriteBuffer<GPURenderPassCreateInfo>(createInfo);
		for (U32 i = 0; i < pass->GetColorAttachmentCount(); ++i)
		{
			GetMainCmdList()->WriteBuffer(pass->GetColorFormat(i));
		}
		for (U32 i = 0; i < pass->GetColorAttachmentCount(); ++i)
		{
			GetMainCmdList()->WriteBuffer(pass->GetColorLoadOp(i));
		}
		for (U32 i = 0; i < pass->GetColorAttachmentCount(); ++i)
		{
			GetMainCmdList()->WriteBuffer(pass->GetColorStoreOp(i));
		}
	}

	RenderPass* Graphics::GetMainRenderPass()
	{
		return mRenderPasses[0];
	}

	void Graphics::CreateImage(GPUImage* image, U32 num)
	{
		ASSIGN_NEW_HANDLE(image, mImages);

		GPUMemory* mem = nullptr;
		if (image->GetSize())
		{
			mem = CreateGPUMemory();
			mem->size = image->GetSize();
			mem->data = image->GetData();
			SetGPUMemory(mem);
		}

		GetMainCmdList()->WriteCommand(GFXCommand::CreateImage);
		GetMainCmdList()->WriteBuffer<GPUResourceHandle>(handle);

		GPUImageCreateInfo createInfo{
			image->GetType(),
			image->GetFormat(),
			image->GetUsage(),
			image->GetWidth(),
			image->GetHeight(),
			image->GetDepth(),
			image->GetLayer(),
			image->GetMipLevels(),
			image->GetSamples(),
			num,
			mem ? mem->GetHandle() : GPUResource::INVALID_HANDLE
		};
		GetMainCmdList()->WriteBuffer<GPUImageCreateInfo>(createInfo);

		if (bStagingBuffer && mem)
		{
			GetTransferCommandBuffer()->TransitionImageLayout(handle, ImageLayout::UNDEFINED, ImageLayout::TRANSFER_DST_OPTIMAL, (U32)ImageAspectFlagBits::COLOR_BIT);
			GetTransferCommandBuffer()->CopyBufferToImage(handle, ImageLayout::TRANSFER_DST_OPTIMAL);
			GetTransferCommandBuffer()->TransitionImageLayout(handle, ImageLayout::TRANSFER_DST_OPTIMAL, ImageLayout::SHADER_READ_ONLY_OPTIMAL, (U32)ImageAspectFlagBits::COLOR_BIT);
		}
	}

	void Graphics::CreateImageView(GPUImageView* imageView, U32 num)
	{
		ASSIGN_NEW_HANDLE(imageView, mImageViews);
		GetMainCmdList()->WriteCommand(GFXCommand::CreateImageView);
		GetMainCmdList()->WriteBuffer<GPUResourceHandle>(handle);

		GPUImageViewCreateInfo createInfo{
			imageView->GetImage()->GetHandle(),
			imageView->GetType(),
			imageView->GetFormat(),
			imageView->GetAspectBits(),
			imageView->GetMipLevels(),
			imageView->GetBaseMipLevel(),
			imageView->GetLayer(),
			imageView->GetBaseLayer(),
			num
		};
		GetMainCmdList()->WriteBuffer<GPUImageViewCreateInfo>(createInfo);
	}

	void Graphics::CreateSwapChain()
	{
		mSwapChain = JOJO_NEW(SwapChain, MEMORY_GFX_STRUCT);

		GetMainCmdList()->WriteCommand(GFXCommand::CreateSwapChain);

		GPUSwapChainCreateInfo createInfo{
			mSwapChain
		};
		GetMainCmdList()->WriteBuffer<GPUSwapChainCreateInfo>(createInfo);
	}

	SwapChain* Graphics::GetSwapChain()
	{
		return mSwapChain;
	}

	void Graphics::CreateSyncObjects()
	{
		GetMainCmdList()->WriteCommand(GFXCommand::CreateSyncObjects);
		GetMainCmdList()->WriteBuffer<U32>(MAX_CMDLISTS_IN_FLIGHT);
	}

	void Graphics::CreateDescriptorPool()
	{
		GetMainCmdList()->WriteCommand(GFXCommand::CreateDescriptorPool);
		GetMainCmdList()->WriteBuffer<U32>(MAX_CMDLISTS_IN_FLIGHT);
	}

	void Graphics::CreateShader(Shader* shader)
	{
		shader->SetHandle(mShaders.Size());
		mShaders.Push(shader);
		GetMainCmdList()->WriteCommand(GFXCommand::CreateShader);
		GetMainCmdList()->WriteBuffer<GPUResourceHandle>(shader->GetHandle());
		GPUShaderCreateInfo createInfo
		{
			shader->GetStage(),
			shader->GetFile(),
			shader->GetBlob()
		};
		GetMainCmdList()->WriteBuffer<GPUShaderCreateInfo>(createInfo);
	}

	void Graphics::CreateShaderProgram(ShaderProgram* program)
	{
		program->SetHandle(mShaderPrograms.Size());
		mShaderPrograms.Push(program);
		GetMainCmdList()->WriteCommand(GFXCommand::CreateShaderProgram);
		GetMainCmdList()->WriteBuffer<GPUResourceHandle>(program->GetHandle());
		U32 numStages = program->GetNumStages();
		U32 numSetLayouts = program->GetNumDescriptorSetLayouts();
		GPUShaderProgramCreateInfo createInfo
		{
			program->GetStageMask(),
			numStages,
			numSetLayouts
		};
		GetMainCmdList()->WriteBuffer<GPUShaderProgramCreateInfo>(createInfo);
		for (U32 i = 0; i < numStages; ++i)
		{
			GetMainCmdList()->WriteBuffer<GPUResourceHandle>(program->GetShader(i)->GetHandle());
		}

		for (U32 i = 0; i < numSetLayouts; ++i)
		{
			GetMainCmdList()->WriteBuffer<GPUResourceHandle>(program->GetPipelineLayout()->GetSetLayout(i)->GetHandle());
		}
	}

	void Graphics::SetSampler(SharedPtr<Sampler>& sampler)
	{
		sampler->Rehash();
		U32 hash = sampler->Hash();
		if (mSamplerTable.Contains(hash))
		{
			sampler = mSamplerTable[hash];
			//sampler->SetHandle(mSamplerTable[hash]->GetHandle());
		}
		else
		{
			U32 handle = mSamplerTable.Size();
			sampler->SetHandle(handle);
			mSamplerTable.Insert(hash, sampler);
			GetMainCmdList()->WriteCommand(GFXCommand::CreateSampler);
			GetMainCmdList()->WriteBuffer(handle);
			GPUSamplerCreateInfo createInfo
			{
				sampler->GetMagFilter(),
				sampler->GetMinFilter(),
				sampler->GetMipMapFilter(),
				sampler->GetAddressModeU(),
				sampler->GetAddressModeV(),
				sampler->GetAddressModeW(),
				sampler->GetAnisotrophy(),
				sampler->GetMaxAnisotrophy(),
				sampler->GetCompare(),
				sampler->GetCompareOp(),
				sampler->GetMinLod(),
				sampler->GetMaxLod(),
				sampler->GetMipLodBias()
			};
			GetMainCmdList()->WriteBuffer(createInfo);
		}
	}


	void Graphics::CreateTexture(Texture* texture)
	{
		ASSIGN_NEW_HANDLE(texture, mTextures);
		GetMainCmdList()->WriteCommand(GFXCommand::CreateTexture);
		GetMainCmdList()->WriteBuffer(handle);
		GPUTextureCreateInfo createInfo{
			texture->GetImageView()->GetHandle(),
			texture->GetSampler() ? texture->GetSampler()->GetHandle() : GPUResource::INVALID_HANDLE
		};
		GetMainCmdList()->WriteBuffer(createInfo);

	}

	void Graphics::QueueSubmit(CommandBuffer* cb)
	{
		GetMainCmdList()->WriteCommand(GFXCommand::QueueSubmitCommandBuffer);
		GetMainCmdList()->WriteBuffer<GPUResourceHandle>(cb->GetHandle());
		CommandEncoder& encoder = cb->GetEncoder();
		encoder.Flush();
		GetMainCmdList()->WriteBuffer(encoder.GetSize());
		GetMainCmdList()->WriteBuffer(encoder.GetLast());
		GetMainCmdList()->WriteBufferPtr(encoder.Data(), encoder.GetSize());
		//清空
		encoder.Clear();
	}

	void Graphics::Submit(CommandBuffer* cb)
	{
		GetMainCmdList()->WriteCommand(GFXCommand::SubmitCommandBuffer);
		GetMainCmdList()->WriteBuffer<GPUResourceHandle>(cb->GetHandle());
		CommandEncoder& encoder = cb->GetEncoder();
		encoder.Flush();
		GetMainCmdList()->WriteBuffer(encoder.GetSize());
		GetMainCmdList()->WriteBuffer(encoder.GetLast());
		GetMainCmdList()->WriteBufferPtr(encoder.Data(), encoder.GetSize());
		//清空
		encoder.Clear();
	}
}
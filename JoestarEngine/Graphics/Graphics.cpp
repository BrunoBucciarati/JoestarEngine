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
	Graphics::~Graphics() {
	}

	void Graphics::Init() {
		//GlobalConfig* cfg = GetSubsystem<GlobalConfig>();
		//GFX_API gfxAPI = (GFX_API)cfg->GetConfig<int>(CONFIG_GFX_API);
		/*switch (gfxAPI) {

		}*/
			//auto* a1 = new RenderThreadVulkan(mContext, cmdBuffers, computeCmdBuffers);
		//if (gfxAPI == GFX_API_VULKAN) {
		//}
		//else if (gfxAPI == GFX_API_OPENGL) {
		//	//renderThread = new RenderThreadGL(cmdBuffers, computeCmdBuffers);
		//}
		//else if (gfxAPI == GFX_API_D3D11) {
		//	renderThread = new RenderThreadD3D11(cmdBuffers, computeCmdBuffers);
		//}

		//创建MainCmdList
		ThreadCommandList cmdLists;
		for (int i = 0; i < MAX_CMDLISTS_IN_FLIGHT; ++i)
		{
			cmdLists.cmdList[i] = JOJO_NEW(GFXCommandList);
		}
		mThreadCommandLists.Push(cmdLists);
		//创建交换链
		CreateSwapChain();
		//创建MainCommandBuffer
		CreateCommandBuffer();
		//创建BackBuffer
		CreateFrameBuffer();
		//创建内建的Uniform
		CreateBuiltinUniforms();
		//创建主RenderPass
		CreateMainRenderPass();

		renderThread = new RenderThread(mContext, cmdBuffers, computeCmdBuffers);
		renderThread->SetGFXCommandList(cmdLists.cmdList);
	}

	void Graphics::CreateMainRenderPass()
	{
		RenderPass* rp = JOJO_NEW(RenderPass, MEMORY_GFX_STRUCT);
		rp->SetClear(true);
		rp->SetLoadOP(AttachmentLoadOP::DONT_CARE);
		rp->SetStoreOP(AttachmentStoreOP::DONT_CARE);
		CreateRenderPass(rp);
	}

	void Graphics::CreateBuiltinUniforms()
	{
		for (U32 i = 0; i < (U32)BulitinUniforms::COUNT; ++i)
		{
			CreateGPUUniformBuffer(i);			
		}
	}

	GFXCommandList* Graphics::GetMainCmdList()
	{
		return mThreadCommandLists[0].cmdList[frameIdx % MAX_CMDLISTS_IN_FLIGHT];
	}

	void Graphics::MainLoop() {
		while (GetMainCmdList()->readFlag)
		{
			Sleep(10);
		}
		GetMainCmdList()->Flush();
		++frameIdx;
		return;
		//Flush cmd buffer from last frame
		//while (computeCmdBuffer->ready || cmdBuffer->ready) {
		//	//busy wait, must be both in unready state
		//}
		//if (!computeCmdBuffer->Empty() || !cmdBuffer->Empty()) {
		//	computeCmdBuffer->Flush();
		//	cmdBuffer->Flush();
		//}
		////renderThread->DrawFrame(cmdBuffer);
		////cmdBuffer->Clear();
		//U32 idx = ++frameIdx % MAX_CMDLISTS_IN_FLIGHT;
		//cmdBuffer = cmdBuffers[idx];
		//computeCmdBuffer = computeCmdBuffers[idx];
		//while (computeCmdBuffer->ready || cmdBuffer->ready) {
		//	//busy wait, must be both in unready state
		//}
		//cmdBuffer->Clear();
		//computeCmdBuffer->Clear();
	}

	void Graphics::Clear() {
		cmdBuffer->WriteCommandType(RenderCMD_Clear);
		cmdBuffer->WriteBuffer<Vector4f>(defaultClearColor);
	}

	void Graphics::UpdateBuiltinMatrix(BUILTIN_VALUE typ, Matrix4x4f& mat) {
		RenderCommandType t = typ == BUILTIN_MATRIX_MODEL ? RenderCMD_UpdateUniformBuffer : RenderCMD_UpdateUniformBufferObject;
		cmdBuffer->WriteBuffer<RenderCommandType>(t);
		cmdBuffer->WriteBuffer<BUILTIN_VALUE>(typ);
		cmdBuffer->WriteBuffer<Matrix4x4f>(mat);
	}
	void Graphics::FlushUniformBuffer(const char* s) {
		FlushUniformBuffer(hashString(s));
	}

	void Graphics::FlushUniformBuffer(U32 hash) {
		if (isCompute) {
			//computeCmdBuffer->WriteCommandType
			return;
		}
		cmdBuffer->WriteCommandType(RenderCMD_FlushUniformBufferObject);
		cmdBuffer->WriteBuffer<U32>(hash);
	}

	void Graphics::SetFrameBuffer(FrameBufferDef* def) {
		cmdBuffer->WriteCommandType(RenderCMD_SetFrameBuffer);
		cmdBuffer->WriteBuffer<FrameBufferDef*>(def);
	}

	void Graphics::UpdateBuiltinVec3(BUILTIN_VALUE typ, Vector3f& v3) {
		cmdBuffer->WriteCommandType(RenderCMD_UpdateUniformBufferObject);
		cmdBuffer->WriteBuffer<BUILTIN_VALUE>(typ);
		cmdBuffer->WriteBuffer<Vector3f>(v3);
	}

	void Graphics::UpdateLightBlock(LightBlocks& lb) {
		cmdBuffer->WriteCommandType(RenderCMD_UpdateUniformBufferObject);
		BUILTIN_VALUE bv = BUILTIN_STRUCT_LIGHTBLOCK;
		cmdBuffer->WriteBuffer<BUILTIN_VALUE>(bv);
		cmdBuffer->WriteBuffer<LightBlocks>(lb);
	}

	//void Graphics::UpdateVertexBuffer(VertexBuffer* vb) {
	//	cmdBuffer->WriteCommandType(RenderCMD_UpdateVertexBuffer);
	//	cmdBuffer->WriteBuffer<VertexBuffer*>(vb);
	//}

	//void Graphics::UpdateIndexBuffer(IndexBuffer* ib) {
	//	cmdBuffer->WriteCommandType(RenderCMD_UpdateIndexBuffer);
	//	cmdBuffer->WriteBuffer<IndexBuffer*>(ib);
	//}

	//void Graphics::UpdateInstanceBuffer(InstanceBuffer* ib) {
	//	cmdBuffer->WriteCommandType(RenderCMD_UpdateInstanceBuffer);
	//	cmdBuffer->WriteBuffer<InstanceBuffer*>(ib);
	//}

	void Graphics::DrawIndexed(Mesh* mesh, U32 count) {
		cmdBuffer->WriteCommandType(RenderCMD_DrawIndexed);
		cmdBuffer->WriteBuffer<U32>(count);
		MeshTopology topology = mesh->GetTopology();
		cmdBuffer->WriteBuffer<MeshTopology>(topology);
	}

	void Graphics::DrawArray(Mesh* mesh, U32 count) {
		cmdBuffer->WriteCommandType(RenderCMD_Draw);
		cmdBuffer->WriteBuffer<U32>(count);
		MeshTopology topology = mesh->GetTopology();
		cmdBuffer->WriteBuffer<MeshTopology>(topology);
	}


	void Graphics::UseShader(Shader* shader) {
		if (isCompute) {
			computeCmdBuffer->WriteCommandType(ComputeCMD_UseShader);
			computeCmdBuffer->WriteBuffer<Shader*>(shader);
		} else {
			cmdBuffer->WriteCommandType(RenderCMD_UseShader);
			cmdBuffer->WriteBuffer<Shader*>(shader);
		}

	}

	//update material
	void Graphics::UpdateMaterial(Material* mat) {
		UseShader(mat->GetShader());
		Vector<Texture*>& textures = mat->GetTextures();
		for (int i = 0; i < textures.Size(); i++) {
			UpdateTexture(textures[i], mat->GetShader()->GetSamplerBinding(i));
		}
	}

	void Graphics::UpdateTexture(Texture* t, U8 binding) {
		if (isCompute) {
			computeCmdBuffer->WriteCommandType(ComputeCMD_UpdateTexture);
			computeCmdBuffer->WriteBuffer<Texture*>(t);
			computeCmdBuffer->WriteBuffer<U8>(binding);
			return;
		}
		cmdBuffer->WriteCommandType(RenderCMD_UpdateTexture);
		cmdBuffer->WriteBuffer<Texture*>(t);
		cmdBuffer->WriteBuffer<U8>(binding);
	}

	void Graphics::UpdateProgram(ProgramCPU* p) {

	}

	//void Graphics::DrawMesh(Mesh* mesh, Material* mat) {
	//	//UpdateMaterial(mat);
	//	UpdateVertexBuffer(mesh->GetVB(mat->GetShader()->GetVertexAttributeFlag()));
	//	if (mesh->GetIB()->GetSize() > 0) {
	//		UpdateIndexBuffer(mesh->GetIB());
	//		DrawIndexed(mesh);
	//	} else {
	//		DrawArray(mesh);
	//	}
	//}

	//void Graphics::DrawMeshInstanced(Mesh* mesh, Material* mat, InstanceBuffer* ib) {
	//	//UpdateMaterial(mat);
	//	UpdateVertexBuffer(mesh->GetVB(mat->GetShader()->GetVertexAttributeFlag()));
	//	UpdateInstanceBuffer(ib);
	//	if (mesh->GetIB()->GetSize() > 0) {
	//		UpdateIndexBuffer(mesh->GetIB());
	//		DrawIndexed(mesh, ib->GetCount());
	//	}
	//	else {
	//		DrawArray(mesh, ib->GetCount());
	//	}
	//}

	void Graphics::BeginRenderPass(String name) {
		cmdBuffer->WriteCommandType(RenderCMD_BeginRenderPass);
		cmdBuffer->WriteBuffer<String>(name);
	}

	void Graphics::EndRenderPass(String name) {
		cmdBuffer->WriteCommandType(RenderCMD_EndRenderPass);
		cmdBuffer->WriteBuffer<String>(name);
	}

	void Graphics::SetDepthCompare(DepthCompareFunc func) {
		cmdBuffer->WriteCommandType(RenderCMD_SetDepthCompare);
		cmdBuffer->WriteBuffer<DepthCompareFunc>(func);
	}

	void Graphics::SetPolygonMode(PolygonMode mode) {
		cmdBuffer->WriteCommandType(RenderCMD_SetPolygonMode);
		cmdBuffer->WriteBuffer<PolygonMode>(mode);
	}


	void Graphics::BeginCompute(const char* name) {
		if (!computeCmdBuffer) {
			computeCmdBuffer = new GFXCommandBuffer(100);
		}
		computeCmdBuffer->WriteCommandType(ComputeCMD_BeginCompute);
		computeCmdBuffer->WriteBuffer<const char*>(name);
		isCompute = true;
	}

	void Graphics::DispatchCompute(U32 group[3]) {
		computeCmdBuffer->WriteCommandType(ComputeCMD_DispatchCompute);
		U32 sz = sizeof(U32) * 3;
		computeCmdBuffer->WriteBufferPtr((U8*)group, sz);
	}

	void Graphics::WriteBackComputeBuffer() {
		computeCmdBuffer->WriteCommandType(ComputeCMD_WriteBackComputeBuffer);
	}

	void Graphics::EndCompute(const char* name) {
		computeCmdBuffer->WriteCommandType(ComputeCMD_EndCompute);
		computeCmdBuffer->WriteBuffer<const char*>(name);
		//renderThread->DispatchCompute(computeCmdBuffer);
		isCompute = false;
	}

	//void Graphics::UpdateComputeBuffer(ComputeBuffer* cb, U8 binding) {
	//	computeCmdBuffer->WriteCommandType(ComputeCMD_UpdateComputeBuffer);
	//	computeCmdBuffer->WriteBuffer<ComputeBuffer*>(cb);
	//	computeCmdBuffer->WriteBuffer<U8>(binding);
	//}


	void Graphics::UpdatePushConstant(void* data, U32 size) {
		computeCmdBuffer->WriteCommandType(ComputeCMD_UpdatePushConstant);
		computeCmdBuffer->WriteBuffer<U32>(size);
		computeCmdBuffer->WriteBufferPtr((U8*)data, size);
	}

	CommandBuffer* Graphics::GetMainCommandBuffer()
	{
		return mCommandBuffers[0];
	}

	CommandBuffer* Graphics::CreateCommandBuffer()
	{
		GPUResourceHandle handle = mCommandBuffers.Size();
		CommandBuffer* cb = JOJO_NEW(CommandBuffer);
		cb->handle = handle;
		mCommandBuffers.Push(cb);
		GetMainCmdList()->WriteCommand(GFXCommand::CreateCommandBuffer);
		GetMainCmdList()->WriteBuffer<GPUResourceHandle>(handle);
		return cb;
	}

	FrameBuffer* Graphics::GetBackBuffer()
	{
		return mFrameBuffers[0];
	}

	FrameBuffer* Graphics::CreateFrameBuffer()
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

	GPUMemory* Graphics::CreateGPUMemory()
	{
		GPUResourceHandle handle = mGPUMemories.Size();
		GPUMemory* mem = JOJO_NEW(GPUMemory, MEMORY_GFX_STRUCT);
		mem->handle = handle;
		mGPUMemories.Push(mem);
		return mem;
	}

	GPUVertexBuffer* Graphics::CreateGPUVertexBuffer(VertexBuffer* vertexBuffer)
	{
		CREATE_NEW_HANDLE(vb, GPUVertexBuffer);
		vertexBuffer->SetGPUBuffer(vb);

		GPUMemory* mem = CreateGPUMemory();
		mem->size = vertexBuffer->GetSize();
		mem->data = vertexBuffer->Data();
		GetMainCmdList()->WriteCommand(GFXCommand::CreateMemory);
		GetMainCmdList()->WriteBuffer<U32>(mem->size);
		GetMainCmdList()->WriteBufferPtr(mem->data, mem->size);

		GPUVertexBufferCreateInfo createInfo
		{
			vertexBuffer->GetVertexCount(),
			vertexBuffer->GetVertexSize(),
			mem->handle
		};
		GetMainCmdList()->WriteCommand(GFXCommand::CreateIndexBuffer);
		GetMainCmdList()->WriteBuffer<GPUResourceHandle>(handle);
		GetMainCmdList()->WriteBuffer<GPUVertexBufferCreateInfo>(createInfo);
		
		return vb;
	}

	GPUIndexBuffer* Graphics::CreateGPUIndexBuffer(IndexBuffer* indexBuffer)
	{
		CREATE_NEW_HANDLE(ib, GPUIndexBuffer);
		indexBuffer->SetGPUBuffer(ib);

		GPUMemory* mem = CreateGPUMemory();
		mem->size = indexBuffer->GetSize();
		mem->data = indexBuffer->Data();
		GetMainCmdList()->WriteCommand(GFXCommand::CreateMemory);
		GetMainCmdList()->WriteBuffer<U32>(mem->size);
		GetMainCmdList()->WriteBufferPtr(mem->data, mem->size);


		GPUIndexBufferCreateInfo createInfo
		{
			indexBuffer->GetIndexCount(),
			indexBuffer->GetIndexSize(),
			mem->handle
		};
		GetMainCmdList()->WriteCommand(GFXCommand::CreateIndexBuffer);
		GetMainCmdList()->WriteBuffer<GPUResourceHandle>(handle);
		GetMainCmdList()->WriteBuffer<GPUIndexBufferCreateInfo>(createInfo);

		return ib;
	}

	GPUUniformBuffer* Graphics::CreateGPUUniformBuffer(const String& name)
	{
		return CreateGPUUniformBuffer(name.Hash());
	}

	GPUUniformBuffer* Graphics::CreateGPUUniformBuffer(U32 hash)
	{
		CREATE_NEW_HANDLE(ub, GPUUniformBuffer);
		GetMainCmdList()->WriteCommand(GFXCommand::CreateUniformBuffer);
		GetMainCmdList()->WriteBuffer<GPUResourceHandle>(handle);

		GPUUniformBufferCreateInfo createInfo{
			hash
		};
		GetMainCmdList()->WriteBuffer<GPUUniformBufferCreateInfo>(createInfo);

		return ub;
	}

	void Graphics::CreateRenderPass(RenderPass* pass)
	{
		pass->handle = mRenderPasses.Size();
		mRenderPasses.Push(pass);
		GetMainCmdList()->WriteCommand(GFXCommand::CreateRenderPass);
		GetMainCmdList()->WriteBuffer<GPUResourceHandle>(pass->handle);
		GPURenderPassCreateInfo createInfo{
			pass->GetColorFormat(),
			pass->GetDepthStencilFormat(),
			pass->GetColorLoadOP(),
			pass->GetDepthLoadOP(),
			pass->GetStencilLoadOP(),
			pass->GetColorStoreOP(),
			pass->GetDepthStoreOP(),
			pass->GetStencilStoreOP()
		};
		GetMainCmdList()->WriteBuffer<GPURenderPassCreateInfo>(createInfo);
	}

	RenderPass* Graphics::GetMainRenderPass()
	{
		return mRenderPasses[0];
	}

	void Graphics::CreateImage(GPUImage* image, U32 num = 1)
	{
		GPUResourceHandle handle = mImages.Size();
		image->handle = handle;
		mImages.Push(image);
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
			num
		};
		GetMainCmdList()->WriteBuffer<GPUImageCreateInfo>(createInfo);
	}

	void Graphics::CreateImageView(GPUImageView* imageView, U32 num = 1)
	{
		GPUResourceHandle handle = mImageViews.Size();
		imageView->handle = handle;
		mImageViews.Push(imageView);
		GetMainCmdList()->WriteCommand(GFXCommand::CreateImageView);
		GetMainCmdList()->WriteBuffer<GPUResourceHandle>(handle);

		GPUImageViewCreateInfo createInfo{
			imageView->image->handle,
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
		mSwapChain = JOJO_NEW(SwapChain);

		//GPUImage* img = JOJO_NEW(GPUImage(ImageType::TYPE_2D));
		//Window* window = GetSubsystem<Window>();
		//img->SetWidth(window->GetWidth());
		//img->SetHeight(window->GetHeight());
		//if (bFloatingPointRT)
		//{
		//	img->SetFormat(ImageFormat::RG11B10);
		//}
		//else
		//{
		//	img->SetFormat(ImageFormat::R8G8B8A8_SRGB);
		//}
		//img->SetUsage(U32(ImageUsageBits::COLOR_ATTACHMENT_BIT));
		//CreateImage(img, MAX_CMDLISTS_IN_FLIGHT);

		//GPUImageView* imgView = JOJO_NEW(GPUImageView(ImageViewType::TYPE_2D));
		//imgView->image = img;
		//imgView->SetAspectBits(U32(ImageAspectFlagBits::COLOR_BIT));
		//CreateImageView(imgView, MAX_CMDLISTS_IN_FLIGHT);

		//mSwapChain->imageView = imgView;

		GetMainCmdList()->WriteCommand(GFXCommand::CreateSwapChain);

		GPUSwapChainCreateInfo createInfo{
			//mSwapChain->imageView->handle
		};
		GetMainCmdList()->WriteBuffer<GPUSwapChainCreateInfo>(createInfo);
	}

	void Graphics::CreateSyncObjects()
	{
		GetMainCmdList()->WriteCommand(GFXCommand::CreateSyncObjects);
		GetMainCmdList()->WriteBuffer<U32>(MAX_CMDLISTS_IN_FLIGHT);
	}
}
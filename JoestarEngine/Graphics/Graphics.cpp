#include "Graphics.h"
#include "../Base/Scene.h"
#include "../Misc/GlobalConfig.h"
#include "../Thread/RenderThreadVulkan.h"
#include "../Thread/RenderThreadGL.h"
#include "../IO/MemoryManager.h"

namespace Joestar {
	Graphics::Graphics(EngineContext* context) : Super(context) {
		cmdBuffers.resize(MAX_CMDBUFFERS_IN_FLIGHT);
		for (auto& cmdBuffer : cmdBuffers)
			cmdBuffer = JOJO_NEW(GFXCommandBuffer(1024));
		cmdBuffer = cmdBuffers[0];

		computeCmdBuffers.resize(MAX_CMDBUFFERS_IN_FLIGHT);
		for (auto& cmdBuffer : computeCmdBuffers)
			cmdBuffer = JOJO_NEW(GFXCommandBuffer(256));
		computeCmdBuffer = computeCmdBuffers[0];
		defaultClearColor.Set(0.0f, 0.0f, 0.0f, 1.0f);
	}
	Graphics::~Graphics() {
	}

	void Graphics::Init() {
		GlobalConfig* cfg = GetSubsystem<GlobalConfig>();
		GFX_API gfxAPI = (GFX_API)cfg->GetConfig<int>("GFX_API");
		/*switch (gfxAPI) {

		}*/
		if (gfxAPI == GFX_API_VULKAN) {
			renderThread = new RenderThreadVulkan(cmdBuffers, computeCmdBuffers);
		}
		else if (gfxAPI == GFX_API_OPENGL) {
			renderThread = new RenderThreadGL();
		}
	}

	void Graphics::MainLoop() {
		//Flush cmd buffer from last frame
		if (!computeCmdBuffer->Empty() || !cmdBuffer->Empty()) {
			computeCmdBuffer->Flush();
			cmdBuffer->Flush();
		}
		//renderThread->DrawFrame(cmdBuffer);
		//cmdBuffer->Clear();
		U32 idx = ++frameIdx % MAX_CMDBUFFERS_IN_FLIGHT;
		cmdBuffer = cmdBuffers[idx];
		computeCmdBuffer = computeCmdBuffers[idx];
		while (computeCmdBuffer->ready || cmdBuffer->ready) {
			//busy wait, must be both in unready state
		}
		cmdBuffer->Clear();
		computeCmdBuffer->Clear();
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

	void Graphics::UpdateVertexBuffer(VertexBuffer* vb) {
		cmdBuffer->WriteCommandType(RenderCMD_UpdateVertexBuffer);
		cmdBuffer->WriteBuffer<VertexBuffer*>(vb);
	}

	void Graphics::UpdateIndexBuffer(IndexBuffer* ib) {
		cmdBuffer->WriteCommandType(RenderCMD_UpdateIndexBuffer);
		cmdBuffer->WriteBuffer<IndexBuffer*>(ib);
	}

	void Graphics::UpdateInstanceBuffer(InstanceBuffer* ib) {
		cmdBuffer->WriteCommandType(RenderCMD_UpdateInstanceBuffer);
		cmdBuffer->WriteBuffer<InstanceBuffer*>(ib);
	}

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
		std::vector<Texture*>& textures = mat->GetTextures();
		for (int i = 0; i < textures.size(); i++) {
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

	void Graphics::DrawMesh(Mesh* mesh, Material* mat) {
		//UpdateMaterial(mat);
		UpdateVertexBuffer(mesh->GetVB(mat->GetShader()->GetVertexAttributeFlag()));
		if (mesh->GetIB()->GetSize() > 0) {
			UpdateIndexBuffer(mesh->GetIB());
			DrawIndexed(mesh);
		} else {
			DrawArray(mesh);
		}
	}

	void Graphics::DrawMeshInstanced(Mesh* mesh, Material* mat, InstanceBuffer* ib) {
		//UpdateMaterial(mat);
		UpdateVertexBuffer(mesh->GetVB(mat->GetShader()->GetVertexAttributeFlag()));
		UpdateInstanceBuffer(ib);
		if (mesh->GetIB()->GetSize() > 0) {
			UpdateIndexBuffer(mesh->GetIB());
			DrawIndexed(mesh, ib->GetCount());
		}
		else {
			DrawArray(mesh, ib->GetCount());
		}
	}

	void Graphics::BeginRenderPass(std::string name) {
		cmdBuffer->WriteCommandType(RenderCMD_BeginRenderPass);
		cmdBuffer->WriteBuffer<std::string>(name);
	}

	void Graphics::EndRenderPass(std::string name) {
		cmdBuffer->WriteCommandType(RenderCMD_EndRenderPass);
		cmdBuffer->WriteBuffer<std::string>(name);
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
		computeCmdBuffer->WriteBufferPtr(group, sz);
	}

	void Graphics::WriteBackComputeBuffer() {
		computeCmdBuffer->WriteCommandType(ComputeCMD_WriteBackComputeBuffer);
	}

	void Graphics::EndCompute(const char* name) {
		computeCmdBuffer->WriteCommandType(ComputeCMD_EndCompute);
		computeCmdBuffer->WriteBuffer<const char*>(name);
		computeCmdBuffer->Flush();
		//renderThread->DispatchCompute(computeCmdBuffer);
		isCompute = false;
	}

	void Graphics::UpdateComputeBuffer(ComputeBuffer* cb, U8 binding) {
		computeCmdBuffer->WriteCommandType(ComputeCMD_UpdateComputeBuffer);
		computeCmdBuffer->WriteBuffer<ComputeBuffer*>(cb);
		computeCmdBuffer->WriteBuffer<U8>(binding);
	}


	void Graphics::UpdatePushConstant(void* data, U32 size) {
		computeCmdBuffer->WriteCommandType(ComputeCMD_UpdatePushConstant);
		computeCmdBuffer->WriteBuffer<U32>(size);
		computeCmdBuffer->WriteBufferPtr(data, size);
	}
}
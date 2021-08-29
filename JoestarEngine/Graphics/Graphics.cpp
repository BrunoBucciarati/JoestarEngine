#include "Graphics.h"
#include "../Base/Scene.h"
#include "../Misc/GlobalConfig.h"
#include "../Thread/RenderThreadVulkan.h"
#include "../Thread/RenderThreadGL.h"

namespace Joestar {
	Graphics::Graphics(EngineContext* context) : Super(context) {
		cmdBuffer.resize(1000);
		cmdIdx = 0;
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
			renderThread = new RenderThreadVulkan();
		}
		else if (gfxAPI == GFX_API_OPENGL) {
			renderThread = new RenderThreadGL();
		}
		renderThread->InitRenderContext();
	}

	void Graphics::MainLoop() {
		renderThread->DrawFrame(cmdBuffer, cmdIdx);
		//clear command buffer
		cmdIdx = 0;
	}

	void Graphics::Clear() {
		cmdBuffer[cmdIdx].typ = RenderCMD_Clear;
		cmdBuffer[cmdIdx].size = sizeof(defaultClearColor);
		cmdBuffer[cmdIdx].data = &defaultClearColor;
		++cmdIdx;
	}

	void Graphics::UpdateBuiltinMatrix(BUILTIN_MATRIX typ, Matrix4x4f& mat) {
		cmdBuffer[cmdIdx].typ = typ == BUILTIN_MATRIX_MODEL ? RenderCMD_UpdateUniformBuffer : RenderCMD_UpdateUniformBufferObject;
		//cmdBuffer[cmdIdx].typ = RenderCMD_UpdateUniformBufferObject;
		cmdBuffer[cmdIdx].flag = typ;
		cmdBuffer[cmdIdx].size = sizeof(mat);
		cmdBuffer[cmdIdx].data = mat.GetPtr();
		++cmdIdx;
	}

	void Graphics::UpdateVertexBuffer(VertexBuffer* vb) {
		cmdBuffer[cmdIdx].typ = RenderCMD_UpdateVertexBuffer;
		cmdBuffer[cmdIdx].size = sizeof(VertexBuffer*);// vb->GetSize();
		cmdBuffer[cmdIdx].data = vb;
		++cmdIdx;
	}

	void Graphics::UpdateIndexBuffer(IndexBuffer* ib) {
		cmdBuffer[cmdIdx].typ = RenderCMD_UpdateIndexBuffer;
		cmdBuffer[cmdIdx].size = sizeof(IndexBuffer*);
		cmdBuffer[cmdIdx].data = ib;
		++cmdIdx;
	}

	void Graphics::DrawIndexed(Mesh* mesh) {
		cmdBuffer[cmdIdx].typ = RenderCMD_DrawIndexed;
		cmdBuffer[cmdIdx].size = 0;
		cmdBuffer[cmdIdx].flag = mesh->GetTopology();
		++cmdIdx;
	}


	void Graphics::UseShader(const Shader* shader) {
		cmdBuffer[cmdIdx].typ = RenderCMD_UseShader;
		cmdBuffer[cmdIdx].size = sizeof(Shader*);
		cmdBuffer[cmdIdx].data = (void*)shader;
		++cmdIdx;

	}

	//update material
	void Graphics::UpdateMaterial(Material* mat) {
		UseShader(mat->GetShader());
		std::vector<Texture*>& textures = mat->GetTextures();
		for (int i = 0; i < textures.size(); i++) {
			UpdateTexture(textures[i], 1);
		}
	}

	void Graphics::UpdateTexture(Texture* t, U8 binding) {
		cmdBuffer[cmdIdx].typ = RenderCMD_UpdateTexture;
		cmdBuffer[cmdIdx].size = sizeof(Texture*);
		cmdBuffer[cmdIdx].flag = binding;
		cmdBuffer[cmdIdx].data = (void*)t;
		++cmdIdx;
	}

	void Graphics::UpdateProgram(ProgramCPU* p) {
		cmdBuffer[cmdIdx].typ = RenderCMD_UpdateProgram;
		cmdBuffer[cmdIdx].size = sizeof(ProgramCPU*);
		cmdBuffer[cmdIdx].data = (void*)p;
		++cmdIdx;
	}

	void Graphics::DrawMesh(Mesh* mesh, Material* mat) {
		UpdateMaterial(mat);
		UpdateVertexBuffer(mesh->GetVB(mat->GetShader()->GetVertexAttributeFlag()));
		UpdateIndexBuffer(mesh->GetIB());
		DrawIndexed(mesh);
	}

	void Graphics::BeginRenderPass(const char* name) {
		cmdBuffer[cmdIdx].typ = RenderCMD_BeginRenderPass;
		cmdBuffer[cmdIdx].size = sizeof(const char*);
		cmdBuffer[cmdIdx].data = (void*)name;
		++cmdIdx;
	}

	void Graphics::EndRenderPass(const char* name) {
		cmdBuffer[cmdIdx].typ = RenderCMD_EndRenderPass;
		cmdBuffer[cmdIdx].size = sizeof(const char*);
		cmdBuffer[cmdIdx].data = (void*)name;
		++cmdIdx;
	}
}
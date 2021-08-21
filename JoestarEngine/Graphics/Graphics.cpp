#include "Graphics.h"
#include "../Base/Scene.h"
#include "../Misc/GlobalConfig.h"
#include "../Thread/RenderThreadVulkan.h"
#include "../Thread/RenderThreadGL.h"

namespace Joestar {
	Graphics::Graphics(EngineContext* context) : Super(context) {
		cmdBuffer.resize(1000);
		cmdIdx = 0;
		defaultClearColor.Set(0.0f, 1.0f, 0.0f, 1.0f);
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
		cmdBuffer[cmdIdx].typ = RenderCMD_UpdateUniformBuffer;
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

	void Graphics::DrawIndexed() {
		cmdBuffer[cmdIdx].typ = RenderCMD_DrawIndexed;
		cmdBuffer[cmdIdx].size = 0;
		++cmdIdx;
	}


	void Graphics::UseShader(std::string& name) {
		cmdBuffer[cmdIdx].typ = RenderCMD_UseShader;
		cmdBuffer[cmdIdx].size = name.length();
		cmdBuffer[cmdIdx].data = (void*)name.c_str();
		++cmdIdx;

	}
}
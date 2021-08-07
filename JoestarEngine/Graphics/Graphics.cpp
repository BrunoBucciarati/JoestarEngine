#include "Graphics.h"
#include "GraphicDefines.h"
#include "../Base/Scene.h"
#include "../Misc/GlobalConfig.h"
#include "../Thread/RenderThreadVulkan.h"
#include "../Thread/RenderThreadGL.h"

namespace Joestar {
	static Scene* scene;
	Graphics::Graphics(EngineContext* context) : Super(context) {}
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
		//if (!scene) {
		//	scene = new Scene;
		//}
		//scene->RenderScene();
		renderThread->DrawFrame();
	}
}
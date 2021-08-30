#include "Application.h"
#include "../Base/EngineContext.h"
#include "../Base/SubSystem.h"
//#include "../Graphics/GraphicsGL.h"
//#include "../Graphics/GraphicsVulkan.h"
#include "GlobalConfig.h"
#include "../Graphics/GraphicDefines.h"
#include "../IO/HID.h"
#include "../Base/Scene.h"
#include "../Graphics/Shader/ShaderParser.h"
#include "../Graphics/ProceduralMesh.h"
#include "../IO/MemoryManager.h"
#include "TimeManager.h"

namespace Joestar {
	void Application::Start() {
		gContext = new EngineContext;
		InitSubSystem(MemoryManager, gContext)
		InitSubSystem(TimeManager, gContext)
		InitSubSystem(GlobalConfig, gContext)
		int GFX_API = GFX_API_VULKAN;
		GetSubSystem<GlobalConfig>()->UpdateConfig("GFX_API", GFX_API);
		int width = 800, height = 600;
		GetSubSystem<GlobalConfig>()->UpdateConfig("WINDOW_WIDTH", width);
		GetSubSystem<GlobalConfig>()->UpdateConfig("WINDOW_HEIGHT", height);
		//GetGlobalConfig()->UpdateConfig("GFX_API", str);

		InitSubSystem(Graphics, gContext)
		GetSubSystem<Graphics>()->Init();

		InitSubSystem(HID, gContext)
		InitSubSystem(ShaderParser, gContext)
		InitSubSystem(ProceduralMesh, gContext)
		InitSubSystem(Scene, gContext)
	}

	void Application::Run()
	{
		Start();
		while (true) {
			Update();
		}
	}

	void Application::Update() {
		float dt = GetSubSystem<TimeManager>()->GetElapseTime();
		GetSubSystem<TimeManager>()->BeginFrame();
		GetSubSystem<Scene>()->Update(dt);
		GetSubSystem<Graphics>()->MainLoop();
		GetSubSystem<TimeManager>()->EndFrame();
	}

	static Application* appInstance;
	Application* Application::GetApplication() {
		if (!appInstance) {
			appInstance = new Application;
		}
		return appInstance;
	}
}
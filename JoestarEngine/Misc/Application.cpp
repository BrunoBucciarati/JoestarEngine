#include "Application.h"
#include "../Core/EngineContext.h"
#include "../Core/SubSystem.h"
//#include "../Graphics/GraphicsGL.h"
//#include "../Graphics/GraphicsVulkan.h"
#include "GlobalConfig.h"
#include "../Graphics/GraphicDefines.h"
#include "../IO/HID.h"
#include "../Scene/Scene.h"
#include "../Graphics/Shader/ShaderParser.h"
#include "../Graphics/ProceduralMesh.h"
#include "../Graphics/Window.h"
#include "../IO/MemoryManager.h"
#include "../IO/FileSystem.h"
#include "TimeManager.h"

namespace Joestar {
	void Application::Start() {
		gContext = new EngineContext;
		InitSubSystem(MemoryManager, gContext)
		InitSubSystem(TimeManager, gContext)
		InitSubSystem(FileSystem, gContext)
		InitSubSystem(GlobalConfig, gContext)
		U32 GFX_API = GFX_API_VULKAN;
		GetSubSystem<GlobalConfig>()->UpdateConfig(CONFIG_GFX_API, GFX_API);
		U32 width = 800, height = 600;
		GetSubSystem<GlobalConfig>()->UpdateConfig(CONFIG_WINDOW_WIDTH, width);
		GetSubSystem<GlobalConfig>()->UpdateConfig(CONFIG_WINDOW_HEIGHT, height);
		InitSubSystem(Window, gContext)

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
		GetSubSystem<Window>()->Show();
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
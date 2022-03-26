#include "Application.h"
#include "../Core/EngineContext.h"
#include "../Core/SubSystem.h"
#include "GlobalConfig.h"
#include "../Graphics/GraphicDefines.h"
#include "../Graphics/View.h"
#include "../IO/HID.h"
#include "../Graphics/Shader/ShaderParser.h"
#include "../Graphics/ProceduralMesh.h"
#include "../Graphics/Window.h"
#include "../IO/MemoryManager.h"
#include "../IO/FileSystem.h"
#include "../Math/Rect.h"
#include "TimeManager.h"

namespace Joestar {
	F32 TARGET_FPS = 1.0F / 120.0F;
	void Application::Start() {
		gContext = JOJO_NEW(EngineContext);
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
		//InitSubSystem(ShaderParser, gContext)
		InitSubSystem(ProceduralMesh, gContext)

		mMainView = JOJO_NEW(View(gContext));
		Rect r = { 0.F, 0.F, (float)width, (float)height };
		mMainView->SetRect(r);
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
		mMainView->Update(dt);

		GetSubSystem<Graphics>()->MainLoop();

		mMainView->Render();
		GetSubSystem<TimeManager>()->EndFrame();
		if (dt < TARGET_FPS)
		{
			Sleep(1000.F*(TARGET_FPS - dt));
		}
	}

	static Application* appInstance;
	Application* Application::GetApplication() {
		if (!appInstance) {
			appInstance = new Application;
		}
		return appInstance;
	}
}
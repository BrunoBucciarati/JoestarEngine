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
		GetSubsystem<GlobalConfig>()->UpdateConfig(CONFIG_GFX_API, GFX_API);
		U32 width = 1280, height = 720;
		InitSubSystem(HID, gContext)
		GetSubsystem<GlobalConfig>()->UpdateConfig(CONFIG_WINDOW_WIDTH, width);
		GetSubsystem<GlobalConfig>()->UpdateConfig(CONFIG_WINDOW_HEIGHT, height);
		InitSubSystem(Window, gContext)

		InitSubSystem(Graphics, gContext)
		GetSubsystem<Graphics>()->Init();

		//InitSubSystem(ShaderParser, gContext)
		InitSubSystem(ProceduralMesh, gContext)

		mMainView = JOJO_NEW(View(gContext));
		mMainView->SetSwapChain(GetSubsystem<Graphics>()->GetSwapChain());
		Rect rect{ 0.0F, 0.0F, (F32)width, (F32)height };
		mMainView->SetRect(rect);
	}

	void Application::Run()
	{
		Start();
		GetSubsystem<Window>()->Show();
		while (true) {
			Update();
		}
	}

	void Application::Update() {
		float dt = GetSubsystem<TimeManager>()->GetElapseTime();
		//Input Update
		GetSubsystem<TimeManager>()->BeginFrame();
		GetSubsystem<HID>()->ClearInput();
		GetSubsystem<Window>()->Update(dt);

		Graphics* graphics = GetSubsystem<Graphics>();
		graphics->WaitForRender();
		//Logic Update,逻辑更新时可能会创建GPU数据，因此需要等待上一帧的渲染结束。
		mMainView->Update(dt);

		//Render Update
		mMainView->Render();
		graphics->SubmitTransfer();
		graphics->SubmitRender();
		graphics->Present();
		graphics->Flush();

		GetSubsystem<TimeManager>()->EndFrame();
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
#pragma once
#include "../Core/SubSystem.h"
#include <windows.h>
#include "Viewport.h"
namespace Joestar {
	class HID;
	class Window : public SubSystem {
	public:
		REGISTER_SUBSYSTEM(Window);
		explicit Window(EngineContext* ctx);
		const HINSTANCE& GetAppInstance() const
		{
			return mhAppInst;
		}
		const HWND& GetMainWindow() const
		{
			return mhMainWnd;
		}
		void Show();
		const U32 GetWidth() const
		{
			return mWidth;
		}
		const U32 GetHeight() const
		{
			return mHeight;
		}
		void Update(F32 dt);
	private:
		void InitWindow();
		U32 mWidth{ 0 };
		U32 mHeight{ 0 };
		HINSTANCE mhAppInst;     // 应用程序实例句柄
		HWND      mhMainWnd;     // 主窗口句柄
	};
}
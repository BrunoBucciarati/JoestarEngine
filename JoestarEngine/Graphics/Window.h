#pragma once
#include "../Base/SubSystem.h"
#include <windows.h>
namespace Joestar {
	class Window : public SubSystem {
	public:
		REGISTER_SUBSYSTEM(Window);
		explicit Window(EngineContext* ctx);
		const HINSTANCE& GetAppInstance() const {
			return mhAppInst;
		}
		const HWND& GetMainWindow() const {
			return mhMainWnd;
		}
		void Show();
	private:
		void InitWindow();
		U32 mWidth{ 0 };
		U32 mHeight{ 0 };
		HINSTANCE mhAppInst;     // Ӧ�ó���ʵ�����
		HWND      mhMainWnd;     // �����ھ��
	};
}
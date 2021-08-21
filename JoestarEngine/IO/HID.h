#pragma once
#include "../Base/SubSystem.h"
namespace Joestar {
	enum InputType {
		MOUSE_DOWN = 1,
		MOUSE_UP,
		KEY_W,
		KEY_S,
		KEY_A,
		KEY_D,
		KEY_ESCAPE
	};
	class HID : public SubSystem {
		REGISTER_SUBSYSTEM(HID)
	public:
		explicit HID(EngineContext* context);
		inline void SetKeyboardInputs(uint32_t flag) { inputFlag = flag; }
		void SetMouseInputs(float xOffset, float yOffset, float scrollX, float scrollY) {
			mouseOffset[0] = xOffset;
			mouseOffset[1] = yOffset;
			mouseScroll[0] = scrollX;
			mouseScroll[1] = scrollY;
		}
		inline uint32_t  GetKeyboardInputs() { return inputFlag; }
		inline float* GetMouseOffset() { return mouseOffset;}
		inline float* GetMouseScroll() { return mouseScroll;}
		inline bool CheckKeyboardInput(InputType t) {
			return inputFlag & (1 << t);
		}
	private:
		uint32_t inputFlag;
		float mouseOffset[2];
		float mouseScroll[2];
	};
}
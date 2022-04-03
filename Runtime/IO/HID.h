#pragma once
#include "../Core/SubSystem.h"
namespace Joestar {
	enum InputType {
		MOUSE_DOWN = 1,
		MOUSE_UP,
		KEY_W,
		KEY_S,
		KEY_A,
		KEY_D,
		KEY_UP,
		KEY_DOWN,
		KEY_LEFT,
		KEY_RIGHT,
		KEY_ESCAPE
	};
	class HID : public SubSystem {
		REGISTER_SUBSYSTEM(HID)
	public:
		explicit HID(EngineContext* context);
		void SetKeyboardInputs(U32 flag) { mInputFlag = flag; }
		void SetMouseInputs(float xOffset, float yOffset, float scrollX, float scrollY)
		{
			mouseOffset[0] = xOffset;
			mouseOffset[1] = yOffset;
			mouseScroll[0] = scrollX;
			mouseScroll[1] = scrollY;
		}
		U32  GetKeyboardInputs() { return mInputFlag; }
		float* GetMouseOffset() { return mouseOffset;}
		float* GetMouseScroll() { return mouseScroll;}
		bool CheckKeyboardInput(InputType t)
		{
			return mInputFlag & (1 << t);
		}
		void SetKeyboardInputBit(InputType bit)
		{
			mInputFlag |= (1 << bit);
		}
		void Clear()
		{
			mInputFlag = 0;
		}
	private:
		U32 mInputFlag;
		float mouseOffset[2];
		float mouseScroll[2];
	};
}
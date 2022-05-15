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
		void SetMouseInputs(F32 xOffset, F32 yOffset, F32 scrollX, F32 scrollY)
		{
			mouseOffset[0] = xOffset;
			mouseOffset[1] = yOffset;
			mouseScroll[0] = scrollX;
			mouseScroll[1] = scrollY;
		}
		U32  GetKeyboardInputs() { return mInputFlag; }
		F32* GetMouseOffset() { return mouseOffset;}
		F32* GetMouseScroll() { return mouseScroll;}
		bool CheckKeyboardInput(InputType t)
		{
			return mInputFlag & (1 << t);
		}
		void SetKeyboardInputBit(InputType bit)
		{
			mInputFlag |= (1 << bit);
		}
		void SetMouseRightDown(bool flag)
		{
			mMouseRightDown = flag;
		}
		void SetMouseLeftDown(bool flag)
		{
			mMouseLeftDown = flag;
		}
		bool IsMouseLeftDown()
		{
			return mMouseLeftDown;
		}
		bool IsMouseRightDown()
		{
			return mMouseRightDown;
		}
		void ClearInput()
		{
			mInputFlag = 0;
			mouseOffset[0] = 0.F;
			mouseOffset[1] = 0.F;
			mouseScroll[0] = 0.F;
			mouseScroll[1] = 0.F;
		}
	private:
		U32 mInputFlag;
		F32 mouseOffset[2];
		F32 mouseScroll[2];
		bool mMouseRightDown;
		bool mMouseLeftDown;
	};
}
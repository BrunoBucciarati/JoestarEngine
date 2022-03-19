#pragma once
#include "../Base/Platform.h"
#include "../IO/MemoryManager.h"
namespace Joestar
{
	template<class T>
	class Vector {
	public:
		Vector() = default;
		~Vector() {
			JOJO_DELETE_ARRAY(mBuffer);
		}
		void Resize(U32 sz) {
			Reserve(cap);
			mSize = sz;
		}
		void Reserve(U32 cap) {
			if (!mCapacity) {
				mCapacity = cap;
				mBuffer = JOJO_NEW_ARRAY(T, mCapacity, MEMORY_CONTAINER);
				return;
			}
			if (mCapacity && mCapacity < cap) {
				while (mCapacity < cap) {
					mCapacity = mCapacity << 1;
				}
				if (mBuffer) {
					JOJO_DELETE_ARRAY(mBuffer, MEMORY_CONTAINER);
				}
				mBuffer = JOJO_NEW_ARRAY(T, mCapacity, MEMORY_CONTAINER);
			}
		}
		U32 Size() {
			return mSize;
		}
		T& operator[](int index) {
			return mBuffer[index];
		}
	private:
		U32 mSize{ 0 };
		U32 mCapacity{ 0 };
		T* mBuffer;
	};
}
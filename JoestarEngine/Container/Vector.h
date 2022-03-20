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
			U32 oldSz = mSize;
			Reserve(sz);
			for (int i = oldSz; i < sz; ++i) {
				JOJO_PLACEMENT_NEW(T, mBuffer + i, MEMORY_CONTAINER);
			}
			mSize = sz;
		}
		void Reserve(U32 cap) {
			if (!mCapacity && cap) {
				mCapacity = cap;
				mBuffer = reinterpret_cast<T*>(JOJO_NEW_ARRAY(U8, mCapacity * sizeof(T), MEMORY_CONTAINER));
				return;
			}
			if (mCapacity && mCapacity < cap) {
				U32 oldCapacity = mCapacity;
				while (mCapacity < cap) {
					mCapacity = mCapacity << 1;
				}
				T* oldBuffer = mBuffer;
				mBuffer = reinterpret_cast<T*>(JOJO_NEW_ARRAY(U8, mCapacity * sizeof(T), MEMORY_CONTAINER));
				if (oldBuffer) {
					memcpy(mBuffer, oldBuffer, oldCapacity * sizeof(T));
					JOJO_DELETE_ARRAY(oldBuffer, MEMORY_CONTAINER);
				}
			}
		}
		U32 Size() {
			return mSize;
		}
		T& operator[](int index) {
			return mBuffer[index];
		}
		void Push(const T& value)
		{
			if (mSize >= mCapacity) {
				Reserve(mSize + 1);
			}
			JOJO_PLACEMENT_NEW(T(value), mBuffer + mSize, MEMORY_CONTAINER);
			++mSize;
		}
		void Clear() {
			mSize = 0;
		}
		T* Buffer() {
			return mBuffer;
		}
		T& Back() {
			return mBuffer[mSize - 1];
		}
		T& Begin() {
			return mBuffer[0];
		}

	private:
		U32 mSize{ 0 };
		U32 mCapacity{ 0 };
		T* mBuffer{nullptr};
	};
}
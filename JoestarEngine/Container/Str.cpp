#include "Str.h"
#include "../IO/MemoryManager.h"
namespace Joestar {
	String::String(const char* str) {
		*this = str;
	}

	String::~String() {
		JOJO_DELETE_ARRAY(mBuffer, MEMORY_STRING);
	}

	void String::CopyChars(char* start, const char* str, U32 length) {
		memcpy(start, str, length);
		start[length] = 0;
	}

	void String::Resize(U32 sz) {
		if (!mCapacity) {
			mCapacity = sz + 1;
			mBuffer = JOJO_NEW_ARRAY(char, mCapacity, MEMORY_STRING);
			return;
		}
		if (mCapacity && mCapacity < sz + 1) {
			while (mCapacity < sz + 1) {
				mCapacity = mCapacity << 1;
			}
			if (mBuffer) {
				JOJO_DELETE_ARRAY(mBuffer, MEMORY_STRING);
			}
			mBuffer = JOJO_NEW_ARRAY(char, mCapacity, MEMORY_STRING);
		}
	}
}
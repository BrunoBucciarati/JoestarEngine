#include "Str.h"
#include "../IO/MemoryManager.h"
namespace Joestar {
	U32 CStringLength(const char* str)
	{
		return str ? strlen(str) : 0;
	}
	String::String() : mLength(0) {
		Resize(0);
		mBuffer[0] = 0;
	}
	String::String(const char* str) {
		*this = str;
	}
	String::String(char c) {
		*this = c;
	}
	String::String(const String& rhs) {
		*this = rhs;
	}

	String::~String() {
		if (mBuffer) {
			JOJO_DELETE_ARRAY(mBuffer, MEMORY_STRING);
			mBuffer = nullptr;
		}
	}

	void String::CopyChars(char* start, const char* str, U32 length) {
		memcpy(start, str, length);
		start[length] = 0;
	}

	void String::Resize(U32 sz) {
		if (!mCapacity) {
			mCapacity = sz + 1;
			mBuffer = JOJO_NEW_ARRAY(char, mCapacity, MEMORY_STRING);
			mLength = sz;
			return;
		}
		if (mCapacity && mCapacity < sz + 1) {
			while (mCapacity < sz + 1) {
				mCapacity = mCapacity << 1;
			}
			char* oldBuffer = mBuffer;
			mBuffer = JOJO_NEW_ARRAY(char, mCapacity, MEMORY_STRING);
			memcpy(mBuffer, oldBuffer, mLength);
			mBuffer[mLength] = 0;
			if (oldBuffer) {
				JOJO_DELETE_ARRAY(oldBuffer, MEMORY_STRING);
			}
		}
		mLength = sz;
	}

	String String::SubString(U32 start, U32 length) {
		String ret;
		ret.Resize(length);
		CopyChars(ret.mBuffer, mBuffer + start, length);
		return ret;
	}

	U32 String::Find(String & str) const {
		U32 length = str.mLength;
		for (int i = 0; i < mLength - length; ++i) {
			if (mBuffer[i] == str[0]) {
				bool bFound = true;
				for (int j = 0; j < length; ++j) {
					if (mBuffer[i + j] != str[j]) {
						bFound = false;
						break;
					}
				}
				if (bFound) {
					return i;
				}
			}
		}
		return INDEX_NONE;
	}

	/// Char Add a string.
	String operator+(const char* lhs, const String& rhs)
	{
		String ret;
		ret.Resize(CStringLength(lhs) + rhs.mLength);
		String::CopyChars(ret.mBuffer, lhs, CStringLength(lhs));
		String::CopyChars(ret.mBuffer + CStringLength(lhs), rhs.mBuffer, rhs.mLength);

		return ret;
	}
}
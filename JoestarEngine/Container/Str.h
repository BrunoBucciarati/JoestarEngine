#pragma once
#include "../Base/Platform.h"
#include "../Base/StringHash.h"
#include <cstring>
namespace Joestar {
	U32 CStringLength(const char* str) {
		return str ? strlen(str) : 0;
	}
	class String {
	public:
		String() : mLength(0), mBuffer(nullptr) {}
		String(const char* t);
		~String();
		void Resize(U32 sz);
		static void CopyChars(char* start, const char* str, U32 length);
		const char* CString() const {
			return mBuffer;
		}
		U32 Length() {
			return mLength;
		}
		bool Empty() {
			return 0 == mLength;
		}

		String& operator =(const char* rhs)
		{
			unsigned rhsLength = CStringLength(rhs);
			CopyChars(mBuffer, rhs, rhsLength);

			return *this;
		}

		/// Add-assign a string.
		String& operator +=(const String& rhs)
		{
			unsigned oldLength = mLength;
			mLength = oldLength + mLength;
			Resize(mLength);
			CopyChars(mBuffer + oldLength, rhs.mBuffer, rhs.mLength);

			return *this;
		}

		/// Add a string.
		String operator +(const String& rhs) const
		{
			String ret;
			ret.Resize(mLength + rhs.mLength);
			CopyChars(ret.mBuffer, mBuffer, mLength);
			CopyChars(ret.mBuffer + mLength, rhs.mBuffer, rhs.mLength);

			return ret;
		}

		U32 Hash() const {
			return hashString(CString());
		}
	private:
		U32 mLength{0};
		U32 mCapacity{0};
		char* mBuffer;
	};
}
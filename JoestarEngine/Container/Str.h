#pragma once
#include "../Core/Platform.h"
#include "../Base/StringHash.h"
#include <cstring>
namespace Joestar {
	U32 CStringLength(const char* str);
	class String {
	public:
		String();
		String(const char* t);
		String(char c);
		String(const String& rhs);
		~String();
		void Resize(U32 sz);
		static void CopyChars(char* start, const char* str, U32 length);
		const char* CString() const {
			return mBuffer;
		}
		U32 Length() const {
			return mLength;
		}
		bool Empty() {
			return 0 == mLength;
		}

		String& operator =(const String& rhs)
		{
			Resize(rhs.mLength);
			CopyChars(mBuffer, rhs.mBuffer, mLength);
			return *this;
		}

		String& operator =(const char* rhs)
		{
			unsigned rhsLength = CStringLength(rhs);
			Resize(rhsLength);
			CopyChars(mBuffer, rhs, rhsLength);

			return *this;
		}

		String& operator =(char c)
		{
			Resize(1);
			CopyChars(mBuffer, &c, 1);

			return *this;
		}

		/// Add-assign a string.
		String& operator +=(const String& rhs)
		{
			unsigned oldLength = mLength;
			Resize(mLength + rhs.mLength);
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

		bool operator ==(const String& rhs) const
		{
			return strcmp(CString(), rhs.CString()) == 0;
		}

		bool operator !=(const String& rhs) const
		{
			return strcmp(CString(), rhs.CString()) != 0;
		}

		char& operator[](int index) {
			return mBuffer[index];
		}

		U32 Hash() const {
			return hashString(CString());
		}

		U32 Find(const char* str) const {
			String s(str);
			return Find(s);
		}

		U32 Find(String&& str) const {
			return Find(str);
		}

		U32 Find(String& str) const;

		String SubString(U32 start, U32 length);

		friend String operator+(const char* lhs, const String& rhs);
	private:
		U32 mLength{0};
		U32 mCapacity{0};
		char* mBuffer{ nullptr };
	};
}
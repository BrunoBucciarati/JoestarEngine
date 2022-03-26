#pragma once
#include <fstream>
#include "../Core/Minimal.h"
namespace Joestar {
	class File : public Object {
		REGISTER_OBJECT_ROOT(File);
	public:
		explicit File(EngineContext* ctx, const String& filename);
		inline bool IsReady()
		{
			return mReady;
		}
		inline char* GetBuffer()
		{
			return mBuffer;
		}
		void Open(const String& filename);
		void Close();
		void Read(U32=0);
		void Write(const char* data, U32 size);
		void Seek(U32);
		U32 Size() { return mSize; }
		U32 Tell();
		bool Exist()
		{
			return !!mBuffer;
		}
		void SetWrite(bool flag)
		{
			bWrite = flag;
		}
	private:
		char* mBuffer{ nullptr };
		bool mReady;
		U32 mSize;
		std::fstream mFile;
		String mPath;
		bool bAsync{ false };
		bool bWrite{ false };
		bool bRead{ true};
	};
}
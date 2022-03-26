#include "File.h"
#include "Log.h"
#include <fstream>
namespace Joestar {
	File::File(EngineContext* ctx, const String& filename) : Super(ctx)
	{
		mSize = 0;
		mPath = filename;
		Open(filename);

		if (!mFile.is_open())
		{
			LOGERROR("failed to open file: %s!", filename);
			return;
		}
		else
		{
			int a = 1;
		}

		if (bAsync)
			mReady = true;
		U32 s = Tell();
		Seek(0);
		if (!bWrite)
		{
			Read(s);
			Close();
		}
	}
	File::~File()
	{
		delete mBuffer;
	}

	void File::Open(const String& filename)
	{
		if (bWrite)
		{
			mFile.open(filename.CString(), std::ios::out | std::ios::binary);
		}
		else
		{
			mFile.open(filename.CString(), std::ios::in | std::ios::ate | std::ios::binary);
		}
	}

	void File::Close()
	{
		mFile.close();
	}

	void File::Read(U32 size)
	{
		if (!size)
			size = Size();
		mBuffer = JOJO_NEW_ARRAY(char, size);
		memset(mBuffer, 0, size);
		auto& res = mFile.read((char*)mBuffer, size);
		int c = 1;
	}

	void File::Write(const char* data, U32 size)
	{
		mFile.write(data, size);
	}

	U32 File::Tell()
	{
		mSize = (U32)mFile.tellg();
		return mSize;
	}

	void File::Seek(U32 pos)
	{
		mFile.seekg(pos);
	}
}
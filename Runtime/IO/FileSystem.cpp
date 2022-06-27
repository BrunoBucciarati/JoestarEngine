#include "FileSystem.h"
namespace Joestar {
	FileSystem::~FileSystem()
	{}

	void FileSystem::ReadFileAsync(const String & filename, ReadFileCallback callback)
	{
		File file(mContext, filename);

		//先写成同步的，不过按回调机制处理，后续可以改异步IO
		if (file.IsReady())
		{
			callback(&file);
		}
	}

	SharedPtr<File> FileSystem::ReadFile(const String& filename)
	{
		if (mFileMap.Contains(filename) && mFileMap[filename])
			return mFileMap[filename];
		File* file = NEW_OBJECT(File, filename);
		mFileMap[filename] = file;
		return file;
	}

	SharedPtr<File> FileSystem::OpenFile(const String& filename)
	{
		if (mFileMap.Contains(filename) && mFileMap[filename])
			return mFileMap[filename];
		File* file = NEW_OBJECT(File, filename);
		mFileMap[filename] = file;
		return file;
	}

	bool FileSystem::WriteFile(const String& filePath, String& data)
	{
		File* file = NEW_OBJECT(File, filePath);
		file->SetWrite(true);
		file->Seek(0);
		file->Write(data.CString(), data.Length());
		file->Close();
		return true;
	}

	SharedPtr<File> FileSystem::GetShaderCodeFile(const String& filename)
	{
		if (mFileMap.Contains(filename) && mFileMap[filename])
			return mFileMap[filename];

		File* f = ReadFile(filename);
		f->Close();
		mFileMap[filename] = f;
		return f;
	}
}
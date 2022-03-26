#include "FileSystem.h"
namespace Joestar {
	FileSystem::~FileSystem()
	{}

	void FileSystem::ReadFileAsync(const String & filename, ReadFileCallback callback)
	{
		File file(mContext, filename);

		//��д��ͬ���ģ��������ص����ƴ����������Ը��첽IO
		if (file.IsReady()) {
			callback(&file);
		}
	}

	File* FileSystem::ReadFile(const String& filename)
	{
		File* file = NEW_OBJECT(File, filename);
		return file;
	}

	File* FileSystem::OpenFile(const String& filename)
	{
		File* file = NEW_OBJECT(File, filename);
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


	File* FileSystem::GetShaderCodeFile(const String& file)
	{
		File* f = ReadFile(file);
		f->Close();
		return f;
	}
}
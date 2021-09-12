#include "FileSystem.h"
namespace Joestar {
	void FileSystem::ReadFileAsync(const char* filename, ReadFileCallback callback) {
		File file(filename, true);
		
		//��д��ͬ���ģ��������ص����ƴ����������Ը��첽IO
		if (file.IsReady()) {
			callback(&file);
		}
	}
	FileSystem::~FileSystem() {}

	File* FileSystem::ReadFile(const char* filename) {
		File* file = new File(filename);
		return file;
	}

	File* FileSystem::OpenFile(const char* filename) {
		File* file = new File(filename);
		return file;
	}

	bool FileSystem::WriteFile(const char* filePath, std::string& data) {
		File* file = new File(filePath, true);
		file->Seek(0);
		file->Write(data.c_str(), data.length());
		file->Close();
		return true;
	}


	File* FileSystem::GetShaderCodeFile(const char* file) {
		std::string path = GetResourceDir();
		path += "Shaders/";
		path += file;
		File* f = ReadFile(path.c_str());
		f->Close();
		return f;
	}
}
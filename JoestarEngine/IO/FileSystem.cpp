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

	File* FileSystem::GetShaderCodeFile(const char* file) {
		std::string path = GetResourceDir();
		path += "Shaders/";
		path += file;
		File* f = ReadFile(path.c_str());
		f->Close();
		return f;
	}
}
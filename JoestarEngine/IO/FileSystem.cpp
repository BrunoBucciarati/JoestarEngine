#include "FileSystem.h"
namespace Joestar {
	void FileSystem::ReadFileAsync(const char* filename, ReadFileCallback callback) {
		File file(filename, true);
		
		//先写成同步的，不过按回调机制处理，后续可以改异步IO
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

	bool FileSystem::WriteFile(const char* filePath, String& data) {
		File* file = new File(filePath, true);
		file->Seek(0);
		file->Write(data.CString(), data.Length());
		file->Close();
		return true;
	}


	File* FileSystem::GetShaderCodeFile(const char* file) {
		File* f = ReadFile(file);
		f->Close();
		return f;
	}
}
#include "FileSystem.h"
namespace Joestar {
	void FileSystem::ReadFileAsync(const char* filename, ReadFileCallback callback) {
		File file(filename, true);
		
		//��д��ͬ���ģ��������ص����ƴ����������Ը��첽IO
		if (file.IsReady()) {
			callback(&file);
		}
	}

	File* FileSystem::ReadFile(const char* filename) {
		File* file = new File(filename);
		return file;
	}
}
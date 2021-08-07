#include "FileSystem.h"
namespace Joestar {
	void FileSystem::ReadFileAsync(const char* filename, ReadFileCallback callback) {
		File file(filename, true);
		
		//先写成同步的，不过按回调机制处理，后续可以改异步IO
		if (file.IsReady()) {
			callback(&file);
		}
	}

	File* FileSystem::ReadFile(const char* filename) {
		File* file = new File(filename);
		return file;
	}
}
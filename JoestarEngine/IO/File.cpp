#include "File.h"
#include "Log.h"
#include <fstream>
#include <direct.h>//---getcwd
namespace Joestar {
	File::File(const char* filename, bool async) {
		mSize = 0;
		Open(filename);

		if (!file.is_open()) {
			LOGERROR("failed to open file: %s!", filename);
			return;
		}

		if (async)
			mReady = true;
		size_t s = Tell();
		Seek(0);
		Read(s);
		Close();
	}
	File::~File() {
		delete mBuffer;
	}

	void File::Open(const char* filename) {
		file.open(filename, std::ios::ate | std::ios::binary);
	}

	void File::Close() {
		file.close();
	}

	void File::Read(size_t size) {
		mBuffer = new char[size];
		file.read((char*)mBuffer, size);
	}

	size_t File::Tell() {
		mSize = (size_t)file.tellg();
		return mSize;
	}

	void File::Seek(size_t pos) {
		file.seekg(pos);
	}
}
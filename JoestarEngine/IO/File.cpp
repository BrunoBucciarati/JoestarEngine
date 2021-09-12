#include "File.h"
#include "Log.h"
#include <fstream>
namespace Joestar {
	File::File(const char* filename, bool write, bool async) {
		mSize = 0;
		Open(filename, write);

		if (!file.is_open()) {
			LOGERROR("failed to open file: %s!", filename);
			return;
		}

		if (async)
			mReady = true;
		size_t s = Tell();
		Seek(0);
		Read(s);
		if (!write)
			Close();
	}
	File::~File() {
		delete mBuffer;
	}

	void File::Open(const char* filename, bool write) {
		if (write) {
			file.open(filename, std::ios::out | std::ios::binary);
		} else {
			file.open(filename, std::ios::in | std::ios::ate | std::ios::binary);
		}
	}

	void File::Close() {
		file.close();
	}

	void File::Read(size_t size) {
		mBuffer = new char[size+1];
		memset(mBuffer, 0, size+1);
		file.read((char*)mBuffer, size);
	}

	void File::Write(const char* data, size_t size) {
		file.write(data, size);
	}

	size_t File::Tell() {
		mSize = (size_t)file.tellg();
		return mSize;
	}

	void File::Seek(size_t pos) {
		file.seekg(pos);
	}
}
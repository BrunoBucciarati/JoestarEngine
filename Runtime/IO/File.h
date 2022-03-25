#pragma once
#include <fstream>
namespace Joestar {
	class File {
	public:
		explicit File(const char* filename, bool write = false, bool async = false);
		~File();
		inline bool IsReady() {
			return mReady;
		}
		inline char* GetBuffer() {
			return mBuffer;
		}
		void Open(const char* filename, bool write = false);
		void Close();
		void Read(size_t);
		void Write(const char* data, size_t size);
		void Seek(size_t);
		size_t Size() { return mSize; }
		size_t Tell();
	private:
		char* mBuffer;
		bool mReady;
		size_t mSize;
		std::fstream file;
		std::string path;
	};
}
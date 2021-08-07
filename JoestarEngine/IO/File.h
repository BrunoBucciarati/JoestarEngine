#pragma once
#include <fstream>
namespace Joestar {
	class File {
	public:
		explicit File(const char* filename, bool async = false);
		~File();
		inline bool IsReady() {
			return mReady;
		}
		inline void* GetBuffer() {
			return mBuffer;
		}
		void Open(const char* filename);
		void Close();
		void Read(size_t);
		void Seek(size_t);
		size_t Size() { return mSize; }
		size_t Tell();
	private:
		void* mBuffer;
		bool mReady;
		size_t mSize;
		std::ifstream file;
	};
}
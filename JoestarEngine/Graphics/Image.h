#pragma once
namespace Joestar {
	class Image {
	public:
		Image(const char* path);
		~Image();
		inline int GetWidth() { return width; }
		inline int GetHeight() { return height; }
		inline int GetSize() { return width * height; }
		inline char* GetData() { return data; }
	private:
		int width, height, channels;
		char* data;
	};
}
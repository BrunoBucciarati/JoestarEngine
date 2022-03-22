#pragma once
#include "../Core/Object.h"
namespace Joestar {
	class Image : public Object {
		REGISTER_OBJECT(Image, Object)
	public:
		explicit Image(EngineContext* ctx) : Super(ctx) {}
		void Load(const char* path);
		inline int GetWidth() { return width; }
		inline int GetHeight() { return height; }
		inline int GetSize() { return width * height; }
		inline char* GetData() { return data; }
	private:
		int width, height, channels;
		char* data;
	};
}
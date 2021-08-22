#pragma once
#include <string>
#include "Image.h"
#include "../Base/Object.h"
#include "GraphicDefines.h"
namespace Joestar {
	class Texture : public Object {
		REGISTER_OBJECT(Texture, Object)
	public:
		explicit Texture(EngineContext* ctx) : Super(ctx), hasMipMap(true) {
			
		}
		void TextureFromImage(std::string& path);
		uint32_t id;
		TEXTURE_FORMAT fmt;
		bool hasMipMap;
		inline uint32_t GetSize() { return width * height * TEXTURE_FORMAT_SIZE[fmt]; }
		inline uint32_t GetWidth() { return width; }
		inline uint32_t GetHeight() { return height; }
	private:
		Image* img;
		uint32_t width;
		uint32_t height;
	};
}
#pragma once
#include "Image.h"
#include "../Core/Object.h"
#include "GraphicDefines.h"
namespace Joestar {
	class Texture : public Object {
		REGISTER_OBJECT(Texture, Object)
	public:
		explicit Texture(EngineContext* ctx) : Super(ctx), hasMipMap(true) {
			
		}
		virtual void TextureFromImage(String& path) {}
		U32 id;
		TEXTURE_FORMAT fmt;
		TEXTURE_TYPE typ;
		bool hasMipMap;
		virtual U32 GetSize() { return width * height * TEXTURE_FORMAT_SIZE[fmt]; }
		virtual U32 GetWidth() { return width; }
		virtual U32 GetHeight() { return height; }
		virtual U8* GetData() { return nullptr; }
	protected:
		U32 width;
		U32 height;
	};
}
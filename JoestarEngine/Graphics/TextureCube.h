#pragma once
#include "Texture.h"
#include <vector>

namespace Joestar {
	class TextureCube : public Texture {
		REGISTER_OBJECT(TextureCube, Texture)
	public:
		explicit TextureCube(EngineContext* ctx) : Texture(ctx) {
			typ = TEXTURE_CUBEMAP;
		}
		void TextureFromImage(std::string& path);
		virtual U32 GetSize() { return 6 * width * height * TEXTURE_FORMAT_SIZE[fmt]; }
		U8* GetData();
	private:
		std::vector<Image*> imgs;
		U8* buffer;
	};
}
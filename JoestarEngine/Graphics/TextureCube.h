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
		U8* GetData();
	private:
		std::vector<Image*> imgs;
	};
}
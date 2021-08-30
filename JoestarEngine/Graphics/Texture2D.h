#pragma once
#include "Texture.h"

namespace Joestar {
	class Texture2D : public Texture {
		REGISTER_OBJECT(Texture2D, Texture)
	public:
		explicit Texture2D(EngineContext* ctx) : Texture(ctx) {
			typ = TEXTURE_2D;
		}
		void TextureFromImage(std::string& path);
		U8* GetData();
	private:
		Image* img;
	};
}
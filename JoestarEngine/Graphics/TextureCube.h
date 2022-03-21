#pragma once
#include "Texture.h"
#include "../Container/Vector.h"

namespace Joestar {
	class TextureCube : public Texture {
		REGISTER_OBJECT(TextureCube, Texture)
	public:
		explicit TextureCube(EngineContext* ctx) : Texture(ctx) {
			typ = TEXTURE_CUBEMAP;
		}
		void TextureFromImage(String& path);
		virtual U32 GetSize() { return 6 * width * height * TEXTURE_FORMAT_SIZE[fmt]; }
		U8* GetData();
	private:
		Vector<Image*> imgs;
		U8* buffer;
	};
}
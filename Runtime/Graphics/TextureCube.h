#pragma once
#include "Texture.h"
#include "../Container/Vector.h"

namespace Joestar {
	class TextureCube : public Texture {
		REGISTER_OBJECT(TextureCube, Texture)
	public:
		explicit TextureCube(EngineContext* ctx);
		void SetImageDir(const String& dir, const Vector<String>& faces);
	};
}
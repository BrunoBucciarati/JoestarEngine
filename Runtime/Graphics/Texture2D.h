#pragma once
#include "Texture.h"

namespace Joestar {
	class Texture2D : public Texture {
		REGISTER_OBJECT(Texture2D, Texture)
	public:
		explicit Texture2D(EngineContext* ctx);
	};
}
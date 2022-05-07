#include "Texture2D.h"
#include "../Base/StringHash.h"

namespace Joestar {
	Texture2D::Texture2D(EngineContext* ctx): Super(ctx)
	{
		mType = ImageViewType::TYPE_2D;
		mFormat = ImageFormat::R8G8B8A8_SRGB;
		//mImageView = JOJO_NEW(GPUImageView(ImageViewType::TYPE_2D), MEMORY_GFX_STRUCT);
	}

	Texture2D::~Texture2D()
	{
	}
}
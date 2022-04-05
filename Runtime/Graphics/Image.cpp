#include "Image.h"
#include "../IO/FileSystem.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Joestar {
	Image::Image(EngineContext* ctx) : Super(ctx)
	{}
	void Image::Load(String path)
	{
		String resDir = GetSubsystem<FileSystem>()->GetResourceDir();
		mData = (U8*)stbi_load((resDir + path).CString(), (int*)&mWidth, (int*)&mHeight, (int*)&mChannels, STBI_rgb_alpha);
	}
	Image::~Image()
	{
		if (mData)
			stbi_image_free(mData);
	}
}
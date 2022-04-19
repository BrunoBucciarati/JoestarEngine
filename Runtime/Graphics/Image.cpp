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
		//Not Fit To GPU Format
		if (mChannels == 3)
		{
			//require的是4通道，stb会自动生成4通道的数据，这里把我们存的值改成4通道就行了。
			mChannels = 4;
		}
	}
	Image::~Image()
	{
		if (mData && !mImageFreed)
			stbi_image_free(mData);
	}
}
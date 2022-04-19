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
			U8* rgbData = mData;
			mData = JOJO_NEW_ARRAY(U8, 4 * mWidth * mHeight, MEMORY_TEXTURE);
			memset(mData, 0xFF, 4 * mWidth * mHeight);
			for (U32 i = 0; i < mHeight; ++i)
			{
				for (U32 j = 0; j < mWidth; ++j)
				{
					memcpy(mData + 4 * (i * mWidth + j), rgbData + 3 * (i * mWidth + j), 3);
				}
			}
			mChannels = 4;
			stbi_image_free(rgbData);
			mImageFreed = true;
		}
	}
	Image::~Image()
	{
		if (mData && !mImageFreed)
			stbi_image_free(mData);
	}
}
#include "Texture.h"
#include "Graphics.h"

namespace Joestar {
	Texture::Texture(EngineContext* ctx) : Super(ctx),
		mGraphics(GetSubsystem<Graphics>()),
		mSampler(JOJO_NEW(Sampler, MEMORY_TEXTURE)),
		mImageView(JOJO_NEW(GPUImageView(ctx), MEMORY_TEXTURE))
	{
		mGraphics->SetSampler(mSampler);
	}
	Texture::~Texture()
	{}

	void Texture::CreateSampler()
	{
	}

	void Texture::SetImage(Image* image, U32 layer)
	{
		if (mImages.Empty())
		{
			mImages.Resize(mLayers);
			for (U32 i = 0; i < mLayers; ++i)
			{
				mImages[i] = nullptr;
			}
		}
		mImages[layer] = image;
		if (mLayers == 1)
		{
			mImageView->SetImage(image);
			mGraphics->CreateTexture(this);
		}
		else
		{
			bool bReady = true;
			for (U32 i = 0; i < mLayers; ++i)
			{
				if (!mImages[i])
				{
					bReady = false;
					break;
				}
			}
			if (bReady)
			{
				mImageView->SetImages(mImages);
				mGraphics->CreateTexture(this);
			}
		}
	}


	void Texture::SetData(U8* data)
	{
		//if (mData)
		//	JOJO_DELETE_ARRAY(mData);
		//mData = JOJO_NEW_ARRAY(U8, mSize, MEMORY_TEXTURE);
		//memcpy(mData, data, mSize);

		//if (mImageView)
		//{
		//	mImageView->SetWidth(mWidth);
		//	mImageView->SetHeight(mHeight);
		//	mGraphics->CreateImageView(mImageView);
		//}
	}
}
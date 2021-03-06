#include "Texture.h"
#include "Graphics.h"

namespace Joestar {
	Texture::Texture(EngineContext* ctx) : Super(ctx),
		mGraphics(GetSubsystem<Graphics>()),
		mImageView(JOJO_NEW(GPUImageView(ctx), MEMORY_TEXTURE))
	{}
	Texture::~Texture()
	{}

	void Texture::CreateSampler()
	{
	}

	void Texture::SetRenderTarget()
	{
		mImageView->SetWriteOnly(bWriteOnly);
		mImageView->SetFormat(mFormat);
		mImageView->SetRenderTarget(mWidth, mHeight);
		mGraphics->CreateTexture(this);
	}

	void Texture::SetImage(Image* image, U32 layer)
	{
		mSampler = JOJO_NEW(Sampler, MEMORY_TEXTURE);
		mGraphics->SetSampler(mSampler);
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
		mSampler = JOJO_NEW(Sampler, MEMORY_TEXTURE);
		mGraphics->SetSampler(mSampler);
		if (mLayers == 1)
		{
			mImageView->GetImage()->SetWidth(mWidth);
			mImageView->GetImage()->SetHeight(mHeight);
			mImageView->SetFormat(mFormat);
			mImageView->SetData(data);
			mGraphics->CreateTexture(this);
		}
	}
}
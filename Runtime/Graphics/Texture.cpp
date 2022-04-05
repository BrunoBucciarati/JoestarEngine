#include "Texture.h"
#include "Graphics.h"

namespace Joestar {
	Texture::Texture(EngineContext* ctx) : Super(ctx),
		mGraphics(GetSubsystem<Graphics>())
	{}
	Texture::~Texture()
	{}

	void Texture::CreateSampler()
	{
		mSampler = JOJO_NEW(Sampler, MEMORY_TEXTURE);
		mGraphics->SetSampler(mSampler);
	}

	void Texture::SetImage(Image* image, U32 layer)
	{
		mImageView->SetImage(image);
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
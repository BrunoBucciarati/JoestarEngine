#include "GPUImage.h"
#include "Image.h"
#include "Graphics.h"

namespace Joestar {
	GPUImage::GPUImage(EngineContext* ctx) : Super(ctx),
		mGraphics(GetSubsystem<Graphics>())
	{}

	GPUImage::~GPUImage()
	{}

	void GPUImage::SetImage(Image* image)
	{
		mWidth = image->GetWidth();
		mHeight = image->GetHeight();
		mSize = image->GetSize();
		if (image->GetNumChannels() == 3)
			mFormat = ImageFormat::R8G8B8_SRGB;
		SetData(image->GetData());
	}

	void GPUImage::SetImages(Vector<SharedPtr<Image>>& images)
	{
		mWidth = images[0]->GetWidth();
		mHeight = images[0]->GetHeight();
		U32 sz = images[0]->GetSize();
		mSize = sz * images.Size();
		if (mData)
			JOJO_DELETE_ARRAY(mData);
		mData = JOJO_NEW_ARRAY(U8, mSize, MEMORY_TEXTURE);
		for (U32 i = 0; i < images.Size(); ++i)
			memcpy(mData + i * sz, images[i]->GetData(), sz);
		mGraphics->CreateImage(this);
	}

	void GPUImage::SetSubData(U8* data, U32 offset)
	{
	}

	void GPUImage::SetData(U8* data)
	{
		if (mData)
			JOJO_DELETE_ARRAY(mData);
		mData = JOJO_NEW_ARRAY(U8, mSize, MEMORY_TEXTURE);
		memcpy(mData, data, mSize);
		mGraphics->CreateImage(this);
	}

	GPUImageView::GPUImageView(EngineContext* ctx) : Super(ctx),
		mGraphics(GetSubsystem<Graphics>())
	{}
	GPUImageView::~GPUImageView()
	{}

	void GPUImageView::SetImage(Image* image)
	{
		if (!mImage)
			mImage = JOJO_NEW(GPUImage(mContext), MEMORY_TEXTURE);
		mImage->SetImage(image);
		mFormat = mImage->GetFormat();
		mGraphics->CreateImageView(this);
	}

	void GPUImageView::SetImages(Vector<SharedPtr<Image>>& images)
	{
		if (!mImage)
			mImage = JOJO_NEW(GPUImage(mContext), MEMORY_TEXTURE);
		mImage->SetImages(images);
		mFormat = mImage->GetFormat();
		mGraphics->CreateImageView(this);
	}
}
#include "GPUImage.h"
#include "Image.h"
#include "Graphics.h"

namespace Joestar {
	static bool IsDepthFormat(ImageFormat fmt)
	{
		return fmt == ImageFormat::D24S8 || fmt == ImageFormat::D32S8 || fmt == ImageFormat::D32;
	}

	const U32 ImageChannels[]
	{
		4,
		3,
		4,
		3,
		3,
		4,
		5,
		4,
		4,
		2
	};
	static U32 GetChannels(ImageFormat fmt)
	{
		return ImageChannels[(U32)fmt];
	}
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
		mLayer = images.Size();
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
		if (0 == mSize && data)
		{
			mSize = mWidth * mHeight * GetChannels(mFormat);
		}
		mData = JOJO_NEW_ARRAY(U8, mSize, MEMORY_TEXTURE);
		memcpy(mData, data, mSize);
		mGraphics->CreateImage(this);
	}


	void GPUImage::SetRenderTarget(U32 w, U32 h)
	{
		if (IsDepthFormat(mFormat))
			mUsage = (U32)ImageUsageBits::DEPTH_STENCIL_ATTACHMENT_BIT;
		else
			mUsage = (U32)ImageUsageBits::COLOR_ATTACHMENT_BIT;
		if (!bWriteOnly)
		{
			mUsage |= (U32)ImageUsageBits::SAMPLED_BIT;
		}
		mWidth = w;
		mHeight = h;
		mGraphics->CreateImage(this);
	}

	GPUImageView::GPUImageView(EngineContext* ctx) : Super(ctx),
		mGraphics(GetSubsystem<Graphics>())
	{}
	GPUImageView::~GPUImageView()
	{}

	GPUImage* GPUImageView::GetImage()
	{
		if (!mImage)
			mImage = JOJO_NEW(GPUImage(mContext), MEMORY_TEXTURE);
		return mImage;
	}

	void GPUImageView::SetImage(Image* image)
	{
		GetImage();
		mImage->SetImage(image);
		mFormat = mImage->GetFormat();
		mGraphics->CreateImageView(this);
	}

	void GPUImageView::SetRenderTarget(U32 w, U32 h)
	{
		if (!mImage)
			mImage = JOJO_NEW(GPUImage(mContext), MEMORY_TEXTURE);
		mImage->SetFormat(mFormat);
		mImage->SetWriteOnly(bWriteOnly);
		if (IsDepthFormat(mFormat))
		{
			SetAspectBits((U32)ImageAspectFlagBits::DEPTH_BIT | (U32)ImageAspectFlagBits::STENCIL_BIT);
		}
		else
		{
			SetAspectBits((U32)ImageAspectFlagBits::COLOR_BIT);
		}
		mImage->SetRenderTarget(w, h);
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

	void GPUImageView::SetData(U8* data)
	{
		if (!mImage)
			mImage = JOJO_NEW(GPUImage(mContext), MEMORY_TEXTURE);
		mImage->SetData(data);
		mGraphics->CreateImageView(this);
	}

	void GPUImageView::SetFormat(ImageFormat fmt)
	{
		mFormat = fmt;
		GetImage()->SetFormat(fmt);
	}

}
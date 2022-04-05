#include "GPUTexture.h"
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
		SetData(image->GetData());
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
		//如果2D图片，凑够一张就可以提交了
		if (mType == ImageViewType::TYPE_2D)
		{
			mGraphics->CreateImageView(this);
		}
	}
}
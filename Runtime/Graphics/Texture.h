#pragma once
#include "Image.h"
#include "../Core/Minimal.h"
#include "GraphicDefines.h"
#include "Sampler.h"
#include "GPUImage.h"
namespace Joestar {
	class Graphics;
	class Texture : public Object
	{
		REGISTER_OBJECT(Texture, Object);
		GET_SET_STATEMENT_INITVALUE(GPUResourceHandle, Handle, GPUResource::INVALID_HANDLE);
		GET_SET_STATEMENT_INITVALUE(U32, Size, 0);
		GET_SET_STATEMENT_INITVALUE(U32, Width, 0);
		GET_SET_STATEMENT_INITVALUE(U32, Height, 0);
		GET_SET_STATEMENT(ImageFormat, Format, 0);
		GET_SET_STATEMENT(ImageViewType, Type, 0);
		GET_SET_STATEMENT(UniformFrequency, Frequency, UniformFrequency::BATCH);
		
	public:
		explicit Texture(EngineContext* ctx);
		virtual U32 GetSize() { return mWidth * mHeight; }
		void SetData(U8* data);
		void SetImage(Image* image, U32 layer = 0);
		void SetRenderTarget();
		void CreateSampler();
		GPUResourceHandle GetImageViewHandle()
		{
			return mImageView->GetHandle();
		}
		GPUImageView* GetImageView()
		{
			return mImageView;
		}
		Sampler* GetSampler()
		{
			return mSampler;
		}
		void SetWriteOnly(bool f)
		{
			bWriteOnly = f;
		}
	protected:
		SharedPtr<GPUImageView> mImageView;
		SharedPtr<Sampler> mSampler;
		U32 numMipMaps{ 0 };
		U32 mLayers{ 1 };
		WeakPtr<Graphics> mGraphics;
		Vector<SharedPtr<Image>> mImages;
		bool bWriteOnly{false};
	};
}
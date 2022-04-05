#pragma once
#include "Image.h"
#include "../Core/Minimal.h"
#include "GraphicDefines.h"
#include "Sampler.h"
#include "GPUTexture.h"
namespace Joestar {
	class Graphics;
	class Texture : public Object
	{
		REGISTER_OBJECT(Texture, Object);
		GET_SET_STATEMENT_INITVALUE(GPUResourceHandle, Handle, GPUResource::INVALID_HANDLE);
		GET_SET_STATEMENT_INITVALUE(U32, Size, 0);
		GET_SET_STATEMENT(ImageFormat, Format, 0);
		GET_SET_STATEMENT(ImageType, Type, 0);
		GET_SET_STATEMENT(UniformFrequency, Frequency, UniformFrequency::BATCH);
		
	public:
		explicit Texture(EngineContext* ctx);
		virtual U32 GetSize() { return mWidth * mHeight; }
		virtual U32 GetWidth() { return mWidth; }
		virtual U32 GetHeight() { return mHeight; }
		virtual U8* GetData() { return mData; }
		void SetData(U8* data);
		void SetImage(Image* image, U32 layer = 0);
		void CreateSampler();
	protected:
		U32 mWidth{ 0 };
		U32 mHeight{ 0 };
		SharedPtr<GPUImageView> mImageView;
		SharedPtr<Sampler> mSampler;
		U32 numMipMaps{ 0 };
		U8* mData{ nullptr };
		WeakPtr<Graphics> mGraphics;
	};
}
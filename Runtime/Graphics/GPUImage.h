#pragma once
#include "GPUResource.h"
#include "../Container/Ptr.h"
#include "../Container/Vector.h"
#include "../Core/Object.h"

namespace Joestar {
	enum class ImageType
	{
		TYPE_1D = 0,
		TYPE_2D = 1,
		TYPE_3D = 2,
		TypeCount
	};

	enum class ImageViewType {
		TYPE_1D = 0,
		TYPE_2D = 1,
		TYPE_3D = 2,
		TYPE_CUBE = 3,
		TYPE_1D_ARRAY = 4,
		TYPE_2D_ARRAY = 5,
		TYPE_CUBE_ARRAY = 6,
	};

	//How To Use Image, You may also check Vulkan's VkImageUsageFlagBits
	enum class ImageUsageBits {
		TRANSFER_SRC_BIT = 0x00000001,
		TRANSFER_DST_BIT = 0x00000002,
		SAMPLED_BIT = 0x00000004,
		STORAGE_BIT = 0x00000008,
		COLOR_ATTACHMENT_BIT = 0x00000010,
		DEPTH_STENCIL_ATTACHMENT_BIT = 0x00000020,
		TRANSIENT_ATTACHMENT_BIT = 0x00000040,
		INPUT_ATTACHMENT_BIT = 0x00000080,
	};


	enum class ImageAspectFlagBits {
		COLOR_BIT = 0x00000001,
		DEPTH_BIT = 0x00000002,
		STENCIL_BIT = 0x00000004,
		METADATA_BIT = 0x00000008,
	};

	enum class ImageFormat
	{
		R8G8B8A8_SRGB = 0,
		R8G8B8_SRGB,
		B8G8R8A8_SRGB,
		B8G8R8_SRGB,
		RG11B10,
		D24S8,
		D32S8,
		D32,
		R32,
		R16,
		FormatCount
	};

	static bool IsDepthFormat(ImageFormat);
	static U32 GetChannels(ImageFormat);
	class Image;
	class Graphics;
	class GPUImage : public Object
	{
		REGISTER_OBJECT_ROOT(GPUImage);
		GET_SET_STATEMENT_INITVALUE(GPUResourceHandle, Handle, GPUResource::INVALID_HANDLE);
		GET_SET_STATEMENT_INITVALUE(ImageType, Type, ImageType::TYPE_2D);
		GET_SET_STATEMENT_INITVALUE(ImageFormat, Format, ImageFormat::R8G8B8A8_SRGB);
		GET_SET_STATEMENT_INITVALUE(U32, Width, 0);
		GET_SET_STATEMENT_INITVALUE(U32, Height, 0);
		GET_SET_STATEMENT_INITVALUE(U32, Depth, 1);
		GET_SET_STATEMENT_INITVALUE(U32, Layer, 1);
		GET_SET_STATEMENT_INITVALUE(U32, Usage, (U32)ImageUsageBits::TRANSFER_SRC_BIT | (U32)ImageUsageBits::TRANSFER_DST_BIT | (U32)ImageUsageBits::SAMPLED_BIT);
		GET_SET_STATEMENT_INITVALUE(U32, Samples, 1);
		GET_SET_STATEMENT_INITVALUE(U32, MipLevels, 1);
		GET_SET_STATEMENT_INITVALUE(U32, Size, 0);
		GET_SET_STATEMENT_PREFIX_INITVALUE(bool, WriteOnly, b, false);
	public:
		explicit GPUImage(EngineContext*);
		void SetImage(Image* image);
		void SetImages(Vector<SharedPtr<Image>>&);
		void SetData(U8* data);
		void SetSubData(U8* data, U32 offset=0);
		void SetRenderTarget(U32 w, U32 h);
		void SetSize(U32 w, U32 h, U32 d = 0);
		U8* GetData()
		{
			return mData;
		}
	private:
		U8* mData{ nullptr };
		WeakPtr<Graphics> mGraphics;
	};

	class GPUImageView : public Object
	{
		REGISTER_OBJECT_ROOT(GPUImageView);
		GET_SET_STATEMENT(SharedPtr<GPUImage>, Image);
		GET_SET_STATEMENT_INITVALUE(GPUResourceHandle, Handle, GPUResource::INVALID_HANDLE);
		GET_SET_STATEMENT_INITVALUE(ImageViewType, Type, ImageViewType::TYPE_2D);
		GET_SET_STATEMENT_INITVALUE(U32, AspectBits, (U32)ImageAspectFlagBits::COLOR_BIT);
		GET_SET_STATEMENT_INITVALUE(U32, BaseMipLevel, 0);
		GET_SET_STATEMENT_INITVALUE(U32, MipLevels, 1);
		GET_SET_STATEMENT_INITVALUE(U32, BaseLayer, 0);
		GET_SET_STATEMENT_INITVALUE(U32, Layer, 1);
		GET_SET_STATEMENT_PREFIX_INITVALUE(bool, WriteOnly, b, false);
	public:
		explicit GPUImageView(EngineContext*);
		void SetImage(Image*);
		void SetRenderTarget(U32 w, U32 h);
		void SetImages(Vector<SharedPtr<Image>>&);
		void SetData(U8* data);
		void SetFormat(ImageFormat fmt);
		ImageFormat GetFormat()
		{
			return mFormat;
		}
		GPUImage* GetImage();
	private:
		WeakPtr<Graphics> mGraphics;
		ImageFormat mFormat{ ImageFormat::R8G8B8A8_SRGB };
	};

	class GPUSampler : public GPUResource
	{};

	class GPUTexture : public GPUResource
	{
	public:
		GPUImage mImage;
		GPUSampler mSampler;
	};
}
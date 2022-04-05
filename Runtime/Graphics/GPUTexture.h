#pragma once
#include "GPUResource.h"
#include "../Container/Ptr.h"
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
		B8G8R8A8_SRGB = 1,
		RG11B10 = 2,
		D24S8 = 3,
		D32S8 = 4,
		FormatCount
	};
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
		GET_SET_STATEMENT_INITVALUE(U32, Layer, 0);
		GET_SET_STATEMENT_INITVALUE(U32, Usage, (U32)ImageUsageBits::SAMPLED_BIT);
		GET_SET_STATEMENT_INITVALUE(U32, Samples, 0);
		GET_SET_STATEMENT_INITVALUE(U32, MipLevels, 0);
		GET_SET_STATEMENT_INITVALUE(U32, Size, 0);
	public:
		explicit GPUImage(EngineContext*);
		void SetImage(Image* image);
		void SetData(U8* data);
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
		GET_SET_STATEMENT_INITVALUE(ImageFormat, Format, ImageFormat::R8G8B8A8_SRGB);
		GET_SET_STATEMENT_INITVALUE(U32, AspectBits, (U32)ImageAspectFlagBits::COLOR_BIT);
		GET_SET_STATEMENT_INITVALUE(U32, BaseMipLevel, 0);
		GET_SET_STATEMENT_INITVALUE(U32, MipLevels, 0);
		GET_SET_STATEMENT_INITVALUE(U32, BaseLayer, 0);
		GET_SET_STATEMENT_INITVALUE(U32, Layer, 0);
	public:
		explicit GPUImageView(EngineContext*);
		void SetImage(Image*);
	private:
		U32 mBaseMipLevel{ 0 };
		U32 mBaseLayer{ 0 };
		WeakPtr<Graphics> mGraphics;
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
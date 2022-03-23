#pragma once
#include "GPUResource.h"

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
		TYPE_MAX_ENUM = 0x7FFFFFFF
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
		VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
	};

	enum class ImageFormat
	{
		R8G8B8A8_SRGB = 0,
		B8G8R8A8_SRGB = 1,
		RG11B10 = 2,
		FormatCount
	};
	class GPUImage : public GPUResource
	{
	public:
		GPUImage(ImageType typ) : mType(typ) {}
		ImageType GetType() { return mType; }

		ImageFormat GetFormat() { return mFormat; }
		void SetFormat(ImageFormat f) { mFormat = f; }

		void SetWidth(U32 width) { mWidth = width; }
		U32 GetWidth() { return mWidth; }

		void SetHeight(U32 height) { mHeight = height; }
		U32 GetHeight() { return mHeight; }

		void SetDepth(U32 depth) { mDepth = depth; }
		U32 GetDepth() { return mDepth; }

		void SetLayer(U32 layer) { mLayer = layer; }
		U32 GetLayer() { return mLayer; }

		void SetUsage(U32 usage) { mUsage = usage; }
		U32 GetUsage() { return mUsage; }

		void SetSamples(U32 value) { mSamples = value; }
		U32 GetSamples() { return mSamples; }

		void SetMipLevels(U32 value) { mMipLevels = value; }
		U32 GetMipLevels() { return mMipLevels; }
	private:
		ImageFormat mFormat;
		ImageType mType;
		U32 mWidth{ 0 };
		U32 mHeight{ 0 };
		U32 mDepth{ 1 };
		U32 mLayer{ 1 };
		U32 mUsage{ (U32)ImageUsageBits::SAMPLED_BIT };
		U32 mSamples{ 0 };
		U32 mMipLevels{ 0 };
	};

	class GPUImageView : public GPUResource
	{
	public:
		GPUImageView(ImageViewType typ) : mType(typ) {}
		ImageViewType GetType() { return mType; }
		GPUImage* image;
		void SetBaseMipLevel(U32 value) { mBaseMipLevel = value; }
		U32 GetBaseMipLevel() { return mBaseMipLevel; }
		void SetAspectBits(U32 value) { mAspectBits = value; }
		U32 GetAspectBits() { return mAspectBits; }
		ImageFormat GetFormat()
		{
			return image->GetFormat();
		}
		U32 GetMipLevels()
		{
			return image->GetMipLevels();
		}
		U32 GetBaseLayer()
		{
			return mBaseLayer;
		}
		U32 GetLayer()
		{
			return image->GetLayer();
		}
	private:
		ImageViewType mType;
		U32 mAspectBits{ (U32)ImageAspectFlagBits::COLOR_BIT };
		U32 mBaseMipLevel{ 0 };
		U32 mBaseLayer{ 0 };
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
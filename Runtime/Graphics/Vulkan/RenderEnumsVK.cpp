#include "RenderEnumsVK.h"
namespace Joestar {
	const VkImageType ImageTypeMapping[]{
			VK_IMAGE_TYPE_1D,
			VK_IMAGE_TYPE_2D,
			VK_IMAGE_TYPE_3D,
			VK_IMAGE_TYPE_MAX_ENUM
	};
	VkImageType GetImageTypeVK(ImageType imgType)
	{
		return ImageTypeMapping[(U32)imgType];
	}

	const VkImageViewType ImageViewTypeMapping[]{
		VK_IMAGE_VIEW_TYPE_1D,
		VK_IMAGE_VIEW_TYPE_2D,
		VK_IMAGE_VIEW_TYPE_3D,
		VK_IMAGE_VIEW_TYPE_CUBE,
		VK_IMAGE_VIEW_TYPE_1D_ARRAY,
		VK_IMAGE_VIEW_TYPE_2D_ARRAY,
		VK_IMAGE_VIEW_TYPE_CUBE_ARRAY,
		VK_IMAGE_VIEW_TYPE_MAX_ENUM
	};
	VkImageViewType GetImageViewTypeVK(ImageViewType imgType)
	{
		return ImageViewTypeMapping[(U32)imgType];
	}

	VkFormat ImageFormatMapping[]{
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_FORMAT_B8G8R8A8_SRGB,
		VK_FORMAT_B10G11R11_UFLOAT_PACK32,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D32_SFLOAT_S8_UINT
	};
	VkFormat GetImageFormatVK(ImageFormat fmt)
	{
		return ImageFormatMapping[(U32)fmt];
	}

	const VkImageUsageFlagBits ImageFlagBitsMapping[]{
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_IMAGE_USAGE_STORAGE_BIT,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
		VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT
	};
	const U32 ImageFlagBitsCount = sizeof(ImageFlagBitsMapping) / sizeof(VkImageUsageFlagBits);
	U32 GetImageUsageBitsVK(U32 bits)
	{
		U32 flagBits = 0;
		for (U32 i = 0; i < ImageFlagBitsCount; ++i)
		{
			if (bits & 1 << i)
			{
				flagBits |= ImageFlagBitsMapping[i];
			}
		}
		return flagBits;
	}


	const VkImageAspectFlagBits ImageAspectBitsMapping[]{
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_ASPECT_DEPTH_BIT,
		VK_IMAGE_ASPECT_STENCIL_BIT,
		VK_IMAGE_ASPECT_METADATA_BIT
	};
	const U32 ImageAspectBitsCount = sizeof(ImageAspectBitsMapping) / sizeof(VkImageAspectFlagBits);
	U32 GetImageAspectBitsVK(U32 bits)
	{
		U32 flagBits = 0;
		for (U32 i = 0; i < ImageAspectBitsCount; ++i)
		{
			if (bits & 1 << i)
			{
				flagBits |= ImageAspectBitsMapping[i];
			}
		}
		return flagBits;
	}


	VkFormat GetInputFormatVK(VertexType vt)
	{
		if (vt == VertexType::VEC4)
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		if (vt == VertexType::VEC3)
			return VK_FORMAT_R32G32B32_SFLOAT;
		if (vt == VertexType::VEC2)
			return VK_FORMAT_R32G32_SFLOAT;
		if (vt == VertexType::FLOAT)
			return VK_FORMAT_R32_SFLOAT;
		if (vt == VertexType::UINT32)
			return VK_FORMAT_R32_UINT;
	}

	VkShaderStageFlagBits GetShaderStageFlagBits(ShaderStage stage)
	{
		if (ShaderStage::VS == stage)
			return VK_SHADER_STAGE_VERTEX_BIT;
		if (ShaderStage::PS == stage)
			return VK_SHADER_STAGE_FRAGMENT_BIT;
		if (ShaderStage::CS == stage)
			return VK_SHADER_STAGE_COMPUTE_BIT;
		if (ShaderStage::GS == stage)
			return VK_SHADER_STAGE_GEOMETRY_BIT;
	}

}
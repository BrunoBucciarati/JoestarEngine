#pragma once
#include <vulkan/vulkan.h>
#include "../GPUCreateInfos.h"

namespace Joestar
{
	VkImageType GetImageTypeVK(ImageType imgType);
	
	VkImageViewType GetImageViewTypeVK(ImageViewType imgType);

	VkFormat GetImageFormatVK(ImageFormat fmt);

	U32 GetImageUsageBitsVK(U32 bits);

	U32 GetImageAspectBitsVK(U32 bits);

	VkFormat GetInputFormatVK(VertexType vt);

	VkShaderStageFlagBits GetShaderStageFlagBits(ShaderStage stage);
}
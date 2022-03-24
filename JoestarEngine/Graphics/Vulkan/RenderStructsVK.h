#pragma once
#include <vulkan/vulkan.h>
#include "../../IO/Log.h"
namespace Joestar {
	class CommandBufferVK
	{
	public:
		void Create(VkDevice& device, VkCommandBufferAllocateInfo& allocInfo, U32 num = 1)
		{
			if (bCreated)
				return;
			commandBuffers.Resize(num);
			for (U32 i = 0; i < num; ++i)
			{
				if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffers[i]) != VK_SUCCESS)
				{
					LOGERROR("failed to allocate command buffer!\n");
					return;
				}
			}
			bCreated = true;
		}

		void Begin(U32 idx = 0)
		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = 0; // Optional
			beginInfo.pInheritanceInfo = nullptr; // Optional

			if (vkBeginCommandBuffer(commandBuffers[idx], &beginInfo) != VK_SUCCESS) {
				LOGERROR("failed to begin recording command buffer!");
			}
		}

		void End(U32 idx = 0)
		{
			if (vkEndCommandBuffer(commandBuffers[idx]) != VK_SUCCESS) {
				LOGERROR("failed to record command buffer!");
			}
		}

	private:
		Vector<VkCommandBuffer> commandBuffers;
		bool bCreated{ false };
	};

	class ImageVK
	{
	public:
		void Create(VkDevice& device, VkImageCreateInfo& imageInfo, U32 num = 1)
		{
			images.Resize(num);
			for (int i = 0; i < num; ++i)
			{
				if (vkCreateImage(device, &imageInfo, nullptr, &images[i]) != VK_SUCCESS) {
					LOGERROR("failed to create image!");
				}
			}
			vkGetImageMemoryRequirements(device, images[0], &memRequirements);
		}

		void AllocMemory(VkDevice& device, U32 memoryTypeIdx)
		{
			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = memoryTypeIdx;
			imageMemorys.Resize(images.Size());
			for (int i = 0; i < images.Size(); ++i)
			{
				if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemorys[i]) != VK_SUCCESS) {
					LOGERROR("failed to allocate image memory!");
				}
				vkBindImageMemory(device, images[i], imageMemorys[i], 0);
			}
		}

		VkImage& GetImage(U32 idx = 0)
		{
			return images[idx];
		}
		VkMemoryRequirements memRequirements;

		void SetRawImages(Vector<VkImage>& imgs)
		{
			images = imgs;
		}

	private:
		Vector<VkImage> images;
		Vector<VkDeviceMemory> imageMemorys;
	};

	class ImageViewVK
	{
	public:
		ImageVK* image;
		void Create(VkDevice& device, VkImageViewCreateInfo& viewInfo, U32 num = 1)
		{
			imageViews.Resize(num);
			for (U32 i = 0; i < num; ++i)
			{
				if (vkCreateImageView(device, &viewInfo, nullptr, &imageViews[i]) != VK_SUCCESS) {
					LOGERROR("failed to create texture image view!");
				}
			}
		}
		Vector<VkImageView> imageViews;

		void SetRawImageViews(Vector<VkImageView>& imgs)
		{
			imageViews = imgs;
		}
	};

	class RenderPassVK
	{
	public:
		VkRenderPass renderPass;
	};

	class FrameBufferVK
	{
	public:
		Vector<ImageViewVK*> colorAttachments;
		ImageViewVK* depthStencilAttachment;
		Vector<VkFramebuffer> frameBuffers;
		U32 msaaSamples{ 1 };
		void SetRawImages(Vector<VkImage>& images, U32 idx = 0)
		{
			colorAttachments[idx]->image->SetRawImages(images);
		}
		void SetRawImageViews(Vector<VkImageView>& imageViews, U32 idx = 0)
		{
			colorAttachments[idx]->SetRawImageViews(imageViews);
		}
		RenderPassVK* renderPass;
	};

	class SwapChainVK
	{
	public:
		VkFormat format;
		VkSwapchainKHR swapChain;
		VkExtent2D extent;
		Vector<VkImage> images;
		Vector<VkImageView> imageViews;
		FrameBufferVK* frameBuffer{nullptr};
		U32 GetImageCount()
		{
			return images.Size();
		}
	};
}
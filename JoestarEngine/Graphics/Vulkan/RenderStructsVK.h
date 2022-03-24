#pragma once
#include <vulkan/vulkan.h>
#include "../../IO/Log.h"
namespace Joestar {
	class CommandBufferVK
	{
	public:
		void Create(VkDevice& inDevice, VkCommandBufferAllocateInfo& allocInfo, U32 num = 1)
		{
			if (bCreated)
				return;
            device = inDevice;
            pool = allocInfo.commandPool;
			commandBuffers.Resize(num);
			for (U32 i = 0; i < num; ++i)
			{
				if (vkAllocateCommandBuffers(inDevice, &allocInfo, &commandBuffers[i]) != VK_SUCCESS)
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

        void Submit(VkPipelineStageFlags waitMask = 0, VkFence fence = VK_NULL_HANDLE) {
            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pWaitDstStageMask = &waitMask;
            submitInfo.pCommandBuffers = &GetCommandBuffer();

            vkQueueSubmit(queue, 1, &submitInfo, fence);
            vkQueueWaitIdle(queue);
            vkFreeCommandBuffers(device, pool, 1, &GetCommandBuffer());
        }

        VkCommandBuffer& GetCommandBuffer(U32 idx = 0)
        {
            return commandBuffers[idx];
        }

        void SetQueue(VkQueue& q)
        {
            queue = q;
        }

        void SetIndex(U32 idx)
        {
            index = idx;
        }

	private:
		Vector<VkCommandBuffer> commandBuffers;
		bool bCreated{ false };
        VkDevice device;
        VkCommandPool pool;
        VkQueue queue;
        U32 index{ 0 };
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
        void TransitionImageLayout(CommandBufferVK& cb, VkImageLayout oldLayout, VkImageLayout newLayout, U32 aspect = VK_IMAGE_ASPECT_COLOR_BIT) {
            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.oldLayout = oldLayout;
            barrier.newLayout = newLayout;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = image->GetImage();
            barrier.subresourceRange.aspectMask = aspect;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;
            VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
            VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
            // Source layouts (old)
            // Source access mask controls actions that have to be finished on the old layout
            // before it will be transitioned to the new layout
            switch (oldLayout)
            {
            case VK_IMAGE_LAYOUT_UNDEFINED:
                // Image layout is undefined (or does not matter)
                // Only valid as initial layout
                // No flags required, listed only for completeness
                barrier.srcAccessMask = 0;
                break;

            case VK_IMAGE_LAYOUT_PREINITIALIZED:
                // Image is preinitialized
                // Only valid as initial layout for linear images, preserves memory contents
                // Make sure host writes have been finished
                barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
                // Image is a color attachment
                // Make sure any writes to the color buffer have been finished
                barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                // Image is a depth/stencil attachment
                // Make sure any writes to the depth/stencil buffer have been finished
                barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
                // Image is a transfer source
                // Make sure any reads from the image have been finished
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                break;

            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                // Image is a transfer destination
                // Make sure any writes to the image have been finished
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
                // Image is read by a shader
                // Make sure any shader reads from the image have been finished
                barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                break;
            default:
                // Other source layouts aren't handled (yet)
                break;
            }

            // Target layouts (new)
            // Destination access mask controls the dependency for the new image layout
            switch (newLayout)
            {
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                // Image will be used as a transfer destination
                // Make sure any writes to the image have been finished
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
                // Image will be used as a transfer source
                // Make sure any reads from the image have been finished
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                break;

            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
                // Image will be used as a color attachment
                // Make sure any writes to the color buffer have been finished
                barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                // Image layout will be used as a depth/stencil attachment
                // Make sure any writes to depth/stencil buffer have been finished
                barrier.dstAccessMask = barrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
                // Image will be read in a shader (sampler, input attachment)
                // Make sure any writes to the image have been finished
                if (barrier.srcAccessMask == 0)
                {
                    barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
                }
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                break;
            default:
                // Other source layouts aren't handled (yet)
                break;
            }

            cb.Begin();

            vkCmdPipelineBarrier(
                cb.GetCommandBuffer(),
                srcStageMask, dstStageMask,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier
            );
            cb.End();
            cb.Submit();
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
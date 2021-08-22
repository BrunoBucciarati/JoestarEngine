#include "GPUProgramVulkan.h"
#include "../IO/Log.h"
#include "../Misc/Application.h"
#include <array>
#include <direct.h>
#include "Image.h"

namespace Joestar {
	TextureVK::TextureVK(Texture* t) : texture(t) {
		
	}

	VkFormat GPUProgramVulkan::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(vkCtxPtr->physicalDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}

		LOGERROR("failed to find supported format!");
	}
	VkFormat GPUProgramVulkan::FindDepthFormat() {
		return FindSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}
	VkShaderModule GPUProgramVulkan::CreateShaderModule(File* file) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		size_t codeSize = file->Size();
		createInfo.codeSize = codeSize;
		createInfo.pCode = reinterpret_cast<const uint32_t*>(file->GetBuffer());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(vkCtxPtr->device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			LOGERROR("failed to create shader module!");
		}
		return shaderModule;
	}

	GPUProgramVulkan::GPUProgramVulkan() {
		//Application* app = Application::GetApplication();
		//FileSystem* fs = app->GetSubSystem<FileSystem>();
		//std::string path = fs->GetResourceDir();
		//path += "Models/";
		//char workDir[260];
		//if (_getcwd(workDir, 260))
		//	path = workDir + ("/" + path);
		//mesh = new Mesh;
		//mesh->Load(path + "viking_room/viking_room.obj");
	}
	
	void GPUProgramVulkan::SetShader(PipelineState& pso, std::string& vertexPath, std::string& fragmentPath, std::string& geometryPath) {
		Application* app = Application::GetApplication();
		FileSystem* fs = app->GetSubSystem<FileSystem>();
		std::string path = fs->GetResourceDir();
		path += "Shaders/";
		char workDir[260];
		if (_getcwd(workDir, 260))
			path = workDir + ("/" + path);

		//First Compile To Spir-V
		std::string vertSpvPath = std::string(vertexPath) + ".spv";
		std::string fragSpvPath = std::string(fragmentPath) + ".spv";
		std::string compileVertSpv = path + "glslc.exe " + (path + vertexPath) + " -o " + (path + vertSpvPath);
		std::string compileFragSpv = path + "glslc.exe " + (path + fragmentPath) + " -o " + (path + fragSpvPath);
		system(compileVertSpv.c_str());
		system(compileFragSpv.c_str());
		File* vShaderCode = ShaderCodeFile(vertSpvPath.c_str());
		File* fShaderCode = ShaderCodeFile(fragSpvPath.c_str());

		pso.vertShaderModule = CreateShaderModule(vShaderCode);
		pso.fragShaderModule = CreateShaderModule(fShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = pso.vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = pso.fragShaderModule;
		fragShaderStageInfo.pName = "main";

		//VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
		pso.shaderStage[0] = vertShaderStageInfo;
		pso.shaderStage[1] = fragShaderStageInfo;
	}

	void GPUProgramVulkan::Clean() {
		vkDestroyDescriptorSetLayout(vkCtxPtr->device, currentPSO.pipelineCtx->descriptorSetLayout, nullptr);
		vkDestroyShaderModule(vkCtxPtr->device, currentPSO.vertShaderModule, nullptr);
		vkDestroyShaderModule(vkCtxPtr->device, currentPSO.fragShaderModule, nullptr);
		vkDestroyBuffer(vkCtxPtr->device, currentPSO.vertexBuffer, nullptr);
		vkFreeMemory(vkCtxPtr->device, currentPSO.vertexBufferMemory, nullptr);
		vkDestroyBuffer(vkCtxPtr->device, currentPSO.indexBuffer, nullptr);
		vkFreeMemory(vkCtxPtr->device, currentPSO.indexBufferMemory, nullptr);
		vkDestroySampler(vkCtxPtr->device, currentPSO.textureSampler, nullptr);
		//vkDestroyImageView(vkCtxPtr->device, currentPSO.textureImageView, nullptr);
		//vkDestroyImage(vkCtxPtr->device, currentPSO.textureImage, nullptr);
		//vkFreeMemory(vkCtxPtr->device, currentPSO.textureImageMemory, nullptr);
		//delete mesh;

		vkDestroyPipeline(vkCtxPtr->device, currentPSO.pipelineCtx->graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(vkCtxPtr->device, currentPSO.pipelineCtx->pipelineLayout, nullptr);
		vkDestroyRenderPass(vkCtxPtr->device, currentPSO.pipelineCtx->renderPass, nullptr);
	}

	//VkPipelineVertexInputStateCreateInfo* GPUProgramVulkan::GetVertexInputInfo() {
	//	//return mesh->GetVB()->GetVKVertexInputInfo();
	//}

	void GPUProgramVulkan::CreateVertexBuffer(PipelineState& pso, VertexBuffer* vb) {
		VkDeviceSize bufferSize = vb->GetSize();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
		void* data;
		vkMapMemory(vkCtxPtr->device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vb->GetBuffer(), (size_t)bufferSize);
		vkUnmapMemory(vkCtxPtr->device, stagingBufferMemory);

		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, pso.vertexBuffer, pso.vertexBufferMemory);
		CopyBuffer(stagingBuffer, pso.vertexBuffer, bufferSize);

		vkDestroyBuffer(vkCtxPtr->device, stagingBuffer, nullptr);
		vkFreeMemory(vkCtxPtr->device, stagingBufferMemory, nullptr);
	}

	void GPUProgramVulkan::CreateIndexBuffer(PipelineState& pso, IndexBuffer* ib) {
		VkDeviceSize bufferSize = ib->GetSize();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(vkCtxPtr->device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, ib->GetBuffer(), (size_t)bufferSize);
		vkUnmapMemory(vkCtxPtr->device, stagingBufferMemory);

		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, pso.indexBuffer, pso.indexBufferMemory);

		CopyBuffer(stagingBuffer, pso.indexBuffer, bufferSize);

		vkDestroyBuffer(vkCtxPtr->device, stagingBuffer, nullptr);
		vkFreeMemory(vkCtxPtr->device, stagingBufferMemory, nullptr);
	}

	uint32_t GPUProgramVulkan::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(vkCtxPtr->physicalDevice, &memProperties);
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}


		LOGERROR("failed to find suitable memory type!");
	}

	void GPUProgramVulkan::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(vkCtxPtr->device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(vkCtxPtr->device, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(vkCtxPtr->device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate buffer memory!");
		}

		vkBindBufferMemory(vkCtxPtr->device, buffer, bufferMemory, 0);
	}

	void GPUProgramVulkan::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		EndSingleTimeCommands(commandBuffer);
	}

	void GPUProgramVulkan::CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = static_cast<uint32_t>(width);
		imageInfo.extent.height = static_cast<uint32_t>(height);
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = mipLevels;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = numSamples;;
		imageInfo.flags = 0; // Optional  
		if (vkCreateImage(vkCtxPtr->device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
			LOGERROR("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(vkCtxPtr->device, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if (vkAllocateMemory(vkCtxPtr->device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(vkCtxPtr->device, image, imageMemory, 0);
	}

	void GPUProgramVulkan::CreateTextureImage(TextureVK* tex) {
		Image* img = new Image("Models/viking_room/viking_room.png");
		VkDeviceSize imageSize = tex->GetSize();
		mipLevels = static_cast<uint32_t>(std::floor(std::log2(Max(img->GetWidth(), img->GetHeight())))) + 1;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
		void* data;
		vkMapMemory(vkCtxPtr->device, stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, img->GetData(), static_cast<size_t>(imageSize));
		vkUnmapMemory(vkCtxPtr->device, stagingBufferMemory);

		CreateImage(img->GetWidth(), img->GetHeight(), mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, tex->textureImage, tex->textureImageMemory);
		//for mipmap
		TransitionImageLayout(tex->textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
		CopyBufferToImage(stagingBuffer, tex->textureImage, static_cast<uint32_t>(img->GetWidth()), static_cast<uint32_t>(img->GetHeight()));
		if (tex->HasMipmap()) {
			GenerateMipmaps(tex->textureImage, VK_FORMAT_R8G8B8A8_SRGB, img->GetWidth(), img->GetHeight(), mipLevels);
		} else {
			TransitionImageLayout(tex->textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels);
		}

		delete img;
		vkDestroyBuffer(vkCtxPtr->device, stagingBuffer, nullptr);
		vkFreeMemory(vkCtxPtr->device, stagingBufferMemory, nullptr);
	}

	void GPUProgramVulkan::GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels) {
		// Check if image format supports linear blitting
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(vkCtxPtr->physicalDevice, imageFormat, &formatProperties);

		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
			LOGERROR("texture image format does not support linear blitting!");
		}

		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		int32_t mipWidth = texWidth;
		int32_t mipHeight = texHeight;

		for (uint32_t i = 1; i < mipLevels; i++) {
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			VkImageBlit blit{};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(commandBuffer,
				image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			if (mipWidth > 1) mipWidth /= 2;
			if (mipHeight > 1) mipHeight /= 2;
		}

		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		EndSingleTimeCommands(commandBuffer);
	}

	VkCommandBuffer GPUProgramVulkan::BeginSingleTimeCommands() {
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = vkCtxPtr->commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(vkCtxPtr->device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void GPUProgramVulkan::EndSingleTimeCommands(VkCommandBuffer commandBuffer) {
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(vkCtxPtr->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(vkCtxPtr->graphicsQueue);

		vkFreeCommandBuffers(vkCtxPtr->device, vkCtxPtr->commandPool, 1, &commandBuffer);
	}

	bool HasStencilComponent(VkFormat format) {
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	void GPUProgramVulkan::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels) {
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;
		if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (HasStencilComponent(format)) {
				barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		else {
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else {
			LOGERROR("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		EndSingleTimeCommands(commandBuffer);
	}
	void GPUProgramVulkan::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			width,
			height,
			1
		};

		vkCmdCopyBufferToImage(
			commandBuffer,
			buffer,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);

		EndSingleTimeCommands(commandBuffer);
	}
	void GPUProgramVulkan::CreateTextureImageView(TextureVK* tex) {
		tex->textureImageView = CreateImageView(tex->textureImage, VK_FORMAT_R8G8B8A8_SRGB);
	}
	VkImageView GPUProgramVulkan::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		VkImageView view;
		if (vkCreateImageView(vkCtxPtr->device, &viewInfo, nullptr, &view) != VK_SUCCESS) {
			LOGERROR("failed to create texture image view!");
		}
		return view;
	}
	void GPUProgramVulkan::CreateTextureSampler(PipelineState& pso) {
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 16;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = static_cast<float>(mipLevels);
		samplerInfo.mipLodBias = 0.0f;

		if (vkCreateSampler(vkCtxPtr->device, &samplerInfo, nullptr, &pso.textureSampler) != VK_SUCCESS) {
			LOGERROR("failed to create texture sampler!");
		}
	}

	void GPUProgramVulkan::CreateRenderPass(PipelineState& pso) {
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = vkCtxPtr->swapChainImageFormat;
		colorAttachment.samples = msaaSamples;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = FindDepthFormat();
		depthAttachment.samples = msaaSamples;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription colorAttachmentResolve{};
		colorAttachmentResolve.format = vkCtxPtr->swapChainImageFormat;
		colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentResolveRef{};
		colorAttachmentResolveRef.attachment = 2;
		colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		subpass.pResolveAttachments = &colorAttachmentResolveRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(vkCtxPtr->device, &renderPassInfo, nullptr, &(pso.pipelineCtx->renderPass)) != VK_SUCCESS) {
			LOGERROR("failed to create render pass!");
		}
	}

	void GPUProgramVulkan::CreateGraphicsPipeline(PipelineState& pso) {
		std::string vs = pso.shader + ".vert";
		std::string fs = pso.shader + ".frag";
		std::string gs = "";
		SetShader(pso, vs, fs, gs);

		VkPipelineShaderStageCreateInfo* info = pso.shaderStage;


		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)vkCtxPtr->swapChainExtent.width;
		viewport.height = (float)vkCtxPtr->swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = vkCtxPtr->swapChainExtent;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_NONE;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = msaaSamples;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		VkDynamicState dynamicStates[] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_LINE_WIDTH
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = 2;
		dynamicState.pDynamicStates = dynamicStates;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1; // Optional
		VkDescriptorSetLayout layouts[] = { pso.pipelineCtx->descriptorSetLayout };
		pipelineLayoutInfo.pSetLayouts = layouts; // Optional

		if (vkCreatePipelineLayout(vkCtxPtr->device, &pipelineLayoutInfo, nullptr, &(pso.pipelineCtx->pipelineLayout)) != VK_SUCCESS) {
			LOGERROR("failed to create pipeline layout!");
		}

		CreateVertexBuffer(pso, pso.vb);
		CreateIndexBuffer(pso, pso.ib);
		//Create Vertex Buffer
		VkPipelineVertexInputStateCreateInfo* vertexInputInfo = pso.vb->GetVKVertexInputInfo();
		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = info;
		pipelineInfo.pVertexInputState = vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = nullptr; // Optional

		pipelineInfo.layout = pso.pipelineCtx->pipelineLayout;
		pipelineInfo.renderPass = pso.pipelineCtx->renderPass;
		pipelineInfo.subpass = 0;

		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		//pipelineInfo.basePipelineIndex = -1; // Optional
		if (vkCreateGraphicsPipelines(vkCtxPtr->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &(pso.pipelineCtx->graphicsPipeline)) != VK_SUCCESS) {
			LOGERROR("failed to create graphics pipeline!");
		}
	}

	void GPUProgramVulkan::CreateDescriptorSetLayout(PipelineState& pso) {
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(vkCtxPtr->device, &layoutInfo, nullptr, &(pso.pipelineCtx->descriptorSetLayout)) != VK_SUCCESS) {
			LOGERROR("failed to create descriptor set layout!");
		}
	}

	VKPipelineContext* GPUProgramVulkan::GetPipelineContext(PipelineState& pso) {
		pso.pipelineCtx = new VKPipelineContext;
		pso.fbCtx = new VKFrameBufferContext;
		CreateRenderPass(pso);
		CreateDescriptorSetLayout(pso);
		CreateGraphicsPipeline(pso);
		CreateColorResources(pso);
        CreateDepthResources(pso);
        CreateUniformBuffers();
        CreateFrameBuffers(pso);
		//CreateVertexBuffer();
		//CreateIndexBuffer();
		//CreateTextureImage(pso);
		//CreateTextureImageView(pso);
		CreateTextureSampler(pso);
		CreateDescriptorPool();
		CreateDescriptorSets(pso);

		return pso.pipelineCtx;
	}

	void GPUProgramVulkan::CreateColorResources(PipelineState& pso) {
		VkFormat colorFormat = vkCtxPtr->swapChainImageFormat;

		CreateImage(vkCtxPtr->swapChainExtent.width, vkCtxPtr->swapChainExtent.height, 1, msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, pso.fbCtx->colorImage, pso.fbCtx->colorImageMemory);
		pso.fbCtx->colorImageView = CreateImageView(pso.fbCtx->colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}

	void GPUProgramVulkan::CreateDepthResources(PipelineState& pso) {
		VkFormat depthFormat = FindDepthFormat();
		CreateImage(vkCtxPtr->swapChainExtent.width, vkCtxPtr->swapChainExtent.height, 1, msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, pso.fbCtx->depthImage, pso.fbCtx->depthImageMemory);
		pso.fbCtx->depthImageView = CreateImageView(pso.fbCtx->depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
		TransitionImageLayout(pso.fbCtx->depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
	}

	void GPUProgramVulkan::CreateUniformBuffers() {
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);
		
		vkCtxPtr->uniformBuffers.resize(vkCtxPtr->swapChainImages.size());
		vkCtxPtr->uniformBuffersMemory.resize(vkCtxPtr->swapChainImages.size());
		
		for (size_t i = 0; i < vkCtxPtr->swapChainImages.size(); i++) {
		    CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vkCtxPtr->uniformBuffers[i], vkCtxPtr->uniformBuffersMemory[i]);
		}
	}

	void GPUProgramVulkan::CreateFrameBuffers(PipelineState& pso) {
		vkCtxPtr->swapChainFramebuffers.resize(vkCtxPtr->swapChainImageViews.size());
		for (size_t i = 0; i < vkCtxPtr->swapChainImageViews.size(); i++) {
			std::array<VkImageView, 3> attachments = {
				pso.fbCtx->colorImageView,
				pso.fbCtx->depthImageView,
				vkCtxPtr->swapChainImageViews[i]
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = pso.pipelineCtx->renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = vkCtxPtr->swapChainExtent.width;
			framebufferInfo.height = vkCtxPtr->swapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(vkCtxPtr->device, &framebufferInfo, nullptr, &vkCtxPtr->swapChainFramebuffers[i]) != VK_SUCCESS) {
				LOGERROR("failed to create framebuffer!");
			}
		}
	}

	void GPUProgramVulkan::CreateDescriptorPool() {
		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(vkCtxPtr->swapChainImages.size());
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(vkCtxPtr->swapChainImages.size());

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(vkCtxPtr->swapChainImages.size());

		if (vkCreateDescriptorPool(vkCtxPtr->device, &poolInfo, nullptr, &vkCtxPtr->descriptorPool) != VK_SUCCESS) {
			LOGERROR("failed to create descriptor pool!");
		}
	}
	void GPUProgramVulkan::CreateDescriptorSets(PipelineState& pso) {
		std::vector<VkDescriptorSetLayout> layouts(vkCtxPtr->swapChainImages.size(), pso.pipelineCtx->descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = vkCtxPtr->descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(vkCtxPtr->swapChainImages.size());
		allocInfo.pSetLayouts = layouts.data();

		vkCtxPtr->descriptorSets.resize(vkCtxPtr->swapChainImages.size());
		if (vkAllocateDescriptorSets(vkCtxPtr->device, &allocInfo, vkCtxPtr->descriptorSets.data()) != VK_SUCCESS) {
			LOGERROR("failed to allocate descriptor sets!");
		}

		UpdateDescriptorSets(pso);
	}

	void GPUProgramVulkan::UpdateDescriptorSets(PipelineState& pso) {
		for (size_t i = 0; i < vkCtxPtr->swapChainImages.size(); i++) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = vkCtxPtr->uniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			std::map<uint32_t, TextureVK*>::iterator it = textureVKs.find(pso.textures[0]);
			TextureVK* tex;
			if (it == textureVKs.end()) {
				it = pendingTextureVKs.find(pso.textures[0]);
				if (it == pendingTextureVKs.end()) {
					LOGERROR("this texture didn't call Graphics::UpdateTexture!!!");
				}
				CreateTextureImage(it->second);
				CreateTextureImageView(it->second);
				textureVKs[it->first] = it->second;
				tex = it->second;
				pendingTextureVKs.erase(it);
			} else {
				tex = it->second;
			}
			imageInfo.imageView = tex->textureImageView;
			imageInfo.sampler = pso.textureSampler;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = vkCtxPtr->descriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = vkCtxPtr->descriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(vkCtxPtr->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}

	void GPUProgramVulkan::UpdateUniformBuffer(uint32_t currentImage) {
		if (vkCtxPtr->uniformBuffersMemory.empty()) return;
		void* data;
		vkMapMemory(vkCtxPtr->device, vkCtxPtr->uniformBuffersMemory[currentImage], 0, sizeof(currentPSO.ubo), 0, &data);
		memcpy(data, &currentPSO.ubo, sizeof(currentPSO.ubo));
		vkUnmapMemory(vkCtxPtr->device, vkCtxPtr->uniformBuffersMemory[currentImage]);

		//for (std::map<uint32_t, TextureVK*>::iterator it = pendingTextureVKs.begin(); it != pendingTextureVKs.end(); it++) {
		//	CreateTextureImage(it->second);
		//	CreateTextureImageView(it->second);
		//	UpdateDescriptorSets(currentPSO, it->second);
		//	textureVKs[it->first] = it->second;
		//}
		//pendingTextureVKs.clear();
	}

	void GPUProgramVulkan::RecordCommandBuffer(PipelineState& pso) {
		for (size_t i = 0; i < vkCtxPtr->commandBuffers.size(); i++) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = 0; // Optional
			beginInfo.pInheritanceInfo = nullptr; // Optional

			if (vkBeginCommandBuffer(vkCtxPtr->commandBuffers[i], &beginInfo) != VK_SUCCESS) {
				LOGERROR("failed to begin recording command buffer!");
			}
			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = pso.pipelineCtx->renderPass;
			renderPassInfo.framebuffer = vkCtxPtr->swapChainFramebuffers[i];
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = vkCtxPtr->swapChainExtent;
			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { pso.clearColor.x, pso.clearColor.y, pso.clearColor.z, pso.clearColor.w};
			clearValues[1].depthStencil = { 1.0f, 0 };

			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();
			vkCmdBeginRenderPass(vkCtxPtr->commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(vkCtxPtr->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pso.pipelineCtx->graphicsPipeline);

			VkBuffer vertexBuffer = pso.vertexBuffer;
			VkBuffer vertexBuffers[] = { vertexBuffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(vkCtxPtr->commandBuffers[i], 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(vkCtxPtr->commandBuffers[i], pso.indexBuffer, 0, VK_INDEX_TYPE_UINT16);

			vkCmdBindDescriptorSets(vkCtxPtr->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pso.pipelineCtx->pipelineLayout, 0, 1, &vkCtxPtr->descriptorSets[i], 0, nullptr);
			//vkCmdDrawIndexed(vkCtxPtr->commandBuffers[i], 9, 1, 0, 0, 0);
			vkCmdDrawIndexed(vkCtxPtr->commandBuffers[i], pso.ib->GetIndexCount(), 1, 0, 0, 0);
			//vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
			vkCmdEndRenderPass(vkCtxPtr->commandBuffers[i]);
			if (vkEndCommandBuffer(vkCtxPtr->commandBuffers[i]) != VK_SUCCESS) {
				LOGERROR("failed to record command buffer!");
			}
		}
	}

	void RenderCmdUpdateUniformBuffer(RenderCommand cmd, PipelineState& pso) {
		BUILTIN_MATRIX flag = (BUILTIN_MATRIX)cmd.flag;
		switch (flag) {
			case BUILTIN_MATRIX_MODEL: pso.ubo.model = *(Matrix4x4f*)cmd.data; break;
			case BUILTIN_MATRIX_VIEW: pso.ubo.view = *(Matrix4x4f*)cmd.data; break;
			case BUILTIN_MATRIX_PROJECTION: pso.ubo.proj = *(Matrix4x4f*)cmd.data; break;
			default: break;
		}
	}

	void RenderCmdUpdateVertexBuffer(RenderCommand cmd, PipelineState& pso) {
		
	}

	void RenderCmdUpdateIndexBuffer(RenderCommand cmd, PipelineState& pso) {
		BUILTIN_MATRIX flag = (BUILTIN_MATRIX)cmd.flag;
		switch (flag) {
		case BUILTIN_MATRIX_MODEL: pso.ubo.model = *(Matrix4x4f*)cmd.data; break;
		case BUILTIN_MATRIX_VIEW: pso.ubo.view = *(Matrix4x4f*)cmd.data; break;
		case BUILTIN_MATRIX_PROJECTION: pso.ubo.proj = *(Matrix4x4f*)cmd.data; break;
		default: break;
		}
	}

	void GPUProgramVulkan::ExecuteRenderCommand(std::vector<RenderCommand> cmdBuffer, uint16_t cmdIdx) {
		if (cmdIdx == 0) return;

		PipelineState pso;
		for (int i = 0; i < cmdIdx; ++i) {
			switch (cmdBuffer[i].typ) {
			case RenderCMD_Clear: pso.clearColor = *((Vector4f*)cmdBuffer[i].data); break;
			case RenderCMD_UpdateUniformBuffer: RenderCmdUpdateUniformBuffer(cmdBuffer[i], pso); break;
			case RenderCMD_UpdateVertexBuffer: {
				VertexBuffer* vb = (VertexBuffer*)cmdBuffer[i].data;
				pso.vb = vb;
				break;
			}
			case RenderCMD_UpdateIndexBuffer: {
				IndexBuffer* ib = (IndexBuffer*)cmdBuffer[i].data;
				pso.ib = ib;
				break;
			}
			case RenderCMD_UseShader: {
				std::string shader = (const char*)cmdBuffer[i].data;
				pso.shader = shader;
				break;
			}
			case RenderCMD_UpdateTexture: {
				Texture* tex = (Texture*)cmdBuffer[i].data;
				//check if uniform buffer is ready
				if (textureVKs.find(tex->id) == textureVKs.end() && pendingTextureVKs.find(tex->id) == pendingTextureVKs.end()) {
					TextureVK* vkTex = new TextureVK(tex);
					pendingTextureVKs[vkTex->ID()] = vkTex;
				}
				pso.textures.push_back(tex->id);
				break;
			}
			case RenderCMD_Draw: {
				if (!(currentPSO == pso)) {
					currentPSO = pso;
					GetPipelineContext(pso);
					RecordCommandBuffer(pso);
				} else {
					//still need to update ubo
					currentPSO.ubo = pso.ubo;
				}
				break;
			}
			case RenderCMD_DrawIndexed: {
				if (!(currentPSO == pso)) {
					GetPipelineContext(pso);
					RecordCommandBuffer(pso);
					currentPSO = pso;
				} else {
					//still need to update ubo
					currentPSO.ubo = pso.ubo;
				}
				break;
			}
			default: break;
			}
		}

		//for (size_t i = 0; i < vkCtxPtr->commandBuffers.size(); i++) {
		//	VkCommandBufferBeginInfo beginInfo{};
		//	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		//	beginInfo.flags = 0; // Optional
		//	beginInfo.pInheritanceInfo = nullptr; // Optional

		//	if (vkBeginCommandBuffer(vkCtxPtr->commandBuffers[i], &beginInfo) != VK_SUCCESS) {
		//		LOGERROR("failed to begin recording command buffer!");
		//	}

		//	VkRenderPassBeginInfo renderPassInfo{};
		//	for (int i = 0; i < cmdIdx; ++i) {
		//		switch (cmdBuffer[i].typ) {
		//		case RenderCMD_Clear:
		//		}
		//	}

		//	vkCmdEndRenderPass(vkCtxPtr->commandBuffers[i]);
		//	if (vkEndCommandBuffer(vkCtxPtr->commandBuffers[i]) != VK_SUCCESS) {
		//		LOGERROR("failed to record command buffer!");
		//	}
		//}
	}

	void GPUProgramVulkan::CleanupSwapChain() {
		vkDestroyImageView(vkCtxPtr->device, currentPSO.fbCtx->colorImageView, nullptr);
		vkDestroyImage(vkCtxPtr->device, currentPSO.fbCtx->colorImage, nullptr);
		vkFreeMemory(vkCtxPtr->device, currentPSO.fbCtx->colorImageMemory, nullptr);
		vkDestroyImageView(vkCtxPtr->device, currentPSO.fbCtx->depthImageView, nullptr);
		vkDestroyImage(vkCtxPtr->device, currentPSO.fbCtx->depthImage, nullptr);
		vkFreeMemory(vkCtxPtr->device, currentPSO.fbCtx->depthImageMemory, nullptr);
	}
}
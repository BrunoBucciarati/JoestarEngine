#include "GPUProgramVulkan.h"
#include "../IO/Log.h"
#include "../Misc/Application.h"
#include <array>
#include <direct.h>
#include "Image.h"

namespace Joestar {
	TextureVK::TextureVK(Texture* t) : texture(t) {
		
	}

	VkVertexInputBindingDescription VertexBufferVK::GetBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		uint32_t stride = 0;
		for (int i = 0; i < vb->attrs.size(); ++i) {
			stride += VERTEX_ATTRIBUTE_SIZE[vb->attrs[i]] * sizeof(float);
		}
		bindingDescription.stride = stride;
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}
	std::vector<VkVertexInputAttributeDescription> VertexBufferVK::GetAttributeDescriptions() {
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
		attributeDescriptions.resize(vb->attrs.size());

		uint32_t vaSize = 0, offset = 0;
		for (int i = 0; i < vb->attrs.size(); ++i) {
			attributeDescriptions[i].binding = 0;
			attributeDescriptions[i].location = i;
			attributeDescriptions[i].offset = offset;
			vaSize = VERTEX_ATTRIBUTE_SIZE[vb->attrs[i]];
			switch (vaSize) {
			case 3: attributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT; offset += 12; break;
			case 2: attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT; offset += 8;  break;
			default:break;
			}
		}

		return attributeDescriptions;
	}
	VkPipelineVertexInputStateCreateInfo* VertexBufferVK::GetVertexInputInfo() {
		bindingDescription = GetBindingDescription();
		attributeDescriptions = GetAttributeDescriptions();
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
		return &vertexInputInfo;
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

	GPUProgramVulkan::GPUProgramVulkan() : dynamicCommandBuffer(false) {
	}
	
	void GPUProgramVulkan::SetShader(ShaderVK* shader) {
		Application* app = Application::GetApplication();
		FileSystem* fs = app->GetSubSystem<FileSystem>();
		std::string path = fs->GetResourceDir();
		path += "Shaders/";
		char workDir[260];
		if (_getcwd(workDir, 260))
			path = workDir + ("/" + path);

		//First Compile To Spir-V
		std::string vertSpvPath = std::string(shader->GetName()) + "vert.spv";
		std::string fragSpvPath = std::string(shader->GetName()) + "frag.spv";
		std::string compileVertSpv = path + "glslc.exe " + (path + shader->GetName() + ".vert") + " -o " + (path + vertSpvPath);
		std::string compileFragSpv = path + "glslc.exe " + (path + shader->GetName() + ".frag") + " -o " + (path + fragSpvPath);
		system(compileVertSpv.c_str());
		system(compileFragSpv.c_str());
		File* vShaderCode = ShaderCodeFile(vertSpvPath.c_str());
		File* fShaderCode = ShaderCodeFile(fragSpvPath.c_str());

		shader->vertShaderModule = CreateShaderModule(vShaderCode);
		shader->fragShaderModule = CreateShaderModule(fShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = shader->vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = shader->fragShaderModule;
		fragShaderStageInfo.pName = "main";

		//VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
		shader->shaderStage[0] = vertShaderStageInfo;
		shader->shaderStage[1] = fragShaderStageInfo;
	}

	void GPUProgramVulkan::Clean() {
		//vkDestroyDescriptorSetLayout(vkCtxPtr->device, currentPSO.pipelineCtx->descriptorSetLayout, nullptr);
		//currentPSO.shader->Clear(vkCtxPtr->device);

		for (auto& shader : shaderVKs) {
			shader.second->Clean(vkCtxPtr->device);
		}

		for (auto& buffer : uniformVKs) {
			buffer.second->Clean(vkCtxPtr->device);
		}

		for (auto& buffer : ibs) {
			buffer.second->Clean(vkCtxPtr->device);
		}

		for (auto& buffer : vbs) {
			buffer.second->Clean(vkCtxPtr->device);
		}

		//vkDestroySampler(vkCtxPtr->device, currentPSO.textureSampler, nullptr);
		//vkDestroyImageView(vkCtxPtr->device, currentPSO.textureImageView, nullptr);
		//vkDestroyImage(vkCtxPtr->device, currentPSO.textureImage, nullptr);
		//vkFreeMemory(vkCtxPtr->device, currentPSO.textureImageMemory, nullptr);
		//delete mesh;

		//vkDestroyPipeline(vkCtxPtr->device, currentPSO.pipelineCtx->graphicsPipeline, nullptr);
		//vkDestroyPipelineLayout(vkCtxPtr->device, currentPSO.pipelineCtx->pipelineLayout, nullptr);
		//vkDestroyRenderPass(vkCtxPtr->device, currentPSO.pipelineCtx->renderPass, nullptr);
	}

	//VkPipelineVertexInputStateCreateInfo* GPUProgramVulkan::GetVertexInputInfo() {
	//	//return mesh->GetVB()->GetVKVertexInputInfo();
	//}

	void GPUProgramVulkan::CreateVertexBuffer(VertexBufferVK* vb) {
		VkDeviceSize bufferSize = vb->GetSize();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
		void* data;
		vkMapMemory(vkCtxPtr->device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vb->GetBuffer(), (size_t)bufferSize);
		vkUnmapMemory(vkCtxPtr->device, stagingBufferMemory);

		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vb->buffer, vb->memory);
		CopyBuffer(stagingBuffer, vb->buffer, bufferSize);

		vkDestroyBuffer(vkCtxPtr->device, stagingBuffer, nullptr);
		vkFreeMemory(vkCtxPtr->device, stagingBufferMemory, nullptr);
	}

	void GPUProgramVulkan::CreateIndexBuffer(IndexBufferVK* ib) {
		VkDeviceSize bufferSize = ib->GetSize();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(vkCtxPtr->device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, ib->GetBuffer(), (size_t)bufferSize);
		vkUnmapMemory(vkCtxPtr->device, stagingBufferMemory);

		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, ib->buffer, ib->memory);

		CopyBuffer(stagingBuffer, ib->buffer, bufferSize);

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
		imageInfo.samples = numSamples;
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
	void GPUProgramVulkan::CreateTextureSampler(RenderPassVK* pass, int i) {
		PipelineStateVK* pso = pass->dcs[i]->pso;
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 1;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = static_cast<float>(mipLevels);
		samplerInfo.mipLodBias = 0.0f;

		if (vkCreateSampler(vkCtxPtr->device, &samplerInfo, nullptr, &pso->textureSampler) != VK_SUCCESS) {
			LOGERROR("failed to create texture sampler!");
		}
	}

	void GPUProgramVulkan::CreateRenderPass(RenderPassVK* pass) {
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = vkCtxPtr->swapChainImageFormat;
		colorAttachment.samples = pass->msaaSamples;
		colorAttachment.loadOp = pass->clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = pass->msaa ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_GENERAL;

		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = FindDepthFormat();
		depthAttachment.samples = pass->msaaSamples;
		depthAttachment.loadOp = pass->clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkRenderPassCreateInfo renderPassInfo{};
		if (pass->msaa) {
			VkAttachmentDescription colorAttachmentResolve{};
			colorAttachmentResolve.format = vkCtxPtr->swapChainImageFormat;
			colorAttachmentResolve.samples = pass->msaaSamples;
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
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			renderPassInfo.pAttachments = attachments.data();
			renderPassInfo.subpassCount = 1;
			renderPassInfo.pSubpasses = &subpass;
			renderPassInfo.dependencyCount = 1;
			renderPassInfo.pDependencies = &dependency;

			if (vkCreateRenderPass(vkCtxPtr->device, &renderPassInfo, nullptr, &(pass->renderPass)) != VK_SUCCESS) {
				LOGERROR("failed to create render pass!");
			}
		} else {
			VkSubpassDescription subpass{};
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &colorAttachmentRef;
			subpass.pDepthStencilAttachment = &depthAttachmentRef;

			VkSubpassDependency dependency{};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.srcAccessMask = 0;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
			renderPassInfo.pAttachments = attachments.data();
			renderPassInfo.subpassCount = 1;
			renderPassInfo.pSubpasses = &subpass;
			renderPassInfo.dependencyCount = 1;
			renderPassInfo.pDependencies = &dependency;

			if (vkCreateRenderPass(vkCtxPtr->device, &renderPassInfo, nullptr, &(pass->renderPass)) != VK_SUCCESS) {
				LOGERROR("failed to create render pass!");
			}
		}

		CreateFrameBuffers(pass);
	}

	void GPUProgramVulkan::CreateGraphicsPipeline(RenderPassVK* pass, int i) {
		DrawCallVK* dc = pass->dcs[i];
		PipelineStateVK* pso = dc->pso;
		SetShader(dc->shader);

		VkPipelineShaderStageCreateInfo* info = dc->shader->shaderStage;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		if (dc->topology == MESH_TOPOLOGY_TRIANGLE_STRIP) {
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		} else {
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		}
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
		multisampling.rasterizationSamples = pass->msaaSamples;
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
		VkDescriptorSetLayout layouts[] = { pso->descriptorSetLayout };
		pipelineLayoutInfo.pSetLayouts = layouts; // Optional
		
		std::string pushConstsName = dc->shader->GetPushConsts();
		if (!pushConstsName.empty()) {
			VkPushConstantRange pushConstantRange{};
			pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			pushConstantRange.offset = 0;
			pushConstantRange.size = sizeof(PushConsts);
			pipelineLayoutInfo.pushConstantRangeCount = 1; // Optional
			pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; // Optional
		}

		if (vkCreatePipelineLayout(vkCtxPtr->device, &pipelineLayoutInfo, nullptr, &(pso->pipelineLayout)) != VK_SUCCESS) {
			LOGERROR("failed to create pipeline layout!");
		}

		//Create Vertex Buffer
		VkPipelineVertexInputStateCreateInfo* vertexInputInfo = dc->vb->GetVertexInputInfo();
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

		pipelineInfo.layout = pso->pipelineLayout;
		pipelineInfo.renderPass = pass->renderPass;
		pipelineInfo.subpass = 0;

		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		//pipelineInfo.basePipelineIndex = -1; // Optional
		if (vkCreateGraphicsPipelines(vkCtxPtr->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &(pso->graphicsPipeline)) != VK_SUCCESS) {
			LOGERROR("failed to create graphics pipeline!");
		}
	}

	void GPUProgramVulkan::CreateDescriptorSetLayout(RenderPassVK* pass, int i) {
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


		PipelineStateVK* pso = pass->dcs[i]->pso;
		if (vkCreateDescriptorSetLayout(vkCtxPtr->device, &layoutInfo, nullptr, &(pso->descriptorSetLayout)) != VK_SUCCESS) {
			LOGERROR("failed to create descriptor set layout!");
		}
	}

	void GPUProgramVulkan::GetPipeline(RenderPassVK* pass, int i) {
		CreateDescriptorSetLayout(pass, i);
		CreateGraphicsPipeline(pass, i);
		CreateTextureSampler(pass, i);
		CreateDescriptorPool();
		CreateDescriptorSets(pass->dcs[i]);
	}

	void GPUProgramVulkan::CreateColorResources(RenderPassVK* pass) {
		VkFormat colorFormat = vkCtxPtr->swapChainImageFormat;

		CreateImage(vkCtxPtr->swapChainExtent.width, vkCtxPtr->swapChainExtent.height, 1, pass->msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, pass->fb->colorImage, pass->fb->colorImageMemory);
		pass->fb->colorImageView = CreateImageView(pass->fb->colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}

	void GPUProgramVulkan::CreateDepthResources(RenderPassVK* pass) {
		VkFormat depthFormat = FindDepthFormat();
		CreateImage(vkCtxPtr->swapChainExtent.width, vkCtxPtr->swapChainExtent.height, 1, pass->msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, pass->fb->depthImage, pass->fb->depthImageMemory);
		pass->fb->depthImageView = CreateImageView(pass->fb->depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
		TransitionImageLayout(pass->fb->depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
	}

	void GPUProgramVulkan::CreateUniformBuffers(UniformBufferVK* ub) {
		VkDeviceSize bufferSize = ub->size;
		
		ub->buffers.resize(vkCtxPtr->swapChainImages.size());

		for (size_t i = 0; i < vkCtxPtr->swapChainImages.size(); i++) {
		    CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, ub->buffers[i].buffer, ub->buffers[i].memory);
		}
	}

	void GPUProgramVulkan::CreateFrameBuffers(RenderPassVK* pass) {
		if (!vkCtxPtr->swapChainFramebuffers.empty()) return;
		pass->fb = new FrameBufferVK;
		CreateColorResources(pass);
		CreateDepthResources(pass);
		vkCtxPtr->swapChainFramebuffers.resize(vkCtxPtr->swapChainImageViews.size());
		for (size_t i = 0; i < vkCtxPtr->swapChainImageViews.size(); i++) {
			std::vector<VkImageView> attachments;
			if (pass->msaa) {
				attachments = {
					pass->fb->colorImageView,
					pass->fb->depthImageView,
					vkCtxPtr->swapChainImageViews[i]
				};
			} else {
				attachments = {
					vkCtxPtr->swapChainImageViews[i],
					pass->fb->depthImageView,
				};
			}

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = pass->renderPass;
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
	void GPUProgramVulkan::CreateDescriptorSets(DrawCallVK* dc) {
		std::vector<VkDescriptorSetLayout> layouts(vkCtxPtr->swapChainImages.size(), dc->pso->descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = vkCtxPtr->descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(vkCtxPtr->swapChainImages.size());
		allocInfo.pSetLayouts = layouts.data();

		vkCtxPtr->descriptorSets.resize(vkCtxPtr->swapChainImages.size());
		if (vkAllocateDescriptorSets(vkCtxPtr->device, &allocInfo, vkCtxPtr->descriptorSets.data()) != VK_SUCCESS) {
			LOGERROR("failed to allocate descriptor sets!");
		}

		UpdateDescriptorSets(dc);
	}

	void GPUProgramVulkan::UpdateDescriptorSets(DrawCallVK* dc) {
		for (size_t i = 0; i < vkCtxPtr->swapChainImages.size(); ++i) {
			std::vector<VkWriteDescriptorSet> descriptorWrites{};
			descriptorWrites.resize(dc->shader->ubs.size());
			int samplerCount = 0;
			for (int j = 0; j < dc->shader->ubs.size(); ++j) {
				UniformBufferVK* ub = uniformVKs[dc->shader->ubs[j]];
				if (ub->texID > 0) {
					VkDescriptorImageInfo imageInfo{};
					imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					std::map<uint32_t, TextureVK*>::iterator it = textureVKs.find(dc->shader->textures[0]);
					TextureVK* tex;
					if (it == textureVKs.end()) {
						it = pendingTextureVKs.find(dc->shader->textures[0]);
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
					imageInfo.sampler = dc->pso->textureSampler;

					descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[j].dstSet = vkCtxPtr->descriptorSets[i];
					descriptorWrites[j].dstBinding = dc->shader->GetSamplerBinding(samplerCount++);
					descriptorWrites[j].dstArrayElement = 0;
					descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					descriptorWrites[j].descriptorCount = 1;
					descriptorWrites[j].pImageInfo = &imageInfo;

				} else {
					VkDescriptorBufferInfo bufferInfo{};
					bufferInfo.buffer = ub->buffers[i].buffer;
					bufferInfo.offset = 0;
					bufferInfo.range = sizeof(UniformBufferObject);
					
					descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[j].dstSet = vkCtxPtr->descriptorSets[i];
					descriptorWrites[j].dstBinding = dc->shader->GetUniformBindingByName(ub->name);
					descriptorWrites[j].dstArrayElement = 0;
					descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					descriptorWrites[j].descriptorCount = 1;
					descriptorWrites[j].pBufferInfo = &bufferInfo;
				}
			}
			vkUpdateDescriptorSets(vkCtxPtr->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
			//descriptorWrites.clear();
		}
	}

	void GPUProgramVulkan::UpdateUniformBuffer(uint32_t currentImage) {
		void* data;

		for (auto& ub : uniformVKs) {
			if (ub.second->texID > 0) continue;
			vkMapMemory(vkCtxPtr->device, ub.second->buffers[currentImage].memory, 0, ub.second->size, 0, &data);
			memcpy(data, ub.second->data, ub.second->size);
			vkUnmapMemory(vkCtxPtr->device, ub.second->buffers[currentImage].memory);
		}
	}

	void GPUProgramVulkan::RecordRenderPass(RenderPassVK* pass, int i) {
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		CreateRenderPass(pass);
		renderPassInfo.renderPass = pass->renderPass;
		renderPassInfo.framebuffer = vkCtxPtr->swapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = vkCtxPtr->swapChainExtent;
		std::array<VkClearValue, 2> clearValues{};
		if (pass->clear) {
			clearValues[0].color = { pass->clearColor.x, pass->clearColor.y, pass->clearColor.z, pass->clearColor.w };
			clearValues[1].depthStencil = { 1.0f, 0 };
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();
		} else {

			//renderPassInfo.pClearValues = clearValues.data();
		}

		vkCmdBeginRenderPass(vkCtxPtr->commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		std::string pushConsts;
		for (int j = 0; j < pass->dcs.size(); ++j) {
			DrawCallVK* dc = pass->dcs[j];
			GetPipeline(pass, j);
			vkCmdBindPipeline(vkCtxPtr->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, dc->pso->graphicsPipeline);

			VkBuffer vertexBuffer = dc->vb->buffer;
			VkBuffer vertexBuffers[] = { vertexBuffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(vkCtxPtr->commandBuffers[i], 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(vkCtxPtr->commandBuffers[i], dc->ib->buffer, 0, VK_INDEX_TYPE_UINT16);

			vkCmdBindDescriptorSets(vkCtxPtr->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, dc->pso->pipelineLayout, 0, 1, &vkCtxPtr->descriptorSets[i], 0, nullptr);

			pushConsts = dc->shader->GetPushConsts();
			if (!pushConsts.empty()) {
				vkCmdPushConstants(vkCtxPtr->commandBuffers[i], dc->pso->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConsts), dc->pc);
			}
			vkCmdDrawIndexed(vkCtxPtr->commandBuffers[i], dc->ib->GetIndexCount(), 1, 0, 0, 0);
		}
		vkCmdEndRenderPass(vkCtxPtr->commandBuffers[i]);
	}

	void GPUProgramVulkan::RecordCommandBuffer(std::vector<RenderPassVK*>& passes) {
		for (size_t i = 0; i < vkCtxPtr->commandBuffers.size(); i++) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = 0; // Optional
			beginInfo.pInheritanceInfo = nullptr; // Optional

			if (vkBeginCommandBuffer(vkCtxPtr->commandBuffers[i], &beginInfo) != VK_SUCCESS) {
				LOGERROR("failed to begin recording command buffer!");
			}
			for (int j = 0; j < passes.size(); ++j) {
				RecordRenderPass(passes[j], i);
			}
			if (vkEndCommandBuffer(vkCtxPtr->commandBuffers[i]) != VK_SUCCESS) {
				LOGERROR("failed to record command buffer!");
			}
		}
	}

	void GPUProgramVulkan::RenderCmdUpdateUniformBufferObject(RenderCommand& cmd) {
		BUILTIN_MATRIX flag = (BUILTIN_MATRIX)cmd.flag;
		std::string name = "UniformBufferObject";
		//built in name
		uint32_t hashID = hashString(name.c_str());
		if (uniformVKs.find(hashID) == uniformVKs.end()) {
			uniformVKs[hashID] = new UniformBufferVK;
			uniformVKs[hashID]->name = name;
			uniformVKs[hashID]->size = sizeof(UniformBufferObject);
			uniformVKs[hashID]->id = hashID;
			uniformVKs[hashID]->data = new UniformBufferObject;

			CreateUniformBuffers(uniformVKs[hashID]);
		}

		switch (flag) {
			case BUILTIN_MATRIX_VIEW: {
				((UniformBufferObject*)uniformVKs[hashID]->data)->view = *(Matrix4x4f*)cmd.data;
				break; 
			}
			case BUILTIN_MATRIX_PROJECTION: { 
				((UniformBufferObject*)uniformVKs[hashID]->data)->proj = *(Matrix4x4f*)cmd.data;
				break; 
			}
			//case BUILTIN_MATRIX_MODEL: {
			//	((UniformBufferObject*)uniformVKs[hashID]->data)->model = *(Matrix4x4f*)cmd.data;
			//	break;
			//}
			default: break;
		}
		if (name.empty()) {
			LOGERROR("update ubo but type not recognize!");
			return;
		}
	}

	void GPUProgramVulkan::RenderCmdUpdateUniformBuffer(RenderCommand cmd, DrawCallVK* dc) {
		BUILTIN_MATRIX flag = (BUILTIN_MATRIX)cmd.flag;
		switch (flag) {
		case BUILTIN_MATRIX_MODEL: {
			//built in name
			dc->pc = new PushConstsVK;
			dc->pc->model = *(Matrix4x4f*)cmd.data;
			break; 
		}
		default: break;
		}
	}

#define CHECK_PASS() if(pass == nullptr) {LOGERROR("PLEASE START PASS FIRST!\n");}
	void GPUProgramVulkan::ExecuteRenderCommand(std::vector<RenderCommand>& cmdBuffer, uint16_t cmdIdx, U16 imgIdx) {
		if (cmdIdx == 0) return;
		//for test, we only record once now
		if (!renderPassList.empty()) return;
		curImageIdx = imgIdx;

		RenderPassVK* pass = nullptr;
		DrawCallVK* drawcall = new DrawCallVK;
		drawcall->pso = new PipelineStateVK;
		for (int i = 0; i < cmdIdx; ++i) {
			switch (cmdBuffer[i].typ) {
			case RenderCMD_BeginRenderPass: {
				pass = new RenderPassVK;
				pass->name = ((const char*)cmdBuffer[i].data);
				break;
			}
			case RenderCMD_EndRenderPass: {
				CHECK_PASS()
				for (auto& dc : pass->dcs) {
					pass->HashInsert(dc->hash);
				}
				renderPassList.push_back(pass);
				break;
			}
			case RenderCMD_Clear: {
				CHECK_PASS()
				pass->clear = true;
				pass->clearColor = *((Vector4f*)cmdBuffer[i].data);
				break; 
			}
			case RenderCMD_UpdateUniformBufferObject: {
				RenderCmdUpdateUniformBufferObject(cmdBuffer[i]);
				UpdateUniformBuffer(imgIdx);
				break; 
			}
			case RenderCMD_UpdateUniformBuffer: {
				RenderCmdUpdateUniformBuffer(cmdBuffer[i], drawcall);
				break; 
			}
			case RenderCMD_UpdateVertexBuffer: {
				CHECK_PASS()
				VertexBuffer* vb = (VertexBuffer*)cmdBuffer[i].data;
				if (vbs.find(vb->id) == vbs.end()) {
					vbs[vb->id] = new VertexBufferVK(vb);
					CreateVertexBuffer(vbs[vb->id]);
				}
				drawcall->vb = vbs[vb->id];
				drawcall->HashInsert(vb->id);
				break;
			}
			case RenderCMD_UpdateIndexBuffer: {
				CHECK_PASS()
				IndexBuffer* ib = (IndexBuffer*)cmdBuffer[i].data;
				if (ibs.find(ib->id) == ibs.end()) {
					ibs[ib->id] = new IndexBufferVK(ib);
					CreateIndexBuffer(ibs[ib->id]);
				}
				drawcall->ib = ibs[ib->id];
				drawcall->HashInsert(ib->id);
				break;
			}
			case RenderCMD_UseShader: {
				CHECK_PASS()
				Shader* shader = (Shader*)cmdBuffer[i].data;
				if (shaderVKs.find(shader->id) == shaderVKs.end()) {
					shaderVKs[shader->id] = new ShaderVK(shader);
				}
				drawcall->shader = shaderVKs[shader->id];
				drawcall->HashInsert(shader->id);
				break;
			}
			case RenderCMD_UpdateTexture: {
				CHECK_PASS()
				Texture* tex = (Texture*)cmdBuffer[i].data;
				U8 binding = cmdBuffer[i].flag;
				//check if uniform buffer is ready
				if (textureVKs.find(tex->id) == textureVKs.end() && pendingTextureVKs.find(tex->id) == pendingTextureVKs.end()) {
					TextureVK* vkTex = new TextureVK(tex);
					pendingTextureVKs[vkTex->ID()] = vkTex;
				}
				drawcall->shader->textures.push_back(tex->id);

				if (uniformVKs.find(tex->id) == uniformVKs.end()) {
					UniformBufferVK* texUB = new UniformBufferVK;
					texUB->texID = tex->id;
					uniformVKs[tex->id] = texUB;
				}
				//update binding tex
				drawcall->shader->ubs[binding] = tex->id;
				drawcall->HashInsert(tex->id);
				break;
			}
			case RenderCMD_Draw: {
				CHECK_PASS()
				MeshTopology topology = (MeshTopology)cmdBuffer[i].flag;
				drawcall->topology = topology;
				drawcall->HashInsert(topology);
				break;
			}
			case RenderCMD_DrawIndexed: {
				CHECK_PASS()
				MeshTopology topology = (MeshTopology)cmdBuffer[i].flag;
				drawcall->topology = topology;
				drawcall->HashInsert(topology);
				pass->dcs.push_back(drawcall);

				drawcall = new DrawCallVK;
				drawcall->pso = new PipelineStateVK;
				break;
			}
			default: break;
			}
		}

		bool needRecord = false;
		if (lastRenderPassList.size() != renderPassList.size()) {
			needRecord = true;
		} else {
			for (int i = 0; i < lastRenderPassList.size(); ++i) {
				if (lastRenderPassList[i]->hash != renderPassList[i]->hash) {
					needRecord = true;
					break;
				} else {
					//update push consts
					for (int j = 0; j < renderPassList[i]->dcs.size(); ++j) {
						if (renderPassList[i]->dcs[j]->pc) {
							*(lastRenderPassList[i]->dcs[j]->pc) = *(renderPassList[i]->dcs[j]->pc);
						}
					}
				}
			}
		}
		if (needRecord) {
			RecordCommandBuffer(renderPassList);
		}


		delete drawcall->pso;
		delete drawcall;

		lastRenderPassList.clear();
		lastRenderPassList.swap(renderPassList);
	}

	void GPUProgramVulkan::CleanupSwapChain() {
		for (auto& fb : fbs) {
			fb.second->Clean(vkCtxPtr->device);
		}
	}
}
#include "GPUProgramVulkan.h"
#include "../IO/Log.h"
#include "../Misc/Application.h"
#include <array>
#include "../IO/MemoryManager.h"
#include "Image.h"

namespace Joestar {
	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice& device) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(device, &memProperties);
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		LOGERROR("failed to find suitable memory type!");
	}

	VkCompareOp VKCompareOps[] = {
		VK_COMPARE_OP_NEVER, VK_COMPARE_OP_ALWAYS, VK_COMPARE_OP_LESS, VK_COMPARE_OP_LESS_OR_EQUAL,
		VK_COMPARE_OP_GREATER, VK_COMPARE_OP_GREATER_OR_EQUAL, VK_COMPARE_OP_EQUAL, VK_COMPARE_OP_NOT_EQUAL
	};

	VkPolygonMode VKPolygonModes[] = {
		VK_POLYGON_MODE_FILL, VK_POLYGON_MODE_LINE
	};

	TextureVK::TextureVK(Texture* t) : texture(t), image(nullptr) {
		
	}

	VkVertexInputBindingDescription VertexBufferVK::GetBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = binding;
		uint32_t stride = 0;

		for (int i = 0; i < vb->attrs.size(); ++i) {
			stride += VERTEX_ATTRIBUTE_SIZE[vb->attrs[i]] * sizeof(float);
		}
		if (instance) {
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
		} else {
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		}
		bindingDescription.stride = stride;

		return bindingDescription;
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

	GPUProgramVulkan::GPUProgramVulkan() : dynamicCommandBuffer(false), firstRecord(true) {
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
			buffer.second->Clean();
		}

		for (auto& buffer : vbs) {
			buffer.second->Clean();
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

	void GPUProgramVulkan::CreateVertexBuffer(VertexBufferVK* vb) {
		VkDeviceSize bufferSize = vb->GetSize();

		BufferVK stagingBuffer {
			vkCtxPtr, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		stagingBuffer.Create();
		stagingBuffer.CopyBuffer(vb->GetBuffer());

		vb->buffer.size = bufferSize;
		vb->buffer.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		vb->buffer.Create();
		CopyBuffer(stagingBuffer, vb->buffer, bufferSize);
	}

	void GPUProgramVulkan::CreateIndexBuffer(IndexBufferVK* ib) {
		VkDeviceSize bufferSize = ib->GetSize();

		BufferVK stagingBuffer{
			vkCtxPtr, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		stagingBuffer.Create();
		stagingBuffer.CopyBuffer(ib->GetBuffer());

		ib->buffer.size = bufferSize;
		ib->buffer.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		ib->buffer.Create();

		CopyBuffer(stagingBuffer, ib->buffer, bufferSize);
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

	void GPUProgramVulkan::CopyBuffer(BufferVK& srcBuffer, BufferVK& dstBuffer, VkDeviceSize size) {
		CommandBufferVK cb{ vkCtxPtr };
		cb.Begin();

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = size;
		vkCmdCopyBuffer(cb.commandBuffer, srcBuffer.buffer, dstBuffer.buffer, 1, &copyRegion);

		cb.End();
	}

	void GPUProgramVulkan::CreateTextureImage(TextureVK* tex, UniformBufferVK* ub) {
		if (tex->image) return;
		VkDeviceSize imageSize = tex->GetSize();
		mipLevels = tex->HasMipmap() ? static_cast<uint32_t>(std::floor(std::log2(Max(tex->GetWidth(), tex->GetHeight())))) + 1 : 1;
		tex->image = new ImageVK;
		tex->image->ctx = vkCtxPtr;
		tex->image->width = tex->GetWidth();
		tex->image->height = tex->GetHeight();
		tex->image->mipLevels = mipLevels;
		tex->image->viewType = (tex->Type() == TEXTURE_CUBEMAP ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D);
		tex->image->imageType = VK_IMAGE_TYPE_2D;
		if (ub->IsImage()) {
			tex->image->format = VK_FORMAT_R8G8B8A8_UNORM;
			tex->image->usage = tex->image->usage | VK_IMAGE_USAGE_STORAGE_BIT;
		}
		tex->image->Create();

		BufferVK stagingBuffer{
			vkCtxPtr,
			imageSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		stagingBuffer.Create();
		stagingBuffer.CopyBuffer((U8*)tex->GetData());

		//for mipmap
		CommandBufferVK cb{ vkCtxPtr };
		cb.Begin();
		if (ub->IsSampler()) {
			tex->image->TransitionImageLayout(cb, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			tex->image->CopyBufferToImage(stagingBuffer, cb);
			if (tex->HasMipmap()) {
				tex->image->GenerateMipmaps(cb);
			} else {
				tex->image->TransitionImageLayout(cb, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, ub->GetTargetImageLayout());
			}
		} else if (ub->IsImage()) {
			tex->image->TransitionImageLayout(cb, VK_IMAGE_LAYOUT_UNDEFINED, ub->GetTargetImageLayout());
		}
		tex->image->CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT, cb);
		cb.End();
	}

	void GPUProgramVulkan::CreateTextureSampler(DrawCallVK* dc) {
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

		if (vkCreateSampler(vkCtxPtr->device, &samplerInfo, nullptr, &dc->textureSampler) != VK_SUCCESS) {
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
		dc->shader->Prepare();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		if (dc->topology == MESH_TOPOLOGY_TRIANGLE_STRIP) {
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		} else if (dc->topology == MESH_TOPOLOGY_LINE) {
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		} else if (dc->topology == MESH_TOPOLOGY_LINE_STRIP) {
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
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
		rasterizer.polygonMode = dc->polygonMode;
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
		depthStencil.depthCompareOp = dc->depthOp;
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

		CreatePipelineLayout<DrawCallVK>(dc);

		//Create Vertex Buffer
		VkPipelineVertexInputStateCreateInfo& vertexInputInfo = dc->GetVertexInputInfo();
		VkPipelineShaderStageCreateInfo* shaderCreateInfo = dc->shader->shaderStage.data();
		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderCreateInfo;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = nullptr; // Optional

		pipelineInfo.layout = dc->pipelineLayout;
		pipelineInfo.renderPass = pass->renderPass;
		pipelineInfo.subpass = 0;

		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		//pipelineInfo.basePipelineIndex = -1; // Optional
		if (vkCreateGraphicsPipelines(vkCtxPtr->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &(dc->graphicsPipeline)) != VK_SUCCESS) {
			LOGERROR("failed to create graphics pipeline!");
		}
	}

	void GPUProgramVulkan::GetPipeline(RenderPassVK* pass, int i) {
		CreateDescriptorSetLayout<DrawCallVK>(pass->dcs[i]);
		CreateGraphicsPipeline(pass, i);
		CreateTextureSampler(pass->dcs[i]);
		CreateDescriptorPool<DrawCallVK>(pass->dcs[i]);
		CreateDescriptorSets<DrawCallVK>(pass->dcs[i]);
	}

	void GPUProgramVulkan::CreateColorResources(RenderPassVK* pass) {
		VkFormat colorFormat = vkCtxPtr->swapChainImageFormat;
		pass->fb->colorImage = new ImageVK{
			vkCtxPtr, vkCtxPtr->swapChainExtent.width, vkCtxPtr->swapChainExtent.height, 1, pass->msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		};
		CommandBufferVK cb{ vkCtxPtr };
		cb.Begin();
		pass->fb->colorImage->Create();
		pass->fb->colorImage->CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT, cb);
		cb.End();
	}

	void GPUProgramVulkan::CreateDepthResources(RenderPassVK* pass) {
		VkFormat depthFormat = FindDepthFormat();
		pass->fb->depthImage = new ImageVK{
			vkCtxPtr,
			vkCtxPtr->swapChainExtent.width, vkCtxPtr->swapChainExtent.height,
			1, pass->msaaSamples,  depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		};
		pass->fb->depthImage->Create();
		CommandBufferVK cb{ vkCtxPtr };
		cb.Begin();
		pass->fb->depthImage->CreateImageView(VK_IMAGE_ASPECT_DEPTH_BIT, cb);
		pass->fb->depthImage->TransitionImageLayout(cb, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT);
		cb.End();
	}

	void GPUProgramVulkan::CreateUniformBuffers(UniformBufferVK* ub) {
		VkDeviceSize bufferSize = ub->size;
		
		ub->buffers.resize(vkCtxPtr->swapChainImages.size());

		for (size_t i = 0; i < vkCtxPtr->swapChainImages.size(); i++) {
			ub->buffers[i] = { vkCtxPtr, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };
			ub->buffers[i].Create();
		}
	}

	void GPUProgramVulkan::CreateComputeBuffers(ComputeBufferVK* ub) {
		VkDeviceSize bufferSize = ub->size;

		ub->buffers.resize(vkCtxPtr->swapChainImages.size());

		for (size_t i = 0; i < vkCtxPtr->swapChainImages.size(); i++) {
			ub->buffers[i] = { vkCtxPtr, bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT };
			ub->buffers[i].Create();
		}
	}

	void GPUProgramVulkan::CreateFrameBuffers(RenderPassVK* pass) {
		if (!vkCtxPtr->swapChainFramebuffers.empty()) return;
		pass->fb = new FrameBufferVK{vkCtxPtr};
		CreateColorResources(pass);
		CreateDepthResources(pass);
		vkCtxPtr->swapChainFramebuffers.resize(vkCtxPtr->swapChainImageViews.size());
		for (size_t i = 0; i < vkCtxPtr->swapChainImageViews.size(); i++) {
			std::vector<VkImageView> attachments;
			if (pass->msaa) {
				attachments = {
					pass->fb->colorImage->imageView,
					pass->fb->depthImage->imageView,
					vkCtxPtr->swapChainImageViews[i]
				};
			} else {
				attachments = {
					vkCtxPtr->swapChainImageViews[i],
					pass->fb->depthImage->imageView
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

	void GPUProgramVulkan::UpdateUniformBuffer(uint32_t currentImage) {
		void* data;

		for (auto& ub : uniformVKs) {
			if (ub.second->IsUniform())
				ub.second->buffers[currentImage].CopyBuffer((U8*)ub.second->data);
		}
	}

	void GPUProgramVulkan::RecordRenderPass(RenderPassVK* pass, int i) {
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
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
			//only get pipeline at first command buffer and reuse
			vkCmdBindPipeline(vkCtxPtr->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, dc->graphicsPipeline);

			std::vector<VkBuffer> vertexBuffers;
			vertexBuffers.resize(dc->vbs.size());
			for (int idx = 0; idx < dc->vbs.size(); ++idx) {
				vertexBuffers[idx] = dc->vbs[idx]->buffer.buffer;
			}
			VkDeviceSize offsets[] = { 0, 0 };
			vkCmdBindVertexBuffers(vkCtxPtr->commandBuffers[i], 0, vertexBuffers.size(), vertexBuffers.data(), offsets);

			vkCmdBindDescriptorSets(vkCtxPtr->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, dc->pipelineLayout, 0, 1, &dc->descriptorSets[i], 0, nullptr);

			PushConstsVK* pc = dc->shader->pushConst;
			if (nullptr != pc) {
				vkCmdPushConstants(vkCtxPtr->commandBuffers[i], dc->pipelineLayout, pc->GetStageFlags(), 0, pc->size, pc->data);
			}

			if (dc->ib) {
				vkCmdBindIndexBuffer(vkCtxPtr->commandBuffers[i], dc->ib->buffer.buffer, 0, VK_INDEX_TYPE_UINT16);
				if (dc->vbs.size() > 1)
					vkCmdDrawIndexed(vkCtxPtr->commandBuffers[i], dc->ib->GetIndexCount(), dc->vbs[1]->GetInstanceCount(), 0, 0, 0);
				else
					vkCmdDrawIndexed(vkCtxPtr->commandBuffers[i], dc->ib->GetIndexCount(), 1, 0, 0, 0);

			} else {
				vkCmdDraw(vkCtxPtr->commandBuffers[i], dc->vbs[0]->GetVertexCount(), 1, 0, 0);
			}
		}
		vkCmdEndRenderPass(vkCtxPtr->commandBuffers[i]);
	}

	void GPUProgramVulkan::RecordCommandBuffer(std::vector<RenderPassVK*>& passes) {
		//record into next frame's command buffer
		//if (vkCtxPtr->commandBuffers.empty()) CreateCommandBuffers();
		int nextIdx = curImageIdx == vkCtxPtr->swapChainImages.size() - 1 ? 0 : curImageIdx + 1;
		for (size_t i = (firstRecord ? 0 : nextIdx); i < (firstRecord ? vkCtxPtr->swapChainImages.size() : nextIdx + 1); i++) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = 0; // Optional
			beginInfo.pInheritanceInfo = nullptr; // Optional

			VkCommandBuffer& buf = vkCtxPtr->commandBuffers[i];
			if (vkBeginCommandBuffer(buf, &beginInfo) != VK_SUCCESS) {
				LOGERROR("failed to begin recording command buffer!");
			}
			for (int j = 0; j < passes.size(); ++j) {
				RecordRenderPass(passes[j], i);
			}
			if (vkEndCommandBuffer(buf) != VK_SUCCESS) {
				LOGERROR("failed to record command buffer!");
			}
		}
		firstRecord = false;
	}

	void GPUProgramVulkan::RenderCmdUpdateUniformBufferObject(GFXCommandBuffer* cmdBuffer) {
		std::string name = "UniformBufferObject";
		//built in name
		uint32_t hashID = hashString(name.c_str());
		if (uniformVKs.find(hashID) == uniformVKs.end()) {
			uniformVKs[hashID] = new UniformBufferVK;
			uniformVKs[hashID]->size = sizeof(UniformBufferObject);
			uniformVKs[hashID]->data = new UniformBufferObject;
			uniformVKs[hashID]->id = hashID;

			CreateUniformBuffers(uniformVKs[hashID]);
		}

		BUILTIN_VALUE flag;
		cmdBuffer->ReadBuffer<BUILTIN_VALUE>(flag);
		switch (flag) {
			case BUILTIN_MATRIX_VIEW: {
				UniformBufferObject* ubo = (UniformBufferObject*)uniformVKs[hashID]->data;
				cmdBuffer->ReadBuffer<Matrix4x4f>(ubo->view);
				break; 
			}
			case BUILTIN_MATRIX_PROJECTION: {
				UniformBufferObject* ubo = (UniformBufferObject*)uniformVKs[hashID]->data;
				cmdBuffer->ReadBuffer<Matrix4x4f>(ubo->proj);
				break; 
			}
			case BUILTIN_VEC3_CAMERAPOS: {
				UniformBufferObject* ubo = (UniformBufferObject*)uniformVKs[hashID]->data;
				Vector3f v3;
				cmdBuffer->ReadBuffer<Vector3f>(v3);
				ubo->cameraPos.Set(v3.x, v3.y, v3.z, 0.f);
				break;
			}
			case BUILTIN_VEC3_SUNDIRECTION: {
				UniformBufferObject* ubo = (UniformBufferObject*)uniformVKs[hashID]->data;
				Vector3f v3;
				cmdBuffer->ReadBuffer<Vector3f>(v3);
				ubo->sunDirection.Set(v3.x, v3.y, v3.z, 1.f);
				break;
			}
			case BUILTIN_VEC3_SUNCOLOR: {
				UniformBufferObject* ubo = (UniformBufferObject*)uniformVKs[hashID]->data;
				Vector3f v3;
				cmdBuffer->ReadBuffer<Vector3f>(v3);
				ubo->sunColor.Set(v3.x, v3.y, v3.z, 1.f);
				break;
			}
			case BUILTIN_STRUCT_LIGHTBLOCK: {
				LightBlocks lb;
				cmdBuffer->ReadBuffer<LightBlocks>(lb);
				std::string name = "LightBlocks";
				//built in name
				uint32_t hashID = hashString(name.c_str());
				if (uniformVKs.find(hashID) == uniformVKs.end()) {
					uniformVKs[hashID] = new UniformBufferVK;
					uniformVKs[hashID]->size = sizeof(LightBlocks);
					uniformVKs[hashID]->data = JOJO_NEW(LightBlocks, MEMORY_GFX_STRUCT);
					uniformVKs[hashID]->id = hashID;

					CreateUniformBuffers(uniformVKs[hashID]);
				}
				memcpy(uniformVKs[hashID]->data, &lb, sizeof(LightBlocks));
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

	void GPUProgramVulkan::RenderCmdUpdateUniformBuffer(GFXCommandBuffer* cmdBuffer, DrawCallVK* dc) {
		BUILTIN_VALUE flag;
		cmdBuffer->ReadBuffer<BUILTIN_VALUE>(flag);
		U32 size;
		switch (flag) {
		case BUILTIN_MATRIX_MODEL: {
			//built in name
			size = sizeof(PushConsts);
			if (!dc->shader) {
				LOGERROR("can't update uniform buffer without use shader!!!");
				return;
			}
			PushConstsVK* pc = dc->shader->pushConst;
			if (pc->size == 0) {
				pc->data = JOJO_NEW(U8[size], MEMORY_GFX_STRUCT);
				pc->size = size;
				pc->def = dc->shader->GetPushConstsDef();
			}
			cmdBuffer->ReadBufferPtr(pc->data, size);
			break; 
		}
		default: break;
		}
	}

	void GPUProgramVulkan::CreateCommandBuffers() {
		vkCtxPtr->commandBuffers.resize(vkCtxPtr->swapChainImageViews.size());
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = vkCtxPtr->commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)vkCtxPtr->commandBuffers.size();

		VK_CHECK(vkAllocateCommandBuffers(vkCtxPtr->device, &allocInfo, vkCtxPtr->commandBuffers.data()))
	}


#define ENUM_STR(e) #e
#define CHECK_PASS(CMD) if(nullptr == pass) {LOGERROR("PLEASE START PASS FIRST! CMD = %s\n", #CMD);}
#define CHECK_PASS_OR_COMPUTE() if(nullptr == pass && nullptr == computePipeline) {LOGERROR("PLEASE START PASS OR COMPUTE FIRST!\n");}
	bool GPUProgramVulkan::ExecuteRenderCommand(GFXCommandBuffer* cmdBuffer, U16 imgIdx) {
		renderPassList.clear();
		curImageIdx = imgIdx;
		bool isCompute = false;

		bool ret = !lastRenderPassList.empty();

		RenderPassVK* pass = nullptr;
		DrawCallVK* drawcall = JOJO_NEW(DrawCallVK);
		RenderCommandType typ;
		while(cmdBuffer->ReadBuffer<RenderCommandType>(typ)) {
			drawcall->HashInsert(typ);
			switch (typ) {
			case RenderCMD_BeginRenderPass: {
				pass = new RenderPassVK;
				cmdBuffer->ReadBuffer<std::string>(pass->name);
				break;
			}
			case RenderCMD_EndRenderPass: {
				CHECK_PASS(RenderCMD_EndRenderPass)
				for (auto& dc : pass->dcs) {
					pass->HashInsert(dc->hash);
				}
				renderPassList.push_back(pass);
				break;
			}
			case RenderCMD_Clear: {
				CHECK_PASS(RenderCMD_Clear)
				pass->clear = true;
				cmdBuffer->ReadBuffer<Vector4f>(pass->clearColor);
				break; 
			}
			case RenderCMD_UpdateUniformBufferObject: {
				RenderCmdUpdateUniformBufferObject(cmdBuffer);
				UpdateUniformBuffer(imgIdx);
				break; 
			}
			case RenderCMD_UpdateUniformBuffer: {
				RenderCmdUpdateUniformBuffer(cmdBuffer, drawcall);
				break; 
			}
			case RenderCMD_UpdateVertexBuffer: {
				CHECK_PASS(RenderCMD_UpdateVertexBuffer)
				VertexBuffer* vb;
				cmdBuffer->ReadBuffer<VertexBuffer*>(vb);
				if (vbs.find(vb->id) == vbs.end()) {
					vbs[vb->id] = new VertexBufferVK(vb, vkCtxPtr);
					CreateVertexBuffer(vbs[vb->id]);
				}
				drawcall->vbs[0] = vbs[vb->id];
				drawcall->HashInsert(vb->id);
				break;
			}
			case RenderCMD_UpdateIndexBuffer: {
				CHECK_PASS(RenderCMD_UpdateIndexBuffer)
				IndexBuffer* ib;
				cmdBuffer->ReadBuffer<IndexBuffer*>(ib);
				if (ibs.find(ib->id) == ibs.end()) {
					ibs[ib->id] = new IndexBufferVK(ib, vkCtxPtr);
					CreateIndexBuffer(ibs[ib->id]);
				}
				drawcall->ib = ibs[ib->id];
				drawcall->HashInsert(ib->id);
				break;
			}
			case RenderCMD_UpdateInstanceBuffer: {
				CHECK_PASS(RenderCMD_UpdateInstanceBuffer)
				InstanceBuffer* ib;
				cmdBuffer->ReadBuffer<InstanceBuffer*>(ib);
				if (vbs.find(ib->id) == vbs.end()) {
					vbs[ib->id] = new VertexBufferVK(ib, vkCtxPtr);
					CreateVertexBuffer(vbs[ib->id]);
				}
				if (drawcall->vbs.size() < 2)
					drawcall->vbs.resize(2);
				drawcall->vbs[1] = vbs[ib->id];
				drawcall->HashInsert(ib->id);
				break;
			}
			case RenderCMD_UseShader: {
				CHECK_PASS(RenderCMD_UseShader)
				Shader* shader;
				cmdBuffer->ReadBuffer<Shader*>(shader);
				if (shaderVKs.find(shader->id) == shaderVKs.end()) {
					shaderVKs[shader->id] = JOJO_NEW(ShaderVK(shader, vkCtxPtr), MEMORY_GFX_STRUCT);
				}

				drawcall->shader = shaderVKs[shader->id];
				drawcall->shader->Reset();
				drawcall->HashInsert(shader->id);
				break;
			}
			case RenderCMD_UpdateTexture: {
				CHECK_PASS(RenderCMD_UpdateTexture)
				Texture* tex;
				cmdBuffer->ReadBuffer<Texture*>(tex);
				U8 binding;
				cmdBuffer->ReadBuffer<U8>(binding);
				CMDUpdateTexture(tex, drawcall->shader, binding);
				drawcall->HashInsert(tex->id);
				break;
			}
			case RenderCMD_SetDepthCompare: {
				CHECK_PASS(RenderCMD_SetDepthCompare)
				DepthCompareFunc f;
				cmdBuffer->ReadBuffer<DepthCompareFunc>(f);
				drawcall->depthOp = VKCompareOps[f];
				drawcall->HashInsert(f);
				break;
			}
			case RenderCMD_SetPolygonMode: {
				CHECK_PASS(RenderCMD_SetPolygonMode)
				PolygonMode f;
				cmdBuffer->ReadBuffer<PolygonMode>(f);
				drawcall->polygonMode = VKPolygonModes[f];
				drawcall->HashInsert(f);
				break;
			}
			case RenderCMD_DrawIndexed: {
				CHECK_PASS(RenderCMD_DrawIndexed)
				cmdBuffer->ReadBuffer<U32>(drawcall->instanceCount);
				cmdBuffer->ReadBuffer<MeshTopology>(drawcall->topology);
				drawcall->HashInsert(drawcall->instanceCount);
				drawcall->HashInsert(drawcall->topology);
				pass->dcs.push_back(drawcall);

				drawcall = new DrawCallVK;
				break;
			}
			case RenderCMD_Draw: {
				CHECK_PASS(RenderCMD_Draw)
				cmdBuffer->ReadBuffer<U32>(drawcall->instanceCount);
				cmdBuffer->ReadBuffer<MeshTopology>(drawcall->topology);
				drawcall->HashInsert(drawcall->instanceCount);
				drawcall->HashInsert(drawcall->topology);
				pass->dcs.push_back(drawcall);

				drawcall = new DrawCallVK;
				break;
			}
			default: break;
			}
		}


		bool needRecord = false, needPushConstant = false;
		if (lastRenderPassList.size() != renderPassList.size()) {
			needRecord = true;
			//needPushConstant = true;
		} else {
			for (int i = 0; i < lastRenderPassList.size(); ++i) {
				if (lastRenderPassList[i]->hash != renderPassList[i]->hash) {
					needRecord = true;
					break;
				}
			}
		}

		if (needRecord) {
			//CreateRenderPass
			for (auto& pass : renderPassList) {
				CreateRenderPass(pass);
				for (int i = 0; i < pass->dcs.size(); ++i) {
					//assign def for every ubvk
					for (auto& ub : pass->dcs[i]->shader->ubs) {
						UniformBufferVK* ubvk = uniformVKs[ub.id];
						ubvk->def = ub;
					}
					GetPipeline(pass, i);
				}
			}
			//if (needPushConstant)
			//	PushConstants(renderPassList);
			
			RecordCommandBuffer(renderPassList);
			lastRenderPassList.clear();
			lastRenderPassList.swap(renderPassList);
		} else {
			//if (needPushConstant)
			//	PushConstants(lastRenderPassList);
			RecordCommandBuffer(lastRenderPassList);
		}

		delete drawcall;

		return ret;
	}

	bool GPUProgramVulkan::ExecuteComputeCommand(GFXCommandBuffer* cmdBuffer) {
		if (!computeCtx) computeCtx = JOJO_NEW(ComputeContextVK(vkCtxPtr), MEMORY_GFX_STRUCT);
		ComputePipelineVK* computePipeline = nullptr;
		ComputeCommandType typ;
		while (cmdBuffer->ReadBuffer<ComputeCommandType>(typ)) {
			if (computePipeline != nullptr) computePipeline->HashInsert(typ);
			switch (typ) {
			case ComputeCMD_BeginCompute: {
				hasCompute = true;
				computePipeline = new ComputePipelineVK{ computeCtx };
				break;
			}
			case ComputeCMD_EndCompute: {
				break;
			}
			case ComputeCMD_DispatchCompute: {
				U32 sz = 3 * sizeof(U32);
				cmdBuffer->ReadBufferPtr(computePipeline->group, sz);
				//memcpy(computePipeline->group, cmdBuffer[i].data, cmdBuffer[i].size);
				PrepareCompute(computePipeline);
				DispatchCompute(computePipeline);
				break;
			}
			case ComputeCMD_UpdateComputeBuffer: {
				ComputeBuffer* cb;
				cmdBuffer->ReadBuffer<ComputeBuffer*>(cb);
				U8 binding;
				cmdBuffer->ReadBuffer<U8>(binding);
				if (uniformVKs.find(cb->id) == uniformVKs.end()) {
					uniformVKs[cb->id] = JOJO_NEW(ComputeBufferVK(cb), MEMORY_GFX_STRUCT);
					uniformVKs[cb->id]->id = cb->id;
					uniformVKs[cb->id]->size = cb->GetSize();
					CreateComputeBuffers(static_cast<ComputeBufferVK*>(uniformVKs[cb->id]));
				}
				if (computePipeline->computeBuffers.size() < binding + 1) {
					computePipeline->computeBuffers.resize(binding + 1);
				}
				
				computePipeline->computeBuffers[binding] = uniformVKs[cb->id];
				computePipeline->HashInsert(cb->id);
				break;
			}
			case ComputeCMD_WriteBackComputeBuffer: {
				computePipeline->writeBack = true;
				break;
			}
			case ComputeCMD_UseShader: {
				Shader* shader;
				cmdBuffer->ReadBuffer<Shader*>(shader);
				if (shaderVKs.find(shader->id) == shaderVKs.end()) {
					shaderVKs[shader->id] = JOJO_NEW(ShaderVK(shader, vkCtxPtr), MEMORY_GFX_STRUCT);
				}

				computePipeline->shader = shaderVKs[shader->id];
				computePipeline->shader->Reset();
				computePipeline->HashInsert(shader->id);
				break;
			}
			case ComputeCMD_UpdatePushConstant: {
				U32 size;
				cmdBuffer->ReadBuffer<U32>(size);
				if (computePipeline->shader->pushConst->size == 0) {
					computePipeline->shader->pushConst->data = JOJO_NEW(U8(size), MEMORY_GFX_STRUCT);
					computePipeline->shader->pushConst->size = size;
					computePipeline->shader->pushConst->def = computePipeline->shader->GetPushConstsDef(); 
				}
				cmdBuffer->ReadBufferPtr(computePipeline->shader->pushConst->data, size);
				computePipeline->HashInsert(size);
				break;
			}
			case ComputeCMD_UpdateTexture: {
				Texture* tex;
				cmdBuffer->ReadBuffer<Texture*>(tex);
				U8 binding;
				cmdBuffer->ReadBuffer<U8>(binding);
				CMDUpdateTexture(tex, computePipeline->shader, binding);
				computePipeline->HashInsert(tex->id);
				break;
			}
			default: break;
			}
		}
		return true;
	}

	void GPUProgramVulkan::PrepareCompute(ComputePipelineVK* compute) {
		if (computePipelines.find(compute->hash) == computePipelines.end()) {
			compute->shader->Prepare();
			//resolve uniform
			for (auto& ub : compute->shader->ubs) {
				UniformBufferVK* ubvk = uniformVKs[ub.id];
				ubvk->def = ub;
			}

			CreateDescriptorSetLayout<ComputePipelineVK>(compute);
			CreateDescriptorPool<ComputePipelineVK>(compute);
			CreateDescriptorSets<ComputePipelineVK>(compute);

			CreatePipelineLayout<ComputePipelineVK>(compute);
			compute->CreatePipeline();

			computePipelines[compute->hash] = compute;
		}
	}

	void GPUProgramVulkan::DispatchCompute(ComputePipelineVK* compute) {
		compute = computePipelines[compute->hash];
		int nextIdx = curImageIdx == vkCtxPtr->swapChainImages.size() - 1 ? 0 : curImageIdx + 1;
		compute->Record(nextIdx);
	}

	void GPUProgramVulkan::CMDUpdateTexture(Texture* tex, ShaderVK* shader, U16 binding) {
		//check if uniform buffer is ready
		if (textureVKs.find(tex->id) == textureVKs.end() && pendingTextureVKs.find(tex->id) == pendingTextureVKs.end()) {
			TextureVK* vkTex = JOJO_NEW(TextureVK(tex), MEMORY_GFX_STRUCT);
			pendingTextureVKs[vkTex->ID()] = vkTex;
		}
		shader->textures.push_back(tex->id);

		if (uniformVKs.find(tex->id) == uniformVKs.end()) {
			UniformBufferVK* texUB = JOJO_NEW(UniformBufferVK, MEMORY_GFX_STRUCT);
			texUB->texID = tex->id;
			texUB->id = tex->id;
			uniformVKs[tex->id] = texUB;
		}
		//update binding tex
		shader->ubs[binding].id = tex->id;
	}

	void GPUProgramVulkan::CleanupSwapChain() {
		for (auto& fb : fbs) {
			//delete fb;
			//fb.second->Clean(vkCtxPtr->device);
		}
	}
}
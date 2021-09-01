#include "GPUProgramVulkan.h"
#include "../IO/Log.h"
#include "../Misc/Application.h"
#include <array>
#include <direct.h>
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

	GPUProgramVulkan::GPUProgramVulkan() : dynamicCommandBuffer(false), firstRecord(true) {
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

	void GPUProgramVulkan::CreateTextureImage(TextureVK* tex) {
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
		tex->image->TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, cb);
		tex->image->CopyBufferToImage(stagingBuffer, cb);
		if (tex->HasMipmap()) {
			tex->image->GenerateMipmaps(cb);
		} else {
			tex->image->TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, cb);
		}
		tex->image->CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT, cb);
		cb.End();
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
			if (vkCreatePipelineLayout(vkCtxPtr->device, &pipelineLayoutInfo, nullptr, &(pso->pipelineLayout)) != VK_SUCCESS) {
				LOGERROR("failed to create pipeline layout!");
			}
		} else {
			if (vkCreatePipelineLayout(vkCtxPtr->device, &pipelineLayoutInfo, nullptr, &(pso->pipelineLayout)) != VK_SUCCESS) {
				LOGERROR("failed to create pipeline layout!");
			}
		}


		//Create Vertex Buffer
		VkPipelineVertexInputStateCreateInfo* vertexInputInfo = dc->vbs[0]->GetVertexInputInfo();
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

	void GPUProgramVulkan::CreateDescriptorSetLayout(DrawCallVK* dc) {
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		bindings.reserve(dc->shader->ubs.size());
		for (int i = 0; i < dc->shader->ubs.size(); ++i) {
			UniformBufferVK* ubvk = uniformVKs[dc->shader->ubs[i]];
			VkDescriptorSetLayoutBinding layoutBinding{};
			if (ubvk->texID > 0) {
				layoutBinding.binding = i;
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				//need to know shader stage in shader parser, temp write, --todo
				layoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			} else {
				layoutBinding.binding = dc->shader->GetUniformBindingByHash(dc->shader->ubs[i]);
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				//need to know shader stage in shader parser, temp write, --todo
				layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			}
			layoutBinding.descriptorCount = 1;
			layoutBinding.pImmutableSamplers = nullptr;

			bindings.push_back(layoutBinding);
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		PipelineStateVK* pso = dc->pso;
		if (vkCreateDescriptorSetLayout(vkCtxPtr->device, &layoutInfo, nullptr, &(pso->descriptorSetLayout)) != VK_SUCCESS) {
			LOGERROR("failed to create descriptor set layout!");
		}
	}

	void GPUProgramVulkan::GetPipeline(RenderPassVK* pass, int i) {
		CreateDescriptorSetLayout(pass->dcs[i]);
		CreateGraphicsPipeline(pass, i);
		CreateTextureSampler(pass, i);
		CreateDescriptorPool();
		CreateDescriptorSets(pass->dcs[i]);
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
		pass->fb->depthImage->TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, cb);
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

		dc->descriptorSets.resize(vkCtxPtr->swapChainImages.size());
		if (vkAllocateDescriptorSets(vkCtxPtr->device, &allocInfo, dc->descriptorSets.data()) != VK_SUCCESS) {
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
						textureVKs[it->first] = it->second;
						tex = it->second;
						pendingTextureVKs.erase(it);
					} else {
						tex = it->second;
					}
					imageInfo.imageView = tex->image->imageView;
					imageInfo.sampler = dc->pso->textureSampler;

					descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[j].dstSet = dc->descriptorSets[i];
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
					descriptorWrites[j].dstSet = dc->descriptorSets[i];
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
			vkCmdBindPipeline(vkCtxPtr->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, dc->pso->graphicsPipeline);

			VkBuffer vertexBuffer = dc->vbs[0]->buffer.buffer;
			VkBuffer vertexBuffers[] = { vertexBuffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(vkCtxPtr->commandBuffers[i], 0, 1, vertexBuffers, offsets);

			vkCmdBindDescriptorSets(vkCtxPtr->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, dc->pso->pipelineLayout, 0, 1, &dc->descriptorSets[i], 0, nullptr);

			if (dc->pc) {
				vkCmdPushConstants(vkCtxPtr->commandBuffers[i], dc->pso->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConsts), dc->pc);
			}

			if (dc->ib) {
				vkCmdBindIndexBuffer(vkCtxPtr->commandBuffers[i], dc->ib->buffer.buffer, 0, VK_INDEX_TYPE_UINT16);
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

	void GPUProgramVulkan::PushConstants(std::vector<RenderPassVK*>& passes) {
		for (auto& pass : passes) {
			//for (auto& dc : pass->dcs) {
			//	if (dc->pc) {
			//		vkCmdPushConstants(cb->commandBuffer, dc->pso->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConsts), dc->pc);
			//	}
			//}
			//for (int j = pass->dcs.size() - 1; j >= 0; --j) {
			//	if (pass->dcs[j]->pc) {
			//		CommandBufferVK* cb = GetCommandBuffer(true);
			//		cb->Begin();
			//		vkCmdPushConstants(cb->commandBuffer, pass->dcs[j]->pso->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConsts), pass->dcs[j]->pc);
			//		cb->End();
			//	}
			//}
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

	void GPUProgramVulkan::CreateCommandBuffers() {
		vkCtxPtr->commandBuffers.resize(vkCtxPtr->swapChainImageViews.size());
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = vkCtxPtr->commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)vkCtxPtr->commandBuffers.size();

		VK_CHECK(vkAllocateCommandBuffers(vkCtxPtr->device, &allocInfo, vkCtxPtr->commandBuffers.data()))
	}


#define CHECK_PASS() if(pass == nullptr) {LOGERROR("PLEASE START PASS FIRST!\n");}
	bool GPUProgramVulkan::ExecuteRenderCommand(std::vector<RenderCommand>& cmdBuffer, uint16_t cmdIdx, U16 imgIdx) {
		if (cmdIdx == 0) return false;
		renderPassList.clear();
		curImageIdx = imgIdx;

		bool ret = !lastRenderPassList.empty();

		RenderPassVK* pass = nullptr;
		DrawCallVK* drawcall = new DrawCallVK;
		drawcall->pso = new PipelineStateVK;
		for (int i = 0; i < cmdIdx; ++i) {
			drawcall->HashInsert(cmdBuffer[i].typ);
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
					vbs[vb->id] = new VertexBufferVK(vb, vkCtxPtr);
					CreateVertexBuffer(vbs[vb->id]);
				}
				drawcall->vbs[0] = vbs[vb->id];
				drawcall->HashInsert(vb->id);
				break;
			}
			case RenderCMD_UpdateIndexBuffer: {
				CHECK_PASS()
				IndexBuffer* ib = (IndexBuffer*)cmdBuffer[i].data;
				if (ibs.find(ib->id) == ibs.end()) {
					ibs[ib->id] = new IndexBufferVK(ib, vkCtxPtr);
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
			case RenderCMD_SetDepthCompare: {
				CHECK_PASS()
				drawcall->depthOp = VKCompareOps[cmdBuffer[i].flag];
				drawcall->HashInsert(cmdBuffer[i].flag);
				break;
			}
			case RenderCMD_SetPolygonMode: {
				CHECK_PASS()
				drawcall->polygonMode = VKPolygonModes[cmdBuffer[i].flag];
				drawcall->HashInsert(cmdBuffer[i].flag);
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
			case RenderCMD_Draw: {
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


		bool needRecord = false, needPushConstant = false;
		if (lastRenderPassList.size() != renderPassList.size()) {
			needRecord = true;
			//needPushConstant = true;
		} else {
			for (int i = 0; i < lastRenderPassList.size(); ++i) {
				if (lastRenderPassList[i]->hash != renderPassList[i]->hash) {
					needRecord = true;
					//needPushConstant = true;
					break;
				} else {
					//update push consts
					for (int j = 0; j < renderPassList[i]->dcs.size(); ++j) {
						if (renderPassList[i]->dcs[j]->pc) {
							if (*(lastRenderPassList[i]->dcs[j]->pc) != *(renderPassList[i]->dcs[j]->pc)) {
								//needPushConstant = true;
								*(lastRenderPassList[i]->dcs[j]->pc) = *(renderPassList[i]->dcs[j]->pc);
							}
						}
					}
				}
			}
		}

		if (needRecord) {
			//CreateRenderPass
			for (auto& pass : renderPassList) {
				CreateRenderPass(pass);
				for (int i = 0; i < pass->dcs.size(); ++i) {
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

		delete drawcall->pso;
		delete drawcall;

		return ret;
	}

	void GPUProgramVulkan::CleanupSwapChain() {
		for (auto& fb : fbs) {
			//delete fb;
			//fb.second->Clean(vkCtxPtr->device);
		}
	}
}
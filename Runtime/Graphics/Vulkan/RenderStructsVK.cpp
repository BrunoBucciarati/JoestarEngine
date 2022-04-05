#include "RenderStructsVK.h"
#include "../../IO/File.h"

namespace Joestar {
    static VkResult globalResult;
#define VK_CHECK(fn) \
    globalResult = fn; \
    if (globalResult != VK_SUCCESS) \
    { \
        LOGERROR("[VK ERROR=%d] %s\n", globalResult, #fn); \
    }
    U32 FindMemoryType(U32 typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice& device) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(device, &memProperties);
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        LOGERROR("failed to find suitable memory type!");
    }

    void BufferVK::CopyBuffer(VkCommandBuffer cb)
    {
        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0; // Optional
        copyRegion.dstOffset = 0; // Optional
        copyRegion.size = size;
        vkCmdCopyBuffer(cb, stagingBuffer->GetBuffer(), GetBuffer(), 1, &copyRegion);
    }

    void BufferVK::CreateStagingBuffer(U32 size, U8* data)
    {
        stagingBuffer = JOJO_NEW(StagingBufferVK, MEMORY_GFX_STRUCT);
        stagingBuffer->count = count;
        stagingBuffer->SetDevice(device, physicalDevice);
        stagingBuffer->SetSize(size);
        stagingBuffer->Create(data);
    }

    void BufferVK::SetFrame(U32 idx)
    {
        index = idx;
        if (stagingBuffer)
        {
            stagingBuffer->SetFrame(idx);
        }
    }

    void BufferVK::UpdateStagingBuffer(U32 size, U8* data)
    {
        stagingBuffer->SetData(data);
    }

    void GraphicsPipelineStateVK::CreateIAState()
    {
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;
    }

    void GraphicsPipelineStateVK::CreateViewportState(Viewport& vp)
    {
        viewport.x = vp.rect.x;
        viewport.y = vp.rect.y;
        viewport.width = vp.rect.width;
        viewport.height = vp.rect.height;
        viewport.minDepth = vp.minDepth;
        viewport.maxDepth = vp.maxDepth;

        scissor.offset = { (I32)vp.scissor.x, (I32)vp.scissor.y };
        scissor.extent.width = (U32)vp.scissor.width;
        scissor.extent.height = (U32)vp.scissor.height;

        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;
    }

    void GraphicsPipelineStateVK::CreateRasterizationState(GPURasterizationStateCreateInfo& createInfo)
    {
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = createInfo.depthClamp;
        rasterizer.rasterizerDiscardEnable = createInfo.discardEnable;
        rasterizer.polygonMode = (VkPolygonMode)createInfo.polygonMode;
        rasterizer.lineWidth = createInfo.lineWidth;
        rasterizer.cullMode = (VkCullModeFlags)createInfo.cullMode;
        rasterizer.frontFace = (VkFrontFace)createInfo.frontFace;
        rasterizer.depthBiasEnable = createInfo.depthBias;
        if (createInfo.depthBias)
        {
            rasterizer.depthBiasConstantFactor = createInfo.depthBiasConstantFactor; // Optional
            rasterizer.depthBiasClamp = createInfo.depthBiasClamp; // Optional
            rasterizer.depthBiasSlopeFactor = createInfo.depthBiasSlopeFactor; // Optional
        }
    }

    void GraphicsPipelineStateVK::CreateMultiSampleState(GPUMultiSampleStateCreateInfo& createInfo)
    {
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = createInfo.sampleShading;
        multisampling.rasterizationSamples = (VkSampleCountFlagBits)createInfo.rasterizationSamples;
        multisampling.minSampleShading = createInfo.minSampleShading; // Optional
        multisampling.pSampleMask = nullptr; // Optional
        multisampling.alphaToCoverageEnable = createInfo.alphaToCoverage; // Optional
        multisampling.alphaToOneEnable = createInfo.alphaToOne; // Optional
    }

    void GraphicsPipelineStateVK::CreateDepthStencilState(GPUDepthStencilStateCreateInfo& createInfo)
    {
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = createInfo.depthTest;
        depthStencil.depthWriteEnable = createInfo.depthWrite;
        depthStencil.depthCompareOp = (VkCompareOp)createInfo.depthCompareOp;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = createInfo.stencilTest;
    }

    void PipelineLayoutVK::Create(VkDevice& device, PODVector<VkDescriptorSetLayout>& setLayouts)
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = setLayouts.Size(); // Optional
        pipelineLayoutInfo.pSetLayouts = setLayouts.Buffer(); // Optional

        if (false)
        {
            //PushConstant
            //VkPushConstantRange pushConstantRange{};
            //pushConstantRange.stageFlags = pushConsts->GetStageFlags();
            //pushConstantRange.offset = 0;
            //pushConstantRange.size = pushConsts->size;
            //pipelineLayoutInfo.pushConstantRangeCount = 1; // Optional
            //pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; // Optional
            //VK_CHECK(vkCreatePipelineLayout(vkCtxPtr->device, &pipelineLayoutInfo, nullptr, &(call->pipelineLayout)))
        }
        else {
            VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &layout));
        }
    }

    void GraphicsPipelineStateVK::SetPipelineLayout(VkPipelineLayout layout)
    {
        pipelineLayout = layout;
    }

    void GraphicsPipelineStateVK::CreateShaderStages(PODVector<ShaderVK*>& shaders)
    {
        shaderStageInfos.Resize(shaders.Size());
        for (U32 i = 0; i < shaders.Size(); ++i)
        {
            shaderStageInfos[i] = {};
            VkPipelineShaderStageCreateInfo& createInfo = shaderStageInfos[i];
            createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            createInfo.stage = shaders[i]->flagBits;
            createInfo.module = shaders[i]->shaderModule;
            createInfo.pName = shaders[i]->entryName.CString();
        }
    }

    void GraphicsPipelineStateVK::CreateColorBlendState(GPUColorBlendStateCreateInfo& createInfo)
    {
        colorBlendAttachments.Resize(createInfo.numAttachments);
        for (U32 i = 0; i < createInfo.numAttachments; ++i)
        {
            auto& attachmentInfo = createInfo.attachments[i];
            VkPipelineColorBlendAttachmentState& colorBlendAttachment = colorBlendAttachments[i];
            colorBlendAttachment = {};
            colorBlendAttachment.colorWriteMask = VkColorComponentFlags(attachmentInfo.writeMask);
            colorBlendAttachment.blendEnable = attachmentInfo.blendEnable;
            colorBlendAttachment.srcColorBlendFactor = VkBlendFactor(attachmentInfo.srcColorBlendFactor); // Optional
            colorBlendAttachment.dstColorBlendFactor = VkBlendFactor(attachmentInfo.dstColorBlendFactor); // Optional
            colorBlendAttachment.colorBlendOp = VkBlendOp(attachmentInfo.colorBlendOp); // Optional
            colorBlendAttachment.srcAlphaBlendFactor = VkBlendFactor(attachmentInfo.srcAlphaBlendFactor); // Optional
            colorBlendAttachment.dstAlphaBlendFactor = VkBlendFactor(attachmentInfo.dstAlphaBlendFactor); // Optional
            colorBlendAttachment.alphaBlendOp = VkBlendOp(attachmentInfo.alphaBlendOp); // Optional
        }

        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = createInfo.numAttachments;
        colorBlending.pAttachments = colorBlendAttachments.Buffer();
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional
    }

    void GraphicsPipelineStateVK::CreateVertexInputInfo(PODVector<InputBinding>& bindings, PODVector<InputAttribute>& attributes)
    {
        vertexInputBindings.Resize(bindings.Size());
        for (U32 i = 0; i < bindings.Size(); ++i)
        {
            vertexInputBindings[i] = {};
            vertexInputBindings[i].binding = bindings[i].binding;
            vertexInputBindings[i].stride = bindings[i].stride;
            vertexInputBindings[i].inputRate = bindings[i].instance ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
        }
        vertexInputAttributes.Resize(attributes.Size());
        for (U32 i = 0; i < attributes.Size(); ++i)
        {
            vertexInputAttributes[i].binding = attributes[i].binding;
            vertexInputAttributes[i].format = GetInputFormatVK(attributes[i].format);
            vertexInputAttributes[i].location = attributes[i].location;
            vertexInputAttributes[i].offset = attributes[i].offset;
        }
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = vertexInputBindings.Size();
        vertexInputInfo.vertexAttributeDescriptionCount = vertexInputAttributes.Size();
        vertexInputInfo.pVertexBindingDescriptions = vertexInputBindings.Buffer();
        vertexInputInfo.pVertexAttributeDescriptions = vertexInputAttributes.Buffer();
    }

    void GraphicsPipelineStateVK::Create(VkDevice& device)
    {
        VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_LINE_WIDTH
        };

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = 2;
        dynamicState.pDynamicStates = dynamicStates;

        //CreatePipelineLayout<DrawCallVK>(dc);

        //Create Vertex Buffer
        //VkPipelineVertexInputStateCreateInfo& vertexInputInfo = dc->GetVertexInputInfo();
        //VkPipelineShaderStageCreateInfo* shaderCreateInfo = dc->shader->shaderStage.Buffer();
        
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = shaderStageInfos.Size();
        pipelineInfo.pStages = shaderStageInfos.Buffer();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = nullptr; // Optional

        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;

        //pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        ////pipelineInfo.basePipelineIndex = -1; // Optional
        VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline));
    }

    void ShaderVK::Create(VkDevice& device, GPUShaderCreateInfo& shader)
    {
        File* file = (File*)shader.file;
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        U32 codeSize = file->Size();
        createInfo.codeSize = codeSize;
        createInfo.pCode = reinterpret_cast<const U32*>(file->GetBuffer());

        flagBits = GetShaderStageFlagBitsVK(shader.stage);

        VK_CHECK(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule))
    }

    void VertexBufferVK::Create(U32 sz, U32 ct, PODVector<VertexElement>& elements)
    {
        count = ct;
        size = sz * ct;
        bindingDescription.binding = binding;
        bindingDescription.stride = sz;
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        SetVertexElements(elements);
        CreateBuffer();
    }

    void VertexBufferVK::SetVertexElements(PODVector<VertexElement>& elements)
    {
        attributeDescriptions.Resize(elements.Size());

        U32 offset = 0;
        for (U32 i = 0; i < elements.Size(); ++i)
        {
            attributeDescriptions[i] = {};
            attributeDescriptions[i].binding = binding;
            attributeDescriptions[i].location = i;
            attributeDescriptions[i].offset = offset;
            attributeDescriptions[i].format = GetInputFormatVK(elements[i].type);
            offset += elements[i].GetSize();
        }

        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        createInfo.vertexBindingDescriptionCount = 1;
        createInfo.vertexAttributeDescriptionCount = attributeDescriptions.Size();
        createInfo.pVertexBindingDescriptions = &bindingDescription;
        createInfo.pVertexAttributeDescriptions = attributeDescriptions.Buffer();
    }


    void DescriptorSetLayoutVK::Create(VkDevice& device, PODVector<GPUDescriptorSetLayoutBinding>& bindings)
    {
        Vector<VkDescriptorSetLayoutBinding> layoutBindings;
        layoutBindings.Resize(bindings.Size());
        for (U32 i = 0; i < bindings.Size(); ++i) {
            VkDescriptorSetLayoutBinding& layoutBinding = layoutBindings[i];
            layoutBinding = {};
            layoutBinding.binding = bindings[i].binding;            
            layoutBinding.descriptorType = GetDescriptorTypeVK(bindings[i].type);
            layoutBinding.stageFlags = GetShaderStageFlagsVK(bindings[i].stage);
            layoutBinding.descriptorCount = 1;
            layoutBinding.pImmutableSamplers = nullptr;
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = layoutBindings.Size();
        layoutInfo.pBindings = layoutBindings.Buffer();

        VK_CHECK(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &setLayout));
    }

    void CommandBufferVK::Create(VkDevice& inDevice, VkCommandBufferAllocateInfo& allocInfo, U32 num)
    {
        if (bCreated)
            return;
        device = inDevice;
        pool = allocInfo.commandPool;
        commandBuffers.Resize(num);
        for (U32 i = 0; i < num; ++i)
        {
            VK_CHECK(vkAllocateCommandBuffers(inDevice, &allocInfo, &commandBuffers[i]));
        }
        bCreated = true;
    }

    void DescriptorSetsVK::Create(VkDevice& device, VkDescriptorPool& pool, PODVector<VkDescriptorSetLayout>& layout)
    {
        sets.Resize(layout.Size());
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = pool;
        allocInfo.descriptorSetCount = layout.Size();
        allocInfo.pSetLayouts = layout.Buffer();

        VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, sets.Buffer()));
    }

    void SamplerVK::Create(VkDevice& device, GPUSamplerCreateInfo& createInfo)
    {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VkFilter(createInfo.magFilter);
        samplerInfo.minFilter = VkFilter(createInfo.minFilter);
        samplerInfo.mipmapMode = VkSamplerMipmapMode(createInfo.mipmapFilter);
        samplerInfo.addressModeU = VkSamplerAddressMode(createInfo.addressModeU);
        samplerInfo.addressModeV = VkSamplerAddressMode(createInfo.addressModeV);
        samplerInfo.addressModeW = VkSamplerAddressMode(createInfo.addressModeW);
        samplerInfo.anisotropyEnable = createInfo.bAnisotrophy;
        samplerInfo.maxAnisotropy = createInfo.maxAnisotrophy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = createInfo.bCompare;
        samplerInfo.compareOp = VkCompareOp(createInfo.compareOp);
        samplerInfo.minLod = createInfo.minLod;
        samplerInfo.maxLod = createInfo.maxLod;
        samplerInfo.mipLodBias = createInfo.mipLodBias;

        if (vkCreateSampler(device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
            LOGERROR("failed to create texture sampler!");
        }
    }


    void TextureVK::Create(ImageViewVK* imageView, SamplerVK* sampler)
    {
        imageInfo.imageLayout = imageView->GetImageLayout();
        imageInfo.imageView = imageView->GetImageView();
        imageInfo.sampler = sampler->GetSampler();;
    }
}

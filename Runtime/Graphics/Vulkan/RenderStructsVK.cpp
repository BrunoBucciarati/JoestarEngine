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


    void GraphicsPipelineStateVK::CreateIAState()
    {
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;
    }

    void GraphicsPipelineStateVK::CreateViewportState(Viewport& vp)
    {
        VkViewport viewport{};
        viewport.x = vp.rect.x;
        viewport.y = vp.rect.y;
        viewport.width = vp.rect.width;
        viewport.height = vp.rect.height;
        viewport.minDepth = vp.minDepth;
        viewport.maxDepth = vp.maxDepth;

        VkRect2D scissor{};
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

    void GraphicsPipelineStateVK::CreatePipelineLayout(VkDevice& device, Vector<DescriptorSetLayout>& inSetLayouts)
    {
        setLayouts.Resize(inSetLayouts.Size());

        for (U32 setIdx = 0; setIdx < inSetLayouts.Size(); ++setIdx)
        {
            Vector<VkDescriptorSetLayoutBinding> bindings;
            for (U32 i = 0; i < inSetLayouts[setIdx].GetNumBindings(); ++i)
            {
                DescriptorSetLayoutBinding& binding = inSetLayouts[setIdx].GetLayoutBindings(i);
                VkDescriptorSetLayoutBinding layoutBinding{};
                layoutBinding.binding = binding.binding;
                layoutBinding.descriptorType = VkDescriptorType(binding.type);
                layoutBinding.stageFlags = GetShaderStageFlagsVK(binding.stage);
                layoutBinding.descriptorCount = binding.count;
                layoutBinding.pImmutableSamplers = nullptr;
                bindings.Push(layoutBinding);
            }
            VkDescriptorSetLayoutCreateInfo layoutInfo{};
            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = static_cast<U32>(bindings.Size());
            layoutInfo.pBindings = bindings.Buffer();

            VK_CHECK(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &setLayouts[setIdx]));
        }
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
        } else {
            VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout));
        }
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

        Vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments{};
        colorBlendAttachments.Reserve(createInfo.numAttachments);
        for (auto& attachmentInfo : createInfo.attachments)
        {
            VkPipelineColorBlendAttachmentState colorBlendAttachment{};
            colorBlendAttachment.colorWriteMask = VkColorComponentFlags(attachmentInfo.writeMask);
            colorBlendAttachment.blendEnable = attachmentInfo.blendEnable;
            colorBlendAttachment.srcColorBlendFactor = VkBlendFactor(attachmentInfo.srcColorBlendFactor); // Optional
            colorBlendAttachment.dstColorBlendFactor = VkBlendFactor(attachmentInfo.dstColorBlendFactor); // Optional
            colorBlendAttachment.colorBlendOp = VkBlendOp(attachmentInfo.colorBlendOp); // Optional
            colorBlendAttachment.srcAlphaBlendFactor = VkBlendFactor(attachmentInfo.srcAlphaBlendFactor); // Optional
            colorBlendAttachment.dstAlphaBlendFactor = VkBlendFactor(attachmentInfo.dstAlphaBlendFactor); // Optional
            colorBlendAttachment.alphaBlendOp = VkBlendOp(attachmentInfo.alphaBlendOp); // Optional
        }

        VkPipelineColorBlendStateCreateInfo colorBlending{};
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

    void GraphicsPipelineStateVK::CreateVertexInputInfo(VertexBufferVK& vb)
    {
        vertexInputInfo = vb.GetInputStateCreateInfo();
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
        size_t codeSize = file->Size();
        createInfo.codeSize = codeSize;
        createInfo.pCode = reinterpret_cast<const U32*>(file->GetBuffer());

        flagBits = GetShaderStageFlagBitsVK(shader.stage);

        VK_CHECK(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule))
    }
}
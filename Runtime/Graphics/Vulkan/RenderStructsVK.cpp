#include "RenderStructsVK.h"

namespace Joestar {
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

    void GraphicsPipelineStateVK::CreatePipelineLayout()
    {
        //VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        //pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        //pipelineLayoutInfo.setLayoutCount = 1; // Optional
        //VkDescriptorSetLayout layouts[] = { call->descriptorSetLayout };
        //pipelineLayoutInfo.pSetLayouts = layouts; // Optional

        //PushConstsVK* pushConsts = call->pushConst;
        //if (pushConsts) {
        //    VkPushConstantRange pushConstantRange{};
        //    pushConstantRange.stageFlags = pushConsts->GetStageFlags();
        //    pushConstantRange.offset = 0;
        //    pushConstantRange.size = pushConsts->size;
        //    pipelineLayoutInfo.pushConstantRangeCount = 1; // Optional
        //    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; // Optional
        //    VK_CHECK(vkCreatePipelineLayout(vkCtxPtr->device, &pipelineLayoutInfo, nullptr, &(call->pipelineLayout)))
        //} else {
        //    VK_CHECK(vkCreatePipelineLayout(vkCtxPtr->device, &pipelineLayoutInfo, nullptr, &(call->pipelineLayout)))
        //}
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
}
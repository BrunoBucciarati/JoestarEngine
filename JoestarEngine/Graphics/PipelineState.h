#pragma once
#include "../Core/RefCount.h"
#include "../Container/Ptr.h"
namespace Joestar {
    class PipelineLayout : RefCount
    {

    };

    class PipelineShader : RefCount
    {

    };

	class PipelineState : RefCount
	{
    protected:
        SharedPtr<PipelineShader> mShader;
        SharedPtr<PipelineLayout> mLayout;
        U32 mIndex;
        U64 mFlags;
	};

    //typedef struct VkComputePipelineCreateInfo {
    //    VkStructureType                    sType;
    //    const void* pNext;
    //    VkPipelineCreateFlags              flags;
    //    VkPipelineShaderStageCreateInfo    stage;
    //    VkPipelineLayout                   layout;
    //    VkPipeline                         basePipelineHandle;
    //    int32_t                            basePipelineIndex;
    //} VkComputePipelineCreateInfo;
    class ComputePipelineState : public PipelineState
    {

    };

    class VertexInputState : RefCount
    {

    };

    class ViewportState : RefCount
    {

    };

    class RasterizationState : RefCount
    {

    };

    class MultiSampleState : RefCount
    {

    };

    class DepthStencilState : RefCount
    {

    };

    class ColorBlendState : RefCount
    {

    };

    class RenderPass : RefCount
    {

    };

    //typedef struct VkGraphicsPipelineCreateInfo {
    //    VkStructureType                                  sType;
    //    const void* pNext;
    //    VkPipelineCreateFlags                            flags;
    //    uint32_t                                         stageCount;
    //    const VkPipelineShaderStageCreateInfo* pStages;
    //    const VkPipelineVertexInputStateCreateInfo* pVertexInputState;
    //    const VkPipelineInputAssemblyStateCreateInfo* pInputAssemblyState;
    //    const VkPipelineTessellationStateCreateInfo* pTessellationState;
    //    const VkPipelineViewportStateCreateInfo* pViewportState;
    //    const VkPipelineRasterizationStateCreateInfo* pRasterizationState;
    //    const VkPipelineMultisampleStateCreateInfo* pMultisampleState;
    //    const VkPipelineDepthStencilStateCreateInfo* pDepthStencilState;
    //    const VkPipelineColorBlendStateCreateInfo* pColorBlendState;
    //    const VkPipelineDynamicStateCreateInfo* pDynamicState;
    //    VkPipelineLayout                                 layout;
    //    VkRenderPass                                     renderPass;
    //    uint32_t                                         subpass;
    //    VkPipeline                                       basePipelineHandle;
    //    int32_t                                          basePipelineIndex;
    //} VkGraphicsPipelineCreateInfo;
    class GraphicsPipelineState : public PipelineState
    {
    private:
        SharedPtr<VertexInputState> mVertexInputState;
        SharedPtr<ViewportState> mViewportState;
        SharedPtr<RasterizationState> mRasterizationState;
        SharedPtr<MultiSampleState> mMultiSampleState;
        SharedPtr<DepthStencilState> mDepthStencilState;
        SharedPtr<ColorBlendState> mColorBlendState;
        SharedPtr<RenderPass> mRenderPass;
    };
}
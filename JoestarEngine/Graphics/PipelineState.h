#pragma once
#include "../Core/RefCount.h"
#include "../Core/ObjectDefines.h"
#include "../Container/Ptr.h"
#include "GraphicDefines.h"
#include "GPUTexture.h"
namespace Joestar {
    class PipelineLayout : RefCount
    {

    };

    class PipelineShader : RefCount
    {

    };

	class PipelineState : public GPUResource
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

    class ViewportState : public GPUResource
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

    class RenderPass : public GPUResource
    {
    public:
        void SetLoadOP(AttachmentLoadOP op)
        {
            SetColorLoadOP(op);
            SetDepthStencilLoadOP(op);
        }
        void SetDepthStencilLoadOP(AttachmentLoadOP op)
        {
            SetDepthLoadOP(op);
            SetStencilLoadOP(op);
        }
        void SetStoreOP(AttachmentStoreOP op)
        {
            SetColorStoreOP(op);
            SetDepthStencilStoreOP(op);
        }
        void SetDepthStencilStoreOP(AttachmentStoreOP  op)
        {
            SetDepthStoreOP(op);
            SetStencilStoreOP(op);
        }
        GET_SET_STATEMENT_PREFIX_INITVALUE(bool, Clear, b, false);
        GET_SET_STATEMENT_INITVALUE(ImageFormat, ColorFormat, ImageFormat::R8G8B8A8_SRGB);
        GET_SET_STATEMENT_INITVALUE(ImageFormat, DepthStencilFormat, ImageFormat::D32S8);
        GET_SET_STATEMENT_INITVALUE(AttachmentLoadOP, ColorLoadOP, AttachmentLoadOP::LOAD);
        GET_SET_STATEMENT_INITVALUE(AttachmentLoadOP, DepthLoadOP, AttachmentLoadOP::LOAD);
        GET_SET_STATEMENT_INITVALUE(AttachmentLoadOP, StencilLoadOP, AttachmentLoadOP::LOAD);
        GET_SET_STATEMENT_INITVALUE(AttachmentStoreOP, ColorStoreOP, AttachmentStoreOP::STORE);
        GET_SET_STATEMENT_INITVALUE(AttachmentStoreOP, DepthStoreOP, AttachmentStoreOP::STORE);
        GET_SET_STATEMENT_INITVALUE(AttachmentStoreOP, StencilStoreOP, AttachmentStoreOP::STORE);
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
        RenderPass* mRenderPass;
    };
}
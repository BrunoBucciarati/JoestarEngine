#pragma once
#include "../Core/RefCount.h"
#include "../Core/ObjectDefines.h"
#include "../Container/Ptr.h"
#include "../Container/Vector.h"
#include "GraphicDefines.h"
#include "GPUTexture.h"
#include "Viewport.h"
namespace Joestar {
    class PipelineLayout : public GPUResource
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
        void InsertAllHash() override
        {
            HashInsert(mIndex);
            HashInsert(mFlags);
        }
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

    class VertexInputState : public GPUResource
    {

    };


    class RasterizationState : public GPUResource
    {
        GET_SET_STATEMENT_PREFIX_INITVALUE(bool, DepthClamp, b, false);
        GET_SET_STATEMENT_PREFIX_INITVALUE(bool, DiscardEnable, b, false);
        GET_SET_STATEMENT_INITVALUE(PolygonMode, PolygonMode, PolygonMode::FILL);
        GET_SET_STATEMENT_INITVALUE(F32, LineWidth, 1.0F);
        GET_SET_STATEMENT_INITVALUE(CullMode, CullMode, CullMode::BACK);
        GET_SET_STATEMENT_INITVALUE(FrontFaceMode, FrontFace, FrontFaceMode::COUNTER_CLOCKWISE);
        GET_SET_STATEMENT_PREFIX_INITVALUE(bool, DepthBias, b, false);
        GET_SET_STATEMENT_INITVALUE(F32, DepthBiasConstantFactor, 0.0F);
        GET_SET_STATEMENT_INITVALUE(F32, DepthBiasClamp, 0.0F);
        GET_SET_STATEMENT_INITVALUE(F32, DepthBiasSlopeFactor, 0.0F);
    private:
        void InsertAllHash() override
        {
            HashInsert(bDepthClamp);
            HashInsert(bDiscardEnable);
            HashInsert(mPolygonMode);
            HashInsert(mLineWidth);
            HashInsert(mCullMode);
            HashInsert(mFrontFace);
            if (bDepthBias)
            {
                HashInsert(mDepthBiasConstantFactor);
                HashInsert(mDepthBiasClamp);
                HashInsert(mDepthBiasSlopeFactor);
            }
        }
    };

    class MultiSampleState : public GPUResource
    {
        GET_SET_STATEMENT_PREFIX_INITVALUE(bool, SampleShading, b, false);
        GET_SET_STATEMENT_INITVALUE(U32, RasterizationSamples, 1);
        GET_SET_STATEMENT_INITVALUE(F32, MinSampleShading, 1.0F);
        GET_SET_STATEMENT_PREFIX_INITVALUE(bool, AlphaToCoverage, b, false);
        GET_SET_STATEMENT_PREFIX_INITVALUE(bool, AlphaToOne, b, false);
    private:
        void InsertAllHash() override
        {
            HashInsert(bSampleShading);
            HashInsert(mRasterizationSamples);
            HashInsert(mMinSampleShading);
            HashInsert(bAlphaToCoverage);
            HashInsert(bAlphaToOne);
        }
    };

    class DepthStencilState : public GPUResource
    {
        GET_SET_STATEMENT_PREFIX_INITVALUE(bool, DepthTest, b, true);
        GET_SET_STATEMENT_PREFIX_INITVALUE(bool, DepthWrite, b, true);
        GET_SET_STATEMENT_INITVALUE(CompareOp, DeptyCompareOp, CompareOp::ALWAYS);
        GET_SET_STATEMENT_PREFIX_INITVALUE(bool, StencilTest, b, false);
        GET_SET_STATEMENT_INITVALUE(StencilOpState, StencilFront, );
        GET_SET_STATEMENT_INITVALUE(StencilOpState, StencilBack, );
        void InsertAllHash() override
        {
            HashInsert(bDepthTest);
            HashInsert(bDepthWrite);
            HashInsert(mDeptyCompareOp);
            if (bStencilTest)
            {
                HashInsert(mStencilFront.Hash());
                HashInsert(mStencilBack.Hash());
            }
        }
    };

    class ColorAttachmentState : public GPUResource
    {
        GET_SET_STATEMENT_PREFIX_INITVALUE(bool, BlendEnable, b, true);
        GET_SET_STATEMENT_INITVALUE(BlendFactor, SrcColorBlendFactor, BlendFactor::SRC_ALPHA);
        GET_SET_STATEMENT_INITVALUE(BlendFactor, DstColorBlendFactor, BlendFactor::DST_ALPHA);
        GET_SET_STATEMENT_INITVALUE(BlendOp, ColorBlendOp, BlendOp::ADD);
        GET_SET_STATEMENT_INITVALUE(BlendFactor, SrcAlphaBlendFactor, BlendFactor::SRC_ALPHA);
        GET_SET_STATEMENT_INITVALUE(BlendFactor, DstAlphaBlendFactor, BlendFactor::DST_ALPHA);
        GET_SET_STATEMENT_INITVALUE(BlendOp, AlphaBlendOp, BlendOp::ADD);
        GET_SET_STATEMENT_INITVALUE(ColorWriteMask, ColorWriteMask, ColorWriteMask::ALL);
        void InsertAllHash() override
        {
            HashInsert(bBlendEnable);
            HashInsert(mSrcColorBlendFactor);
            HashInsert(mDstColorBlendFactor);
            HashInsert(mColorBlendOp);
            HashInsert(mSrcAlphaBlendFactor);
            HashInsert(mDstAlphaBlendFactor);
            HashInsert(mAlphaBlendOp);
            HashInsert(mColorWriteMask);
        }
    };

    class ColorBlendState : public GPUResource
    {
        GET_SET_STATEMENT_PREFIX_INITVALUE(bool, LogicOpEnable, b, true);
        GET_SET_STATEMENT(Vector<ColorAttachmentState>, ColorAttachments);
        void InsertAllHash() override
        {
            HashInsert(bLogicOpEnable);
            for (U32 i = 0; i < mColorAttachments.Size(); ++i)
            {
                HashInsert(mColorAttachments[i].Hash());
            }
        }
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
        void InsertAllHash() override
        {
            HashInsert(bClear);
            HashInsert(mColorFormat);
            HashInsert(mDepthStencilFormat);
            HashInsert(mColorLoadOP);
            HashInsert(mDepthLoadOP);
            HashInsert(mStencilLoadOP);
            HashInsert(mColorStoreOP);
            HashInsert(mDepthStoreOP);
            HashInsert(mStencilStoreOP);
        }
    };

    class GraphicsPipelineState : public PipelineState
    {
    private:
        VertexInputState* mVertexInputState;
        RasterizationState* mRasterizationState;
        MultiSampleState* mMultiSampleState;
        DepthStencilState* mDepthStencilState;
        ColorBlendState* mColorBlendState;
        RenderPass* mRenderPass;
        Viewport* mViewport;
        void InsertAllHash() override
        {
            HashInsert(PipelineState::Hash());
            HashInsert(mVertexInputState->Hash());
            HashInsert(mRasterizationState->Hash());
            HashInsert(mMultiSampleState->Hash());
            HashInsert(mDepthStencilState->Hash());
            HashInsert(mColorBlendState->Hash());
            HashInsert(mRenderPass->Hash());
            HashInsert(mViewport->Hash());
        }
    };
}
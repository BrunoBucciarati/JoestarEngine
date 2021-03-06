#pragma once
#include "../Core/Minimal.h"
#include "GraphicDefines.h"
#include "GPUImage.h"
#include "Viewport.h"
#include "Shader/ShaderProgram.h"
#include "VertexBuffer.h"
#include "Descriptor.h"
namespace Joestar {
    class PipelineLayout : public GPUResource
    {
    public:
        void ResizeLayouts(U32 sz);
        SharedPtr<DescriptorSetLayout> GetSetLayout(U32 idx)
        {
            if (mSetLayouts.Size() > idx)
                return mSetLayouts[idx];
            return nullptr;
        }
        Vector<SharedPtr<DescriptorSetLayout>>& GetSetLayouts()
        {
            return mSetLayouts;
        }
        U32 GetLayoutsSize()
        {
            return mSetLayouts.Size();
        }
        U32 GetPushConstantSize()
        {
            return 0;
        }
    private:
        Vector<SharedPtr<DescriptorSetLayout>> mSetLayouts;
    };

	class PipelineState : public GPUResource
	{
    public:
        ShaderProgram* GetShaderProgram()
        {
            return mProgram;
        }
        void SetShaderProgram(ShaderProgram* p)
        {
            mProgram = p;
        }
    protected:
        ShaderProgram* mProgram;
        PipelineLayout* mLayout;
        U32 mIndex{ 0 };
        U64 mFlags{ 0 };
        void InsertAllHash() override
        {
            //HashInsert(mIndex);
            //HashInsert(mFlags);
            HashInsert(mProgram->GetHandle());
        }
	};

    class ComputePipelineState : public PipelineState
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
        GET_SET_STATEMENT_INITVALUE(CompareOp, DepthCompareOp, CompareOp::ALWAYS);
        GET_SET_STATEMENT_PREFIX_INITVALUE(bool, StencilTest, b, false);
        GET_SET_STATEMENT_INITVALUE(StencilOpState, StencilFront, );
        GET_SET_STATEMENT_INITVALUE(StencilOpState, StencilBack, );
        void InsertAllHash() override
        {
            HashInsert(bDepthTest);
            HashInsert(bDepthWrite);
            HashInsert(mDepthCompareOp);
            if (bStencilTest)
            {
                HashInsert(mStencilFront.Hash());
                HashInsert(mStencilBack.Hash());
            }
        }
    };

    class ColorAttachmentState : public GPUResource
    {
        GET_SET_STATEMENT_PREFIX_INITVALUE(bool, BlendEnable, b, false);
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
        GET_SET_STATEMENT_PREFIX_INITVALUE(bool, LogicOpEnable, b, false);
        GET_SET_STATEMENT_REF(Vector<ColorAttachmentState>, Attachments);
    public:
        void AddAttachment(const ColorAttachmentState& state)
        {
            mAttachments.Push(state);
        }
        void InsertAllHash() override
        {
            HashInsert(bLogicOpEnable);
            for (U32 i = 0; i < mAttachments.Size(); ++i)
            {
                HashInsert(mAttachments[i].Hash());
            }
        }
    };

    class RenderPass : public GPUResource
    {
    public:
        void SetLoadOp(AttachmentLoadOp op)
        {
            SetColorLoadOp(op);
            SetDepthStencilLoadOp(op);
        }
        void SetDepthStencilLoadOp(AttachmentLoadOp op)
        {
            SetDepthLoadOp(op);
            SetStencilLoadOp(op);
        }
        void SetStoreOp(AttachmentStoreOp op)
        {
            SetColorStoreOp(op);
            SetDepthStencilStoreOp(op);
        }
        void SetDepthStencilStoreOp(AttachmentStoreOp  op)
        {
            SetDepthStoreOp(op);
            SetStencilStoreOp(op);
        }
        void SetColorStoreOp(AttachmentStoreOp op)
        {
            for (U32 i = 0; i < mColorStoreOps.Size(); ++i)
            {
                mColorStoreOps[i] = op;
            }
        }
        void SetColorLoadOp(AttachmentLoadOp op)
        {
            for (U32 i = 0; i < mColorLoadOps.Size(); ++i)
            {
                mColorLoadOps[i] = op;
            }
        }
        ImageFormat GetColorFormat(U32 idx)
        {
            return mColorFormats[idx];
        }
        AttachmentStoreOp GetColorStoreOp(U32 idx)
        {
            return mColorStoreOps[idx];
        }
        AttachmentLoadOp GetColorLoadOp(U32 idx)
        {
            return mColorLoadOps[idx];
        }
        GET_SET_STATEMENT_PREFIX_INITVALUE(bool, Clear, b, false);
        GET_SET_STATEMENT_PREFIX_INITVALUE(bool, HasDepthStencil, b, true);
        GET_SET_STATEMENT_INITVALUE(U32, ColorAttachmentCount, 1);
        GET_SET_STATEMENT_INITVALUE(Vector<ImageFormat>, ColorFormats, ImageFormat::B8G8R8A8_SRGB);
        GET_SET_STATEMENT_INITVALUE(ImageFormat, DepthStencilFormat, ImageFormat::D32S8);
        GET_SET_STATEMENT_INITVALUE(Vector<AttachmentLoadOp>, ColorLoadOps, AttachmentLoadOp::LOAD);
        GET_SET_STATEMENT_INITVALUE(AttachmentLoadOp, DepthLoadOp, AttachmentLoadOp::LOAD);
        GET_SET_STATEMENT_INITVALUE(AttachmentLoadOp, StencilLoadOp, AttachmentLoadOp::LOAD);
        GET_SET_STATEMENT_INITVALUE(Vector<AttachmentStoreOp>, ColorStoreOps, AttachmentStoreOp::STORE);
        GET_SET_STATEMENT_INITVALUE(AttachmentStoreOp, DepthStoreOp, AttachmentStoreOp::STORE);
        GET_SET_STATEMENT_INITVALUE(AttachmentStoreOp, StencilStoreOp, AttachmentStoreOp::STORE);
        void InsertAllHash() override
        {
            HashInsert(bClear);
            HashInsert(bHasDepthStencil);
            HashInsert(mColorAttachmentCount);
            for (U32 i = 0; i < mColorFormats.Size(); ++i)
            {
                HashInsert(mColorFormats[i]);
            }
            HashInsert(mDepthStencilFormat);
            for (U32 i = 0; i < mColorLoadOps.Size(); ++i)
            {
                HashInsert(mColorLoadOps[i]);
            }
            HashInsert(mDepthLoadOp);
            HashInsert(mStencilLoadOp);
            for (U32 i = 0; i < mColorStoreOps.Size(); ++i)
            {
                HashInsert(mColorStoreOps[i]);
            }
            HashInsert(mDepthStoreOp);
            HashInsert(mStencilStoreOp);
        }
    };

    class GraphicsPipelineState : public PipelineState
    {
        //GET_SET_STATEMENT(VertexInputState, VertexInputState);
        GET_SET_STATEMENT(RenderPass*, RenderPass);
        GET_SET_STATEMENT(ColorBlendState*, ColorBlendState);
        GET_SET_STATEMENT(RasterizationState*, RasterizationState);
        GET_SET_STATEMENT(MultiSampleState*, MultiSampleState);
        GET_SET_STATEMENT(DepthStencilState*, DepthStencilState);
        GET_SET_STATEMENT(Viewport*, Viewport);
        GET_SET_STATEMENT(SharedPtr<PipelineLayout>, PipelineLayout);
        GET_SET_STATEMENT_REF(PODVector<InputBinding>, InputBindings);
        GET_SET_STATEMENT_REF(PODVector<InputAttribute>, InputAttributes);
        GET_SET_STATEMENT_INITVALUE(U32, TessellationPatchControlPoints, 0);
        GET_SET_STATEMENT(U32, HashIndex);
         
        U32 GetNumInputBindings()
        {
            return mInputBindings.Size();
        }

        U32 GetNumInputAttributes()
        {
            return mInputAttributes.Size();
        }

        InputBinding& GetInputBinding(U32 idx)
        {
            return mInputBindings[idx];
        }

        InputAttribute& GetInputAttribute(U32 idx)
        {
            return mInputAttributes[idx];
        }

        void InsertAllHash() override
        {
            HashInsert(PipelineState::Hash());
            HashInsert(mRasterizationState->Hash());
            HashInsert(mMultiSampleState->Hash());
            HashInsert(mDepthStencilState->Hash());
            HashInsert(mColorBlendState->Hash());
            HashInsert(mRenderPass->Hash());
            HashInsert(mViewport->Hash());
            for (auto& binding : mInputBindings)
            {
                HashInsert(binding.Hash());
            }
            for (auto& attr : mInputAttributes)
            {
                HashInsert(attr.Hash());
            }
        }
    };
}
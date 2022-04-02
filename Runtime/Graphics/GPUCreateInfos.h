#pragma once
#include "GPUTexture.h"
#include "Viewport.h"
#include "Descriptor.h"
#include "SwapChain.h"
#include "../Container/Vector.h"
namespace Joestar
{
	struct GPUImageCreateInfo
	{
		ImageType type;
		ImageFormat format;
		U32 usage;
		U32 width;
		U32 height;
		U32 depth;
		U32 layer;
		U32 mipLevels;
		U32 samples;
		U32 num{1};
	};

	struct GPUImageViewCreateInfo
	{
		GPUResourceHandle imageHandle;
		ImageViewType type;
		ImageFormat format;
		U32 aspectBits;
		U32 mipLevels;
		U32 baseMipLevel;
		U32 layer;
		U32 baseLayer;
		U32 num{1};
	};

	struct GPUSwapChainCreateInfo
	{
		SwapChain* swapChain;
	};

	struct GPUFrameBufferCreateInfo
	{
		U32 msaaSamples{ 1 };
	};

	struct GPURenderPassCreateInfo
	{
		ImageFormat colorFormat{ ImageFormat::B8G8R8A8_SRGB };
		ImageFormat depthStencilFormat{ImageFormat::D32S8};
		AttachmentLoadOp colorLoadOp{ AttachmentLoadOp::DONT_CARE };
		AttachmentLoadOp depthLoadOp{ AttachmentLoadOp::DONT_CARE };
		AttachmentLoadOp stencilLoadOp{ AttachmentLoadOp::DONT_CARE };
		AttachmentStoreOp colorStoreOp{ AttachmentStoreOp::DONT_CARE };
		AttachmentStoreOp depthStoreOp{ AttachmentStoreOp::DONT_CARE };
		AttachmentStoreOp stencilStoreOp{ AttachmentStoreOp::DONT_CARE };
		bool clear{ false };
		
		U32 msaaSamples{ 1 };
	};

	struct GPUDepthStencilStateCreateInfo
	{
		bool depthTest{ true };
		bool depthWrite{ true };
		CompareOp depthCompareOp{ CompareOp::LESS_EQUAL };
		bool stencilTest{ false };
		StencilOpState stencilFront{};
		StencilOpState stencilBack{};
	};

	struct GPUMultiSampleStateCreateInfo
	{
		bool sampleShading;
		U32 rasterizationSamples;
		F32 minSampleShading;
		bool alphaToCoverage;
		bool alphaToOne;
	};

	struct GPURasterizationStateCreateInfo
	{
		bool depthClamp;
		bool discardEnable;
		PolygonMode polygonMode;
		F32 lineWidth;
		CullMode cullMode;
		FrontFaceMode frontFace;
		bool depthBias;
		F32 depthBiasConstantFactor;
		F32 depthBiasClamp;
		F32 depthBiasSlopeFactor;
	};

	struct GPUColorAttachmentStateCreateInfo
	{
		bool blendEnable;
		BlendFactor srcColorBlendFactor;
		BlendFactor dstColorBlendFactor;
		BlendOp colorBlendOp;
		BlendFactor srcAlphaBlendFactor;
		BlendFactor dstAlphaBlendFactor;
		BlendOp alphaBlendOp;
		ColorWriteMask writeMask;
	};

	struct GPUColorBlendStateCreateInfo
	{
		bool logicOpEnable;
		U32 numAttachments;
		Vector<GPUColorAttachmentStateCreateInfo> attachments;
	};

	struct GPUPipelineStateCreateInfo
	{
		GPUResourceHandle shaderProramHandle;
		GPUResourceHandle pipelineLayoutHandle;
	};

	struct GPUGraphicsPipelineStateCreateInfo : GPUPipelineStateCreateInfo
	{
		GPUResourceHandle renderPassHandle;
		GPUResourceHandle colorBlendStateHandle;
		GPUResourceHandle multiSampleStateHandle;
		GPUResourceHandle rasterizationStateHandle;
		GPUResourceHandle depthStencilStateHandle;
		GPUResourceHandle vertexBufferHandle;
		Viewport viewport;
		U32 numInputBindings;
		U32 numInputAttributes;
		PODVector<InputAttribute> inputAttributes;
		PODVector<InputBinding> inputBindings;
	};
	struct GPUComputePipelineStateCreateInfo : GPUPipelineStateCreateInfo
	{
	};

	struct GPUCommandBufferCreateInfo
	{
		GPUQueue queue;
	};

	struct GPUIndexBufferCreateInfo
	{
		U32 indexCount;
		U32 indexSize;
		GPUResourceHandle memoryHandle;
	};

	struct GPUVertexBufferCreateInfo
	{
		U32 vertexCount;
		U32 vertexSize;
		U32 elementCount{ 0 };
		GPUResourceHandle memoryHandle;
		PODVector<VertexElement> elements;
	};

	struct GPUUniformBufferCreateInfo
	{
		U32 hash;
		U32 size;
	};


	struct GPUShaderCreateInfo
	{
		ShaderStage stage;
		void* file;
	};

	struct GPUShaderProgramCreateInfo
	{
		U32 stageMask;
		U32 numStages;
		U32 numSetLayouts;
		PODVector<GPUResourceHandle> shaderHandles;
		PODVector<GPUResourceHandle> setLayoutHandles;
	};

	struct GPUDescriptorSetsCreateInfo
	{
		GPUResourceHandle layoutHandle;
	};

	struct GPUDescriptorSetsUpdateInfo
	{
		struct Entry
		{
			GPUResourceHandle setHandle;
			U32 binding;
			DescriptorType type;
			U32 count;
			GPUResourceHandle uniformHandle;
		};
		U32 num;
		PODVector<Entry> updateSets;
	};

	//PushConstant
	//VkPushConstantRange pushConstantRange{};
	//pushConstantRange.stageFlags = pushConsts->GetStageFlags();
	//pushConstantRange.offset = 0;
	//pushConstantRange.size = pushConsts->size;
	//pipelineLayoutInfo.pushConstantRangeCount = 1; // Optional
	//pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; // Optional
	struct GPUPipelineLayoutCreateInfo
	{
		U32 numLayouts;
		U32 numPushConstants;
		PODVector<GPUResourceHandle> setLayoutHandles;
		PODVector<GPUResourceHandle> pushConstantHandles;
	};

	union ClearColorValue {
		F32	   f32[4];
		U32    u32[4];
		I32    i32[4];
	};
	struct ClearDepthStencilValue {
		F32    depth;
		U32    stencil;
	};

	union ClearValue {
		ClearColorValue           color;
		ClearDepthStencilValue    depthStencil;
	};

	struct RenderPassBeginInfo
	{
		GPUResourceHandle passHandle;
		GPUResourceHandle fbHandle;
		Rect renderArea;
		U32 numClearValues;
		PODVector<ClearValue> clearValues;
	};

	struct GPUDescriptorSetLayoutBindingMember
	{
		U32 ID;
		U32 offset;
		U32 size;
	};

	struct GPUDescriptorSetLayoutBinding
	{
		U32 binding{ 0 };
		DescriptorType type;
		U32 count{ 0 };
		U32 stage{ 0 };
		U32 size{ 0 };
		U32 numMembers;
		PODVector<GPUDescriptorSetLayoutBindingMember> members;
	};
}
#pragma once
#include "GPUTexture.h"
#include "Viewport.h"
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
		GPUResourceHandle imageViewHandle;
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
		UniformType type;
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
		PODVector<GPUResourceHandle> shaderHandles;
	};
}
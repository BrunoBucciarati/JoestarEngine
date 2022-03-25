#pragma once
#include "GPUTexture.h"
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
		ImageFormat colorFormat;
		ImageFormat depthStencilFormat;
		AttachmentLoadOP colorLoadOp;
		AttachmentLoadOP depthLoadOp;
		AttachmentLoadOP stencilLoadOp;
		AttachmentStoreOP colorStoreOp;
		AttachmentStoreOP depthStoreOp;
		AttachmentStoreOP stencilStoreOp;
		bool clear{ false };
		
		U32 msaaSamples{ 1 };
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
		GPUResourceHandle memoryHandle;
	};

	struct GPUUniformBufferCreateInfo
	{
		U32 type{ 0 };
	};
}
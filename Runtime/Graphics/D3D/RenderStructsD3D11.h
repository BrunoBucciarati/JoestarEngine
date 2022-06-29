#pragma once
#include <d3d11.h>
#include "../../Platform/Platform.h"
#include "../../Container/Vector.h"
#include "../GPUCreateInfos.h"

namespace Joestar
{
	class FrameBufferD3D11;
	class RenderAPID3D11;
	class GraphicsPipelineStateD3D11;
	class ComputePipelineStateD3D11;
	class VertexBufferD3D11;
	class IndexBufferD3D11;
	class SoftwareDescriptorSets;
	class ShaderD3D11;

	class SwapChainD3D11
	{
	public:
		IDXGISwapChain* swapChain{ nullptr };
		U32 width;
		U32 height;
		FrameBufferD3D11* frameBuffer{ nullptr };
	};

	class ImageD3D11
	{
	public:
		ImageType type;
		ImageFormat fmt;
		ID3D11Resource* image;
		void Create(ID3D11Device* device, GPUImageCreateInfo& createInfo, GPUMemory* mem = nullptr);
		ID3D11Texture2D* GetTexture2D()
		{
			if (type == ImageType::TYPE_2D)
				return static_cast<ID3D11Texture2D*>(image);
			return nullptr;
		}
	};

	class ImageViewD3D11
	{
	public:
		ImageD3D11* GetImage()
		{
			return image;
		}
		ID3D11ShaderResourceView* imageView;
		void Create(ID3D11Device* device, ImageD3D11* img, GPUImageViewCreateInfo& createInfo);
		ImageD3D11* image;
		D3D11_RENDER_TARGET_VIEW_DESC GetRenderTargetViewDesc();
		D3D11_DEPTH_STENCIL_VIEW_DESC GetDepthStencilViewDesc();
		ImageFormat format;
		D3D11_RTV_DIMENSION rtvDimension;
		D3D11_DSV_DIMENSION dsvDimension;
	};

	class SamplerD3D11
	{
	public:
		ID3D11SamplerState* samplerState;
		void Create(ID3D11Device* device, GPUSamplerCreateInfo&);
	};

	class TextureD3D11
	{
	public:
		ImageViewD3D11* imageView;
		SamplerD3D11* sampler;
	};

	class FrameBufferD3D11
	{
	public:
		Vector<ImageViewD3D11*> colorAttachments;
		ImageViewD3D11* depthStencilAttachment;
		Vector<ID3D11RenderTargetView*> renderTargetViews;
		ID3D11DepthStencilView* depthStencilView;
	};

	class CommandBufferD3D11
	{
	public:
		GPUCommandBufferCreateInfo createInfo;
		ID3D11DeviceContext* deviceContext;
		RenderAPID3D11* renderContext;
		void BeginRenderPass(RenderPassBeginInfo& beginInfo);
		void BindGraphicsPipeline(GraphicsPipelineStateD3D11* pipeline);
		void BindComputePipeline(ComputePipelineStateD3D11* pipeline);
		void BindVertexBuffer(VertexBufferD3D11* vb, U32 slot=0);
		void BindIndexBuffer(IndexBufferD3D11* ib);
		void BindDescriptorSets(SoftwareDescriptorSets* sets, U32 setIndex = 0);
		void Draw(U32 vertCount);
		void DrawIndexed(U32 indexCount, U32 indexStart, U32 vertStart);
		void SetViewport(const Viewport& vp);
	};

	class BufferD3D11
	{
	public:
		ID3D11Buffer* buffer;
		void CreateBuffer(ID3D11Device* device, D3D11_BUFFER_DESC& desc, GPUMemory& mem);
		void CreateBuffer(ID3D11Device* device, D3D11_BUFFER_DESC& desc);
	};

	class IndexBufferD3D11 : public BufferD3D11
	{
	public:
		void Create(ID3D11Device* device, GPUIndexBufferCreateInfo& createInfo, GPUMemory& mem);
		U32 count;
		U32 size;
		DXGI_FORMAT indexFormat;
	};

	class VertexBufferD3D11 : public BufferD3D11
	{
	public:
		void Create(ID3D11Device* device, GPUVertexBufferCreateInfo& createInfo, GPUMemory& mem);
		U32 count;
		U32 size;
	};

	class UniformBufferD3D11 : public BufferD3D11
	{
	public:
		void Create(ID3D11Device* device, GPUUniformBufferCreateInfo& createInfo);
		void SetStagingData(U8* dat, U32 sz);
		U8* GetStagingData() const
		{
			return data;
		}
		U32 GetSize() const
		{
			return size;
		}
	private:
		U32 size;
		U8* data;
	};

	class RenderPassD3D11
	{
	public:
		GPURenderPassCreateInfo createInfo;
	};

	class ColorBlendStateD3D11
	{
	public:
		ID3D11BlendState* blendState;
		void Create(ID3D11Device* device, GPUColorBlendStateCreateInfo& createInfo);
	};

	class DepthStencilStateD3D11
	{
	public:
		ID3D11DepthStencilState* depthStencilState;
		void Create(ID3D11Device* device, GPUDepthStencilStateCreateInfo& createInfo);
	};

	class RasterizationStateD3D11
	{
	public:
		ID3D11RasterizerState* rasterState;
		void Create(ID3D11Device* device, GPURasterizationStateCreateInfo& createInfo);
	};

	class GraphicsPipelineStateD3D11
	{
	public:
		void Create(ID3D11Device* device,RenderAPID3D11* ctx, GPUGraphicsPipelineStateCreateInfo& createInfo);
		ColorBlendStateD3D11* colorBlendState;
		DepthStencilStateD3D11* depthStencilState;
		RasterizationStateD3D11* rasterState;
		GPUMultiSampleStateCreateInfo* multiSampleState;
		GPUPipelineLayoutCreateInfo* pipelineLayout;
		RenderAPID3D11* renderCtx;
		VertexBufferD3D11* vertexBuffer;
		Viewport viewport;
		ID3D11InputLayout* inputLayout;
		PODVector<ShaderD3D11*> shaders;
	};

	class ComputePipelineStateD3D11
	{
	public:
		void Create(ID3D11Device* device, RenderAPID3D11* ctx, GPUComputePipelineStateCreateInfo& createInfo);
		PODVector<ShaderD3D11*> shaders;
		GPUPipelineLayoutCreateInfo* pipelineLayout;
		RenderAPID3D11* renderCtx;
	};

	class ShaderD3D11
	{
	public:
		union ShaderPtrD3D11
		{
			ID3D11VertexShader* vertexShader;
			ID3D11PixelShader* pixelShader;
			ID3D11ComputeShader* computeShader;
		} shaderPtr;
		void Create(ID3D11Device* device, GPUShaderCreateInfo& createInfo);
		ID3D10Blob* compiledBlob = 0;
		U32 stage{ 0 };

	};
}
#pragma once
#include <d3d11.h>
#include "../../Platform/Platform.h"
#include "../../Container/Vector.h"
#include "../GPUCreateInfos.h"

namespace Joestar
{
	class FrameBufferD3D11;
	class SwapChainD3D11
	{
	public:
		IDXGISwapChain* swapChain;
		U32 width;
		U32 height;
		FrameBufferD3D11* frameBuffer;
	};

	class ImageD3D11
	{
	public:
		ImageType type;
		ID3D11Resource* image;
		void Create(ID3D11Device* device, GPUImageCreateInfo& createInfo);
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
		DXGI_FORMAT format;
		D3D11_RTV_DIMENSION rtvDimension;
		D3D11_DSV_DIMENSION dsvDimension;
	};

	class SamplerD3D11
	{
		
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
	};


}
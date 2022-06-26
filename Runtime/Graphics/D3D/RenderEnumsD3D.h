#pragma once

#include <d3d11.h>
#include "../GPUCreateInfos.h"
namespace Joestar
{
	DXGI_FORMAT GetImageFormatD3D(ImageFormat fmt);
	D3D_SRV_DIMENSION GetViewDimensionD3D(ImageViewType type);
	D3D11_RTV_DIMENSION GetRTVDimensionD3D(ImageViewType type);
	D3D11_DSV_DIMENSION GetDSVDimensionD3D(ImageViewType type);
	D3D11_TEXTURE_ADDRESS_MODE GetD3D11AddressMode(SamplerAddressMode mode);
	D3D11_COMPARISON_FUNC GetD3D11ComparisonFunc(CompareOp op);
	D3D11_BLEND GetD3D11BlendFactor(BlendFactor factor);
	D3D11_BLEND_OP GetD3D11BlendOp(BlendOp op);
	D3D11_COLOR_WRITE_ENABLE GetD3D11ColorWriteMask(ColorWriteMask mask);
	D3D11_STENCIL_OP GetD3D11StencilOp(StencilOp op);
}
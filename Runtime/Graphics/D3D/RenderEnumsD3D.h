#pragma once

#include <d3d11.h>
#include "../GPUCreateInfos.h"
namespace Joestar
{
	DXGI_FORMAT GetImageFormatD3D(ImageFormat fmt);
	D3D_SRV_DIMENSION GetViewDimensionD3D(ImageViewType type);
	D3D11_RTV_DIMENSION GetRTVDimensionD3D(ImageViewType type);
	D3D11_DSV_DIMENSION GetDSVDimensionD3D(ImageViewType type);
}
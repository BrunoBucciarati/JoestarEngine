#include "RenderAPID3D11.h"
namespace Joestar {
	DXGI_FORMAT ImageFormatMapping[]
	{
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
		DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
		DXGI_FORMAT_R11G11B10_FLOAT,
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
		DXGI_FORMAT_R32_FLOAT,
		DXGI_FORMAT_R32_FLOAT,
		DXGI_FORMAT_R16_FLOAT
	};
	DXGI_FORMAT GetImageFormatD3D(ImageFormat fmt)
	{
		return ImageFormatMapping[(U32)fmt];
	}
	DXGI_FORMAT RenderTargetFormatMapping[]
	{
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
		DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
		DXGI_FORMAT_R11G11B10_FLOAT,
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
		DXGI_FORMAT_D32_FLOAT,
		DXGI_FORMAT_R32_FLOAT,
		DXGI_FORMAT_R16_FLOAT
	};
	DXGI_FORMAT GetRenderTargetFormatD3D(ImageFormat fmt)
	{
		return RenderTargetFormatMapping[(U32)fmt];
	}
	DXGI_FORMAT TypelessImageFormatMapping[]
	{
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
		DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
		DXGI_FORMAT_R11G11B10_FLOAT,
		DXGI_FORMAT_R24G8_TYPELESS,
		DXGI_FORMAT_R32G8X24_TYPELESS,
		DXGI_FORMAT_R32_TYPELESS,
		DXGI_FORMAT_R32_TYPELESS,
		DXGI_FORMAT_R16_TYPELESS
	};
	DXGI_FORMAT GetTypelessImageFormatD3D(ImageFormat fmt)
	{
		return TypelessImageFormatMapping[(U32)fmt];
	}

	const D3D_SRV_DIMENSION ViewDimensionMapping[]
	{
		D3D11_SRV_DIMENSION_TEXTURE1D,
		D3D11_SRV_DIMENSION_TEXTURE2D,
		D3D11_SRV_DIMENSION_TEXTURE3D,
		D3D11_SRV_DIMENSION_TEXTURECUBE,
		D3D11_SRV_DIMENSION_TEXTURE1DARRAY,
		D3D11_SRV_DIMENSION_TEXTURE2DARRAY,
		D3D11_SRV_DIMENSION_TEXTURECUBEARRAY
	};
	D3D_SRV_DIMENSION GetViewDimensionD3D(ImageViewType type)
	{
		return ViewDimensionMapping[(U32)type];
	}


	const D3D11_RTV_DIMENSION RTVDimensionMapping[]
	{
		D3D11_RTV_DIMENSION_TEXTURE1D,
		D3D11_RTV_DIMENSION_TEXTURE2D,
		D3D11_RTV_DIMENSION_TEXTURE3D,
		D3D11_RTV_DIMENSION_TEXTURE2D,
		D3D11_RTV_DIMENSION_TEXTURE1DARRAY,
		D3D11_RTV_DIMENSION_TEXTURE2DARRAY,
		D3D11_RTV_DIMENSION_TEXTURE2DARRAY
	};
	D3D11_RTV_DIMENSION GetRTVDimensionD3D(ImageViewType type)
	{
		return RTVDimensionMapping[(U32)type];
	}


	const D3D11_DSV_DIMENSION DSVDimensionMapping[]
	{
		D3D11_DSV_DIMENSION_TEXTURE1D,
		D3D11_DSV_DIMENSION_TEXTURE2D,
		D3D11_DSV_DIMENSION_TEXTURE2D,
		D3D11_DSV_DIMENSION_TEXTURE2D,
		D3D11_DSV_DIMENSION_TEXTURE1DARRAY,
		D3D11_DSV_DIMENSION_TEXTURE2DARRAY,
		D3D11_DSV_DIMENSION_TEXTURE2DARRAY
	};
	D3D11_DSV_DIMENSION GetDSVDimensionD3D(ImageViewType type)
	{
		return DSVDimensionMapping[(U32)type];
	}


	D3D11_TEXTURE_ADDRESS_MODE GetD3D11AddressMode(SamplerAddressMode mode)
	{
		if (mode == SamplerAddressMode::REPEAT)
		{
			return D3D11_TEXTURE_ADDRESS_WRAP;
		}
		else if (mode == SamplerAddressMode::CLAMP_TO_BORDER)
		{
			return D3D11_TEXTURE_ADDRESS_BORDER;
		}
		else if (mode == SamplerAddressMode::CLAMP_TO_EDGE)
		{
			return D3D11_TEXTURE_ADDRESS_CLAMP;
		}
		else if (mode == SamplerAddressMode::MIRRORED_REPEAT)
		{
			return D3D11_TEXTURE_ADDRESS_MIRROR;
		}
		else if (mode == SamplerAddressMode::MIRROR_CLAMP_TO_EDGE)
		{
			return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
		}
	}


	const D3D11_COMPARISON_FUNC ComparisonFuncMapping[]
	{
		D3D11_COMPARISON_NEVER,
		D3D11_COMPARISON_LESS,
		D3D11_COMPARISON_EQUAL,
		D3D11_COMPARISON_LESS_EQUAL,
		D3D11_COMPARISON_GREATER,
		D3D11_COMPARISON_NOT_EQUAL,
		D3D11_COMPARISON_GREATER_EQUAL,
		D3D11_COMPARISON_ALWAYS
	};
	D3D11_COMPARISON_FUNC GetD3D11ComparisonFunc(CompareOp op)
	{
		return ComparisonFuncMapping[(U32)op];
	}

	const D3D11_BLEND BlendFactorMapping[]
	{
		D3D11_BLEND_ZERO,
		D3D11_BLEND_ONE,
		D3D11_BLEND_SRC_COLOR,
		D3D11_BLEND_INV_SRC_COLOR,
		D3D11_BLEND_DEST_COLOR,
		D3D11_BLEND_INV_DEST_COLOR,
		D3D11_BLEND_SRC_ALPHA,
		D3D11_BLEND_INV_SRC_ALPHA,
		D3D11_BLEND_DEST_ALPHA,
		D3D11_BLEND_INV_DEST_ALPHA,
		D3D11_BLEND_BLEND_FACTOR,
		D3D11_BLEND_INV_BLEND_FACTOR,
		D3D11_BLEND_SRC1_ALPHA,
		D3D11_BLEND_INV_SRC1_ALPHA,
		D3D11_BLEND_SRC_ALPHA_SAT,
		D3D11_BLEND_SRC1_COLOR,
		D3D11_BLEND_INV_SRC1_COLOR,
		D3D11_BLEND_SRC1_ALPHA,
		D3D11_BLEND_INV_SRC1_ALPHA,
	};
	D3D11_BLEND GetD3D11BlendFactor(BlendFactor factor)
	{
		return BlendFactorMapping[(U32)factor];
	}

	const D3D11_BLEND_OP BlendOpMapping[]
	{
		D3D11_BLEND_OP_ADD,
		D3D11_BLEND_OP_SUBTRACT,
		D3D11_BLEND_OP_REV_SUBTRACT,
		D3D11_BLEND_OP_MIN,
		D3D11_BLEND_OP_MAX
	};
	D3D11_BLEND_OP GetD3D11BlendOp(BlendOp op)
	{
		return BlendOpMapping[(U32)op];
	}

	D3D11_COLOR_WRITE_ENABLE GetD3D11ColorWriteMask(ColorWriteMask mask)
	{
		if (mask == ColorWriteMask::ALL)
		{
			return D3D11_COLOR_WRITE_ENABLE_ALL;
		}
		U32 retMask = 0;
		if ((U32)mask & (U32)ColorWriteMask::R)
		{
			retMask |= D3D11_COLOR_WRITE_ENABLE_RED;
		}
		if ((U32)mask & (U32)ColorWriteMask::G)
		{
			retMask |= D3D11_COLOR_WRITE_ENABLE_GREEN;
		}
		if ((U32)mask & (U32)ColorWriteMask::B)
		{
			retMask |= D3D11_COLOR_WRITE_ENABLE_BLUE;
		}
		if ((U32)mask & (U32)ColorWriteMask::A)
		{
			retMask |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
		}
		return (D3D11_COLOR_WRITE_ENABLE)retMask;
	}

	const D3D11_STENCIL_OP StencilOpMapping[]
	{
		D3D11_STENCIL_OP_KEEP,
		D3D11_STENCIL_OP_ZERO,
		D3D11_STENCIL_OP_REPLACE,
		D3D11_STENCIL_OP_INCR_SAT,
		D3D11_STENCIL_OP_DECR_SAT,
		D3D11_STENCIL_OP_INVERT,
		D3D11_STENCIL_OP_INCR,
		D3D11_STENCIL_OP_DECR
	};
	D3D11_STENCIL_OP GetD3D11StencilOp(StencilOp op)
	{
		return StencilOpMapping[(U32)op];
	}

	const DXGI_FORMAT VertexFormatMapping[]
	{
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R32G32_FLOAT,
		DXGI_FORMAT_R32_FLOAT,
		DXGI_FORMAT_R32_UINT
	};
	DXGI_FORMAT GetVertexFormatD3D(VertexType type)
	{
		return VertexFormatMapping[(U32)type];
	}
}
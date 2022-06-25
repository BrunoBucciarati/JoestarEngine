#include "RenderStructsD3D11.h"
#include "RenderEnumsD3D.h"

static HRESULT hr1;
#define HR(f) { \
    hr1 = f; \
    if (FAILED(hr1)) \
        { LOGERROR("D3D FAILED: %s", #f);} \
}
namespace Joestar
{
	void ImageD3D11::Create(ID3D11Device* device, GPUImageCreateInfo& createInfo)
	{
        type = createInfo.type;
        if (type == ImageType::TYPE_1D)
        {
            //ID3D11Texture1D* tex;
            //HR(device->CreateTexture1D(&desc, 0, &tex));
            //image = tex;
        }
        else if(type == ImageType::TYPE_2D)
        {
            D3D11_TEXTURE2D_DESC desc = {};
            desc.Width = createInfo.width;
            desc.Height = createInfo.height;
            desc.MipLevels = createInfo.mipLevels;
            desc.ArraySize = createInfo.layer;
            desc.Format = GetImageFormatD3D(createInfo.format);
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            if (createInfo.usage | (U32)ImageUsageBits::STORAGE_BIT)
            {
                desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
            }
            desc.CPUAccessFlags = 0;
            desc.MiscFlags = 0;

            ID3D11Texture2D* tex;
            HR(device->CreateTexture2D(&desc, 0, &tex));
            image = tex;
        }
        else if (type == ImageType::TYPE_3D)
        {
            //ID3D11Texture3D* tex;
            //HR(device->CreateTexture3D(&desc, 0, &tex));
            //image = tex;
        }
	}


    void ImageViewD3D11::Create(ID3D11Device* device, ImageD3D11* img, GPUImageViewCreateInfo& createInfo)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC desc;
        desc.Format = GetImageFormatD3D(createInfo.format);
        desc.ViewDimension = GetViewDimensionD3D(createInfo.type);
        desc.Texture2D.MostDetailedMip = createInfo.baseMipLevel;
        desc.Texture2D.MipLevels = createInfo.mipLevels;
        HR(device->CreateShaderResourceView(image->image, &desc, &imageView));

        image = img;
        format = desc.Format;
        rtvDimension = GetRTVDimensionD3D(createInfo.type);
        dsvDimension = GetDSVDimensionD3D(createInfo.type);
    }

    D3D11_RENDER_TARGET_VIEW_DESC ImageViewD3D11::GetRenderTargetViewDesc()
    {
        D3D11_RENDER_TARGET_VIEW_DESC desc{};
        desc.Format = format;
        desc.ViewDimension = rtvDimension;
        desc.Texture2D = { 0 };
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC ImageViewD3D11::GetDepthStencilViewDesc()
    {
        D3D11_DEPTH_STENCIL_VIEW_DESC desc{};
        desc.Format = format;
        desc.ViewDimension = dsvDimension;
        desc.Flags = 0;
        desc.Texture2D = { 0 };
    }
}
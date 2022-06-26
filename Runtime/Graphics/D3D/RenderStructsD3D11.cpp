#include "RenderStructsD3D11.h"
#include "RenderEnumsD3D.h"
#include "RenderAPID3D11.h"
#include <d3dcompiler.h>

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


    void SamplerD3D11::Create(ID3D11Device* device, GPUSamplerCreateInfo& createInfo)
    {
        D3D11_SAMPLER_DESC sampDesc;
        ZeroMemory(&sampDesc, sizeof(sampDesc));
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;//过滤器
        sampDesc.AddressU = GetD3D11AddressMode(createInfo.addressModeU);//寻址模式
        sampDesc.AddressV = GetD3D11AddressMode(createInfo.addressModeV);
        sampDesc.AddressW = GetD3D11AddressMode(createInfo.addressModeW);
        if (createInfo.bCompare)
        {
            sampDesc.ComparisonFunc = GetD3D11ComparisonFunc(createInfo.compareOp);
        }
        else
        {
            sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        }
        sampDesc.MinLOD = createInfo.minLod;
        sampDesc.MaxLOD = createInfo.maxLod;
        // 创建采样器状态
        device->CreateSamplerState(&sampDesc, &samplerState);
    }

    void BufferD3D11::CreateBuffer(ID3D11Device* device, D3D11_BUFFER_DESC& desc)
    {
        HR(device->CreateBuffer(&desc, NULL, &buffer));
    }

    void BufferD3D11::CreateBuffer(ID3D11Device* device, D3D11_BUFFER_DESC& desc, GPUMemory& mem)
    {
        D3D11_SUBRESOURCE_DATA iinitData;
        ZeroMemory(&iinitData, sizeof(D3D11_SUBRESOURCE_DATA));
        iinitData.pSysMem = mem.data;
        HR(device->CreateBuffer(&desc, &iinitData, &buffer));
    }

    void IndexBufferD3D11::Create(ID3D11Device* device, GPUIndexBufferCreateInfo& createInfo, GPUMemory& mem)
    {
        D3D11_BUFFER_DESC ibd;
        ibd.Usage = D3D11_USAGE_IMMUTABLE;
        ibd.ByteWidth = createInfo.indexSize * createInfo.indexCount;
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.CPUAccessFlags = 0;
        ibd.MiscFlags = 0;
        ibd.StructureByteStride = 0;
        CreateBuffer(device, ibd, mem);

        count = createInfo.indexCount;
        size = createInfo.indexSize;
    }


    void VertexBufferD3D11::Create(ID3D11Device* device, GPUVertexBufferCreateInfo& createInfo, GPUMemory& mem)
    {
        D3D11_BUFFER_DESC vbd;
        vbd.Usage = D3D11_USAGE_IMMUTABLE;
        vbd.ByteWidth = createInfo.vertexSize * createInfo.vertexCount;
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.CPUAccessFlags = 0;
        vbd.MiscFlags = 0;
        vbd.StructureByteStride = 0;
        CreateBuffer(device, vbd, mem);

        count = createInfo.vertexCount;
        size = createInfo.vertexSize;
    }

    void UniformBufferD3D11::Create(ID3D11Device* device, GPUUniformBufferCreateInfo& createInfo)
    {
        //第六,设置(常量)缓存形容结构体,并创建常量缓存
        D3D11_BUFFER_DESC bufferDesc{};
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.ByteWidth = createInfo.size;   //结构体大小,必须为16字节倍数
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        CreateBuffer(device, bufferDesc);
        
        size = createInfo.size;
    }

    void UniformBufferD3D11::SetStagingData(U8* dat, U32 sz)
    {
        size = sz;
        data = dat;
    }

    void ColorBlendStateD3D11::Create(ID3D11Device* device, GPUColorBlendStateCreateInfo& createInfo)
    {
        D3D11_BLEND_DESC blendDesc = { 0 };
        blendDesc.AlphaToCoverageEnable = false;
        blendDesc.IndependentBlendEnable = false;

        for (U32 i = 0; i < createInfo.numAttachments; ++i)
        {
            GPUColorAttachmentStateCreateInfo& info = createInfo.attachments[i];
            blendDesc.RenderTarget[i].BlendEnable = info.blendEnable;
            blendDesc.RenderTarget[i].SrcBlend = GetD3D11BlendFactor(info.srcColorBlendFactor);
            blendDesc.RenderTarget[i].DestBlend = GetD3D11BlendFactor(info.dstColorBlendFactor);
            blendDesc.RenderTarget[i].BlendOp = GetD3D11BlendOp(info.colorBlendOp);
            blendDesc.RenderTarget[i].SrcBlendAlpha = GetD3D11BlendFactor(info.srcAlphaBlendFactor);
            blendDesc.RenderTarget[i].DestBlendAlpha = GetD3D11BlendFactor(info.dstAlphaBlendFactor);
            blendDesc.RenderTarget[i].BlendOpAlpha = GetD3D11BlendOp(info.alphaBlendOp);
            blendDesc.RenderTarget[i].RenderTargetWriteMask = GetD3D11ColorWriteMask(info.writeMask);
        }

        HR(device->CreateBlendState(&blendDesc, &blendState));
    }

    void DepthStencilStateD3D11::Create(ID3D11Device* device, GPUDepthStencilStateCreateInfo& createInfo)
    {
        D3D11_DEPTH_STENCIL_DESC dsDesc{ 0 };
        dsDesc.DepthEnable = createInfo.depthTest;
        dsDesc.DepthWriteMask = createInfo.depthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = GetD3D11ComparisonFunc(createInfo.depthCompareOp);
        dsDesc.StencilEnable = createInfo.stencilTest;
        dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
        dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
        StencilOpState& back = createInfo.stencilBack;
        StencilOpState& front = createInfo.stencilFront;
        dsDesc.BackFace = {
            GetD3D11StencilOp(back.failOp), 
            GetD3D11StencilOp(back.depthFailOp),
            GetD3D11StencilOp(back.passOp),
            GetD3D11ComparisonFunc(back.compareOp)
        };
        dsDesc.FrontFace = {
            GetD3D11StencilOp(front.failOp),
            GetD3D11StencilOp(front.depthFailOp),
            GetD3D11StencilOp(front.passOp),
            GetD3D11ComparisonFunc(front.compareOp)
        };
        HR(device->CreateDepthStencilState(&dsDesc, &depthStencilState));
    }

    void RasterizationStateD3D11::Create(ID3D11Device* device, GPURasterizationStateCreateInfo& createInfo)
    {
        D3D11_RASTERIZER_DESC rsDesc;
        ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
        rsDesc.FillMode = D3D11_FILL_SOLID;
        if (createInfo.cullMode == CullMode::FRONT)
        {
            rsDesc.CullMode = D3D11_CULL_FRONT;
        }
        else if (createInfo.cullMode == CullMode::BACK)
        {
            rsDesc.CullMode = D3D11_CULL_BACK;
        }
        else if (createInfo.cullMode == CullMode::FRONT_AND_BACK)
        {
            rsDesc.CullMode = D3D11_CULL_NONE;
        }
        rsDesc.FrontCounterClockwise = createInfo.frontFace == FrontFaceMode::COUNTER_CLOCKWISE;
        rsDesc.DepthClipEnable = createInfo.depthClamp;

        HR(device->CreateRasterizerState(&rsDesc, &rasterState));
    }

    void GraphicsPipelineStateD3D11::Create(ID3D11Device* device, RenderAPID3D11* ctx, GPUGraphicsPipelineStateCreateInfo& createInfo)
    {
        renderCtx = ctx;
        colorBlendState = ctx->GetColorBlendState(createInfo.colorBlendStateHandle);
        depthStencilState = ctx->GetDepthStencilState(createInfo.depthStencilStateHandle);
        rasterState = ctx->GetRasterizationState(createInfo.rasterizationStateHandle);
        multiSampleState = ctx->GetMultiSampleState(createInfo.multiSampleStateHandle);
        pipelineLayout = ctx->GetPipelineLayout(createInfo.pipelineLayoutHandle);
        vertexBuffer = ctx->GetVertexBuffer(createInfo.vertexBufferHandle);
        viewport = createInfo.viewport;
        GPUShaderProgramCreateInfo* shaderProgram = ctx->GetShaderProgram(createInfo.shaderProramHandle);
        shaders.Resize(shaderProgram->numStages);
        for (U32 i = 0; i < shaderProgram->numStages; ++i)
        {
            shaders[i] = ctx->GetShader(shaderProgram->shaderHandles[i]);
        }
    }

    void ComputePipelineStateD3D11::Create(ID3D11Device* device, RenderAPID3D11* ctx, GPUComputePipelineStateCreateInfo& createInfo)
    {
        renderCtx = ctx;
        pipelineLayout = ctx->GetPipelineLayout(createInfo.pipelineLayoutHandle);
    }

    void ShaderD3D11::Create(ID3D11Device* device, GPUShaderCreateInfo& createInfo)
    {
        File* filePtr = (File*)createInfo.file;

        UINT flags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
        flags |= D3DCOMPILE_DEBUG;
#endif
        String entryPoint = "CS";
        String target = "cs_5_0";
        if (createInfo.stage == ShaderStage::VS)
        {
            entryPoint = "VS";
            target = "vs_5_0";
        }
        else if (createInfo.stage == ShaderStage::PS)
        {
            entryPoint = "PS";
            target = "ps_5_0";
        }
        HR(D3DCompile(filePtr->GetBuffer(), filePtr->Size(), "TEST", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.CString(),
            target.CString(), flags, 0, &compiledShader, &compilationMsgs));
    }
}
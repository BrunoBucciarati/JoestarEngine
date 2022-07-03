#include "RenderStructsD3D11.h"
#include "RenderEnumsD3D.h"
#include "RenderAPID3D11.h"
#include <d3dcompiler.h>

static HRESULT hr1;
#define HR(f) { \
    hr1 = f; \
    if (FAILED(hr1)) \
        { LOGERROR("D3D FAILED IN STRUCT: %s\n", #f);} \
}
namespace Joestar
{
	void ImageD3D11::Create(ID3D11Device* device, GPUImageCreateInfo& createInfo, GPUMemory* mem)
	{
        type = createInfo.type;
        fmt = createInfo.format;
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
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.Format = GetTypelessImageFormatD3D(createInfo.format);
            desc.BindFlags = 0;
            if (createInfo.usage & (U32)ImageUsageBits::COLOR_ATTACHMENT_BIT)
            {
                desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
            }
            if (createInfo.usage & (U32)ImageUsageBits::DEPTH_STENCIL_ATTACHMENT_BIT)
            {
                desc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
            }
            if (createInfo.usage & (U32)ImageUsageBits::SAMPLED_BIT)
            {
                //DX 深度格式要处理成R32/R24这种
                //dxFmt = GetImageViewFormatD3D(createInfo.format);
                desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
            }
            if (createInfo.usage & (U32)ImageUsageBits::STORAGE_BIT)
            {
                desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
            }
            desc.CPUAccessFlags = 0;
            bool isCubeMap = createInfo.layer == 6;
            desc.MiscFlags = isCubeMap ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;

            ID3D11Texture2D* tex;
            if (mem)
            {
                //if (createInfo.layer > 0)
                {
                    PODVector<D3D11_SUBRESOURCE_DATA> tinitdatas;
                    tinitdatas.Resize(createInfo.layer);
                    for (U32 i = 0; i < createInfo.layer; ++i)
                    {
                        auto& tinitdata = tinitdatas[i];
                        ZeroMemory(&tinitdata, sizeof(tinitdata));

                        tinitdata.SysMemPitch = mem->size / (createInfo.layer * createInfo.height);
                        tinitdata.pSysMem = mem->data + i * mem->size / createInfo.layer;
                    }
                    //tinitdata.SysMemPitch = mem->size / ( 6 * createInfo.height);
                    HR(device->CreateTexture2D(&desc, tinitdatas.Buffer(), &tex));
                }
                //else
                //{
                //    D3D11_SUBRESOURCE_DATA tinitdata;
                //    ZeroMemory(&tinitdata, sizeof(tinitdata));
                //    tinitdata.pSysMem = mem->data;
                //    tinitdata.SysMemPitch = mem->size / createInfo.height;
                //}
            }
            else
            {
                HR(device->CreateTexture2D(&desc, 0, &tex));
            }
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
        image = img;
        D3D11_SHADER_RESOURCE_VIEW_DESC desc;
        desc.Format = GetImageFormatD3D(createInfo.format);
        desc.ViewDimension = GetViewDimensionD3D(createInfo.type);
        if (createInfo.type == ImageViewType::TYPE_CUBE)
        {
            desc.TextureCube.MostDetailedMip = createInfo.baseMipLevel;
            desc.TextureCube.MipLevels = createInfo.mipLevels;
        }
        else if (createInfo.type == ImageViewType::TYPE_2D)
        {
            desc.Texture2D.MostDetailedMip = createInfo.baseMipLevel;
            desc.Texture2D.MipLevels = createInfo.mipLevels;
        }
        HR(device->CreateShaderResourceView(image->image, &desc, &imageView));

        format = createInfo.format;
        rtvDimension = GetRTVDimensionD3D(createInfo.type);
        dsvDimension = GetDSVDimensionD3D(createInfo.type);
    }

    D3D11_RENDER_TARGET_VIEW_DESC ImageViewD3D11::GetRenderTargetViewDesc()
    {
        D3D11_RENDER_TARGET_VIEW_DESC desc{};
        desc.Format = GetRenderTargetFormatD3D(format);
        desc.ViewDimension = rtvDimension;
        desc.Texture2D = { 0 };
        return desc;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC ImageViewD3D11::GetDepthStencilViewDesc()
    {
        D3D11_DEPTH_STENCIL_VIEW_DESC desc{};
        desc.Format = GetRenderTargetFormatD3D(format);
        desc.ViewDimension = dsvDimension;
        desc.Flags = 0;
        desc.Texture2D = { 0 };
        return desc;
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
        indexFormat = count == 4 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
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
        else if (createInfo.cullMode == CullMode::NONE)
        {
            rsDesc.CullMode = D3D11_CULL_NONE;
        }
        rsDesc.FrontCounterClockwise = createInfo.frontFace == FrontFaceMode::COUNTER_CLOCKWISE;
        rsDesc.DepthClipEnable = true;// createInfo.depthClamp;

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
            if (shaders[i]->stage == (U32)ShaderStage::VS)
            {
                Vector<D3D11_INPUT_ELEMENT_DESC> vertexDescs;
                vertexDescs.Resize(createInfo.numInputAttributes);
                for (U32 j = 0; j < createInfo.numInputAttributes; ++j)
                {
                    InputAttribute& attr = createInfo.inputAttributes[j];
                    if (attr.semantic == VertexSemantic::POSITION)
                    {
                        vertexDescs[j].SemanticName = "POSITION";
                        vertexDescs[j].SemanticIndex = 0;
                    }
                    else if (attr.semantic == VertexSemantic::NORMAL)
                    {
                        vertexDescs[j].SemanticName = "NORMAL";
                        vertexDescs[j].SemanticIndex = 0;
                    }
                    else if (attr.semantic == VertexSemantic::TEXCOORD0)
                    {
                        vertexDescs[j].SemanticName = "TEXCOORD";
                        vertexDescs[j].SemanticIndex = 0;
                    }
                    else if (attr.semantic == VertexSemantic::TEXCOORD1)
                    {
                        vertexDescs[j].SemanticName = "TEXCOORD";
                        vertexDescs[j].SemanticIndex = 1;
                    }
                    else if (attr.semantic == VertexSemantic::TEXCOORD2)
                    {
                        vertexDescs[j].SemanticName = "TEXCOORD";
                        vertexDescs[j].SemanticIndex = 2;
                    }
                    vertexDescs[j].Format = GetVertexFormatD3D(attr.format);
                    vertexDescs[j].InputSlot = 0;
                    vertexDescs[j].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
                    vertexDescs[j].InstanceDataStepRate = 0;
                    vertexDescs[j].AlignedByteOffset = attr.offset;
                }
                HR(device->CreateInputLayout(vertexDescs.Buffer(), vertexDescs.Size(), shaders[i]->compiledBlob->GetBufferPointer(), shaders[i]->compiledBlob->GetBufferSize(), &inputLayout));
            }
        }
    }

    void ComputePipelineStateD3D11::Create(ID3D11Device* device, RenderAPID3D11* ctx, GPUComputePipelineStateCreateInfo& createInfo)
    {
        renderCtx = ctx;
        pipelineLayout = ctx->GetPipelineLayout(createInfo.pipelineLayoutHandle);
    }

    void ShaderD3D11::Create(ID3D11Device* device, GPUShaderCreateInfo& createInfo)
    {
        ID3D10Blob* compiledShader = (ID3D10Blob*)createInfo.blob;
        compiledBlob = compiledShader;
//        UINT flags = 0;
//#if defined( DEBUG ) || defined( _DEBUG )
//        flags |= D3DCOMPILE_DEBUG;
//#endif
//        String entryPoint = "CS";
//        String target = "cs_5_0";
//        if (createInfo.stage == ShaderStage::VS)
//        {
//            entryPoint = "VS";
//            target = "vs_5_0";
//        }
//        else if (createInfo.stage == ShaderStage::PS)
//        {
//            entryPoint = "PS";
//            target = "ps_5_0";
//        }
//        HR(D3DCompile(filePtr->GetBuffer(), filePtr->Size(), "TEST", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.CString(),
//            target.CString(), flags, 0, &compiledShader, &compilationMsgs));
        stage = (U32)createInfo.stage;
        if (createInfo.stage == ShaderStage::VS)
        {
            device->CreateVertexShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), NULL, &shaderPtr.vertexShader);
        }
        else if (createInfo.stage == ShaderStage::PS)
        {
            device->CreatePixelShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), NULL, &shaderPtr.pixelShader);
        }
        else if (createInfo.stage == ShaderStage::CS)
        {
            device->CreateComputeShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), NULL, &shaderPtr.computeShader);
        }
    }

    void CommandBufferD3D11::BeginRenderPass(RenderPassBeginInfo& beginInfo)
    {
        RenderPassD3D11* renderPass = renderContext->GetRenderPass(beginInfo.passHandle);
        FrameBufferD3D11* frameBuffer = renderContext->GetFrameBuffer(beginInfo.fbHandle);
        deviceContext->OMSetRenderTargets(frameBuffer->renderTargetViews.Size(), frameBuffer->renderTargetViews.Buffer(), frameBuffer->depthStencilView);
        for (U32 i = 0; i < beginInfo.numClearValues; ++i)
        {
            ClearValue& clear = beginInfo.clearValues[i];
            if (i == beginInfo.numClearValues - 1)
            {
                deviceContext->ClearDepthStencilView(frameBuffer->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, clear.depthStencil.depth, clear.depthStencil.stencil);
            }
            else
            {
                if (frameBuffer->renderTargetViews.Size() > i)
                {
                    deviceContext->ClearRenderTargetView(frameBuffer->renderTargetViews[i], clear.color.f32);
                }
            }
        }
    }

    void CommandBufferD3D11::BindGraphicsPipeline(GraphicsPipelineStateD3D11* pipeline)
    {
        float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        deviceContext->OMSetBlendState(pipeline->colorBlendState->blendState, blendFactor, 0xFFFFFFFF);
        deviceContext->OMSetDepthStencilState(pipeline->depthStencilState->depthStencilState, 0);
        deviceContext->RSSetState(pipeline->rasterState->rasterState);
        deviceContext->IASetInputLayout(pipeline->inputLayout);
        deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        for (U32 i = 0; i < pipeline->shaders.Size(); ++i)
        {
            if (pipeline->shaders[i]->stage == (U32)ShaderStage::VS)
            {
                deviceContext->VSSetShader(pipeline->shaders[i]->shaderPtr.vertexShader, 0, 0);
            }
            else if (pipeline->shaders[i]->stage == (U32)ShaderStage::PS)
            {
                deviceContext->PSSetShader(pipeline->shaders[i]->shaderPtr.pixelShader, 0, 0);
            }
        }
    }

    void CommandBufferD3D11::BindComputePipeline(ComputePipelineStateD3D11* pipeline)
    {
        deviceContext->CSSetShader(pipeline->shaders[0]->shaderPtr.computeShader, 0, 0);
    }

    void CommandBufferD3D11::BindVertexBuffer(VertexBufferD3D11* vb, U32 slot)
    {
        U32 offsets[]{ 0 };
        deviceContext->IASetVertexBuffers(slot, 1, &vb->buffer, &vb->size, offsets);
    }

    void CommandBufferD3D11::BindIndexBuffer(IndexBufferD3D11* ib)
    {
        deviceContext->IASetIndexBuffer(ib->buffer, ib->indexFormat, 0);
    }

    void CommandBufferD3D11::Draw(U32 vertCount)
    {
        deviceContext->Draw(vertCount, 0);
    }

    void CommandBufferD3D11::DrawIndexed(U32 indexCount, U32 indexStart, U32 vertStart)
    {
        deviceContext->DrawIndexed(indexCount, indexStart, vertStart);
    }

    void CommandBufferD3D11::SetViewport(const Viewport& vp)
    {
        D3D11_VIEWPORT viewport{};
        viewport.MaxDepth = vp.maxDepth;
        viewport.MinDepth = vp.minDepth;
        viewport.TopLeftX = vp.rect.x;
        viewport.TopLeftY = vp.rect.y;
        viewport.Width = vp.rect.width;
        viewport.Height = vp.rect.height;
        deviceContext->RSSetViewports(1, &viewport);
    }

    void CommandBufferD3D11::BindDescriptorSets(SoftwareDescriptorSets* sets, U32 setIndex)
    {
        PODVector<GPUDescriptorSetLayoutBinding>& setLayout = renderContext->GetDescriptorSetLayout(sets->layoutHandle);
        PODVector<GPUDescriptorSetsUpdateInfo::Entry>& updateSets = sets->updateInfo.updateSets;
        for (U32 i = 0; i < updateSets.Size(); ++i)
        {
            GPUDescriptorSetLayoutBinding binding;
            for (U32 j = 0; j < setLayout.Size(); ++j)
            {
                if (setLayout[j].binding == updateSets[i].binding)
                {
                    binding = setLayout[j];
                }
            }
            if (GPUResource::IsValid(updateSets[i].textureHandle))
            {
                TextureD3D11* tex = renderContext->GetTexture(updateSets[i].textureHandle);
                if (binding.stage & (U32)ShaderStage::VS)
                {
                    deviceContext->VSSetShaderResources(binding.binding, 1, &tex->imageView->imageView);
                    deviceContext->VSSetSamplers(binding.binding, 1, &tex->sampler->samplerState);
                }
                if (binding.stage & (U32)ShaderStage::PS)
                {
                    deviceContext->PSSetShaderResources(binding.binding, 1, &tex->imageView->imageView);
                    deviceContext->PSSetSamplers(binding.binding, 1, &tex->sampler->samplerState);
                }
                if (binding.stage & (U32)ShaderStage::CS)
                {
                    deviceContext->CSSetShaderResources(binding.binding, 1, &tex->imageView->imageView);
                    deviceContext->CSSetSamplers(binding.binding, 1, &tex->sampler->samplerState);
                }
            }
            else
            {
                UniformBufferD3D11* ub = renderContext->GetUniformBuffer(updateSets[i].uniformHandle);
                deviceContext->UpdateSubresource(ub->buffer, 0, NULL, ub->GetStagingData(), 0, 0);
                if (binding.stage & (U32)ShaderStage::VS)
                {
                    deviceContext->VSSetConstantBuffers(setIndex, 1, &ub->buffer);
                }
                if (binding.stage & (U32)ShaderStage::PS)
                {
                    deviceContext->PSSetConstantBuffers(setIndex, 1, &ub->buffer);
                }
                if (binding.stage & (U32)ShaderStage::CS)
                {
                    deviceContext->CSSetConstantBuffers(setIndex, 1, &ub->buffer);
                }
            }
        }
    }
}
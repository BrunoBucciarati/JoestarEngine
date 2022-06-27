#include "RenderAPID3D11.h"
#include "../../Misc/Application.h"
#include "../Window.h"
#include "../../IO/Log.h"
#include "../../Core/Assert.h"

#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }
static HRESULT hr;
#define HR(f) { \
    hr = f; \
    if (FAILED(hr)) \
        {LOGERROR("D3D FAILED: %s", #f);}\
}

#define GET_STRUCT_BY_HANDLE_FROM_VECTOR_ORIGIN(_VAR, _TYP, _HANDLE, _VEC) \
    if (_HANDLE + 1 > _VEC.Size()) _VEC.Resize(_HANDLE + 1); \
    _VEC[_HANDLE] = JOJO_NEW(_TYP, MEMORY_GFX_STRUCT); \
    _TYP& _VAR =  *(_VEC[_HANDLE]);

#define GET_STRUCT_BY_HANDLE_FROM_VECTOR(_VAR, _TYP, _HANDLE, _VEC) \
    GET_STRUCT_BY_HANDLE_FROM_VECTOR_ORIGIN(_VAR, _TYP##D3D11, _HANDLE, _VEC)

#define GET_STRUCT_BY_HANDLE(_VAR, _TYP, _HANDLE) \
    GET_STRUCT_BY_HANDLE_FROM_VECTOR(_VAR, _TYP, _HANDLE, m##_TYP##s);

namespace Joestar
{
    void RenderAPID3D11::CreateDevice()
    {
        Application* app = Application::GetApplication();
        Window* window = app->GetEngineContext()->GetSubsystem<Window>();
        UINT createDeviceFlags = 0;

#if  defined(DEBUG)||defined(_DEBUG)
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        D3D_FEATURE_LEVEL featureLevel;
        HRESULT hr = D3D11CreateDevice(
            0,                     //  默认显示适配器
            D3D_DRIVER_TYPE_HARDWARE,
            0,                     //  不使用软件设备
            createDeviceFlags,
            0, 0,               //  默认的特征等级数组
            D3D11_SDK_VERSION,
            &mDevice,
            &featureLevel,
            &mImmediateContext);
        if (FAILED(hr))
        {
            MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
            return;
        }
        if (featureLevel != D3D_FEATURE_LEVEL_11_0)
        {
            MessageBox(0, L"Direct3D FeatureLevel 11 unsupported.", 0, 0);
            return;
        }
    }


    void RenderAPID3D11::CreateSwapChain()
    {
        UINT  m4xMsaaQuality;
        HR(mDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality));
        bool mEnable4xMsaa = false;// m4xMsaaQuality > 0;

        DXGI_SWAP_CHAIN_DESC sd;
        sd.BufferDesc.Width = window->GetWidth();    // 使用窗口客户区宽度
        sd.BufferDesc.Height = window->GetHeight();
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        mSwapChain = JOJO_NEW(SwapChainD3D11, MEMORY_GFX_STRUCT);
        mSwapChain->width = window->GetWidth();
        mSwapChain->height = window->GetHeight();
        // 是否使用4X MSAA?
        if (mEnable4xMsaa)
        {
            sd.SampleDesc.Count = 4;
            // m4xMsaaQuality是通过CheckMultisampleQualityLevels()方法获得的
            sd.SampleDesc.Quality = m4xMsaaQuality - 1;
        }
        // NoMSAA
        else
        {
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
        }
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;
        sd.OutputWindow = window->GetMainWindow();
        sd.Windowed = true;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        sd.Flags = 0;

        IDXGIDevice* dxgiDevice = 0;
        HR(mDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));
        IDXGIAdapter* dxgiAdapter = 0;
        HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));
        // 获得IDXGIFactory 接口
        IDXGIFactory* dxgiFactory = 0;
        HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));
        // 现在，创建交换链
        HR(dxgiFactory->CreateSwapChain(mDevice, &sd, &mSwapChain->swapChain));
        // 释放COM接口
        ReleaseCOM(dxgiDevice);
        ReleaseCOM(dxgiAdapter);
        ReleaseCOM(dxgiFactory);

        GPUFrameBufferCreateInfo createInfo;
        createInfo.width = window->GetWidth();
        createInfo.height = window->GetHeight();
        CreateBackBuffers(createInfo);
    }

    void RenderAPID3D11::CreateBackBuffers(GPUFrameBufferCreateInfo& createInfo)
    {
        if (mSwapChain->frameBuffer)
            return;
        mSwapChain->frameBuffer = JOJO_NEW(FrameBufferD3D11, MEMORY_GFX_STRUCT);
        if (mFrameBuffers.Empty())
            mFrameBuffers.Resize(1);
        mFrameBuffers[0] = mSwapChain->frameBuffer;
        mFrameBuffers[0]->renderTargetViews.Resize(1);

        ID3D11Texture2D* backBuffer;
        mSwapChain->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
        mDevice->CreateRenderTargetView(backBuffer, 0, &mSwapChain->frameBuffer->renderTargetViews[0]);
        ReleaseCOM(backBuffer);

        D3D11_TEXTURE2D_DESC depthStencilDesc;
        depthStencilDesc.Width = createInfo.width;
        depthStencilDesc.Height = createInfo.height;
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.ArraySize = createInfo.layers;
        depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        // 使用4X MSAA?——必须与交换链的MSAA的值匹配
        if (mEnable4xMsaa)
        {
            depthStencilDesc.SampleDesc.Count = 4;
            depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
        }
        //  不使用MSAA
        else
        {
            depthStencilDesc.SampleDesc.Count = 1;
            depthStencilDesc.SampleDesc.Quality = 0;
        }
        depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
        depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthStencilDesc.CPUAccessFlags = 0;
        depthStencilDesc.MiscFlags = 0;
        ID3D11Texture2D* mDepthStencilBuffer;

        HR(mDevice->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer));

        HR(mDevice->CreateDepthStencilView(mDepthStencilBuffer, 0, &mSwapChain->frameBuffer->depthStencilView));
    }

    void RenderAPID3D11::CreateSyncObjects(U32 num)
    {

    }


    void RenderAPID3D11::CreateCommandPool(GPUResourceHandle handle, GPUQueue queue)
    {
        GET_STRUCT_BY_HANDLE_FROM_VECTOR_ORIGIN(pool, GPUQueue, handle, mCommandPools);
    }


    void RenderAPID3D11::CreateCommandBuffers(GPUResourceHandle handle, GPUCommandBufferCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE(cb, CommandBuffer, handle);
        cb.createInfo = createInfo;
        //暂时没有多线程
        cb.deviceContext = mImmediateContext;
        cb.renderContext = this;
    }

    void RenderAPID3D11::CreateImage(GPUResourceHandle handle, GPUImageCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE(image, Image, handle);
        image.Create(mDevice, createInfo);
    }

    void RenderAPID3D11::CreateImageView(GPUResourceHandle handle, GPUImageViewCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE(imageView, ImageView, handle);
        ImageD3D11* image = mImages[createInfo.imageHandle];
        imageView.Create(mDevice, image, createInfo);
    }

    void RenderAPID3D11::CreateSampler(GPUResourceHandle handle, GPUSamplerCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE(sampler, Sampler, handle);
        sampler.Create(mDevice, createInfo);
    }

    void RenderAPID3D11::CreateTexture(GPUResourceHandle handle, GPUTextureCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE(tex, Texture, handle);
        tex.imageView = mImageViews[createInfo.imageViewHandle];
        tex.sampler = mSamplers[createInfo.samplerHandle];
    }

    void RenderAPID3D11::CreateFrameBuffers(GPUResourceHandle handle, GPUFrameBufferCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE(fb, FrameBuffer, handle);
        fb.renderTargetViews.Resize(createInfo.numColorAttachments);

        for (U32 i = 0; i < createInfo.numColorAttachments; ++i)
        {
            ImageViewD3D11* view = mImageViews[createInfo.colorHandles[i]];
            D3D11_RENDER_TARGET_VIEW_DESC desc = view->GetRenderTargetViewDesc();
            mDevice->CreateRenderTargetView(view->GetImage()->GetTexture2D(), &desc, &fb.renderTargetViews[i]);
        }

        if (GPUResource::IsValid(createInfo.depthStencilHandle))
        {
            ImageViewD3D11* view = mImageViews[createInfo.depthStencilHandle];
            D3D11_DEPTH_STENCIL_VIEW_DESC desc = view->GetDepthStencilViewDesc();
            mDevice->CreateDepthStencilView(view->GetImage()->GetTexture2D(), &desc, &fb.depthStencilView);
        }
    }

    void RenderAPID3D11::CreateIndexBuffer(GPUResourceHandle handle, GPUIndexBufferCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE(ib, IndexBuffer, handle);
        GPUMemory& mem = mMemories[createInfo.memoryHandle];
        ib.Create(mDevice, createInfo, mem);
    }

    void RenderAPID3D11::CreateVertexBuffer(GPUResourceHandle handle, GPUVertexBufferCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE(vb, VertexBuffer, handle);
        GPUMemory& mem = mMemories[createInfo.memoryHandle];
        vb.Create(mDevice, createInfo, mem);
    }

    void RenderAPID3D11::CreateUniformBuffer(GPUResourceHandle handle, GPUUniformBufferCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE(ub, UniformBuffer, handle);
        ub.Create(mDevice, createInfo);
    }

    void RenderAPID3D11::CreateRenderPass(GPUResourceHandle handle, GPURenderPassCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE_FROM_VECTOR(pass, RenderPass, handle, mRenderPasses);
        pass.createInfo = createInfo;
    }

    void RenderAPID3D11::CreateDescriptorPool(U32 num)
    {

    }
    void RenderAPID3D11::CreateColorBlendState(GPUResourceHandle handle, GPUColorBlendStateCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE(state, ColorBlendState, handle);
        state.Create(mDevice, createInfo);
    }
    void RenderAPID3D11::CreateDepthStencilState(GPUResourceHandle handle, GPUDepthStencilStateCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE(state, DepthStencilState, handle);
        state.Create(mDevice, createInfo);
    }
    void RenderAPID3D11::CreateRasterizationState(GPUResourceHandle handle, GPURasterizationStateCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE(state, RasterizationState, handle);
        state.Create(mDevice, createInfo);
    }
    void RenderAPID3D11::SetUniformBuffer(GPUResourceHandle handle, U8* data, U32 size)
    {
        UniformBufferD3D11* ub = mUniformBuffers[handle];
        ub->SetStagingData(data, size);
    }
    void RenderAPID3D11::CreateShader(GPUResourceHandle handle, GPUShaderCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE(shader, Shader, handle);
        shader.Create(mDevice, createInfo);
    }
    void RenderAPID3D11::CreateGraphicsPipelineState(GPUResourceHandle handle, GPUGraphicsPipelineStateCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE(state, GraphicsPipelineState, handle);
        state.Create(mDevice, this, createInfo);
    }
    void RenderAPID3D11::CreateComputePipelineState(GPUResourceHandle handle, GPUComputePipelineStateCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE(state, ComputePipelineState, handle);
        state.Create(mDevice, this, createInfo);
    }
    void RenderAPID3D11::CBBeginRenderPass(GPUResourceHandle handle, RenderPassBeginInfo& beginInfo)
    {
        CommandBufferD3D11* cb = mCommandBuffers[handle];
        cb->BeginRenderPass(beginInfo);
    }
    void RenderAPID3D11::CBEndRenderPass(GPUResourceHandle handle, GPUResourceHandle passHandle)
    {}
    void RenderAPID3D11::CBBindGraphicsPipeline(GPUResourceHandle handle, GPUResourceHandle psoHandle)
    {
        CommandBufferD3D11* cb = mCommandBuffers[handle];
        GraphicsPipelineStateD3D11* pso = mGraphicsPipelineStates[psoHandle];
        cb->BindGraphicsPipeline(pso);
    }
    void RenderAPID3D11::CBBindComputePipeline(GPUResourceHandle handle, GPUResourceHandle psoHandle)
    {
        CommandBufferD3D11* cb = mCommandBuffers[handle];
        ComputePipelineStateD3D11* pso = mComputePipelineStates[psoHandle];
        cb->BindComputePipeline(pso);
    }
    void RenderAPID3D11::CBBindIndexBuffer(GPUResourceHandle handle, GPUResourceHandle ibHandle)
    {
        CommandBufferD3D11* cb = mCommandBuffers[handle];
        IndexBufferD3D11* ib = mIndexBuffers[ibHandle];
        cb->BindIndexBuffer(ib);
    }
    void RenderAPID3D11::CBBindVertexBuffer(GPUResourceHandle handle, GPUResourceHandle vbHandle, U32 slot)
    {
        CommandBufferD3D11* cb = mCommandBuffers[handle];
        VertexBufferD3D11* vb = mVertexBuffers[vbHandle];
        cb->BindVertexBuffer(vb, slot);
    }

    void RenderAPID3D11::CBBindDescriptorSets(GPUResourceHandle handle, GPUResourceHandle layoutHandle, GPUResourceHandle setsHandle, U32 sets)
    {
        CommandBufferD3D11* cb = mCommandBuffers[handle];
        SoftwareDescriptorSets* descriptorSets = mDescriptorSets[setsHandle];
        cb->BindDescriptorSets(descriptorSets, sets);
    }
    void RenderAPID3D11::CBDraw(GPUResourceHandle handle, U32 count)
    {
        CommandBufferD3D11* cb = mCommandBuffers[handle];
        cb->Draw(count);
    }
    void RenderAPID3D11::CBDrawIndexed(GPUResourceHandle handle, U32 count, U32 indexStart, U32 vertStart)
    {
        CommandBufferD3D11* cb = mCommandBuffers[handle];
        cb->DrawIndexed(count, indexStart, vertStart);
    }
    void RenderAPID3D11::CBSetViewport(GPUResourceHandle handle, const Viewport& vp)
    {
        CommandBufferD3D11* cb = mCommandBuffers[handle];
        cb->SetViewport(vp);
    }
    void RenderAPID3D11::QueueSubmit(GPUResourceHandle handle)
    {

    }
    void RenderAPID3D11::Present()
    {
        mSwapChain->swapChain->Present(0, 0);
    }
}
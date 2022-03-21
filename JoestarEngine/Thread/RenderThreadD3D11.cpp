#include "RenderThreadD3D11.h"
#include "../Misc/Application.h"
#include "../Misc/GlobalConfig.h"
#include "../IO/FileSystem.h"
#include "../Graphics/Window.h"
#include <d3dcompiler.h>

#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }

namespace Joestar {
    float vertices[] =
    {
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
        -1.0f, +1.0f, +1.0f, 0.0f, 1.0f,
        +1.0f, +1.0f, +1.0f, 1.0f, 1.0f,
        +1.0f, -1.0f, +1.0f, 1.0f, 0.0f,
    };


    UINT indices[24] = {
        0, 1, 2,   //  Triangle  0
        0, 2, 3,   //  Triangle  1
        0, 3, 4,   //  Triangle  2
        0, 4, 5,   //  Triangle  3
        0, 5, 6,   //  Triangle  4
        0, 6, 7,   //  Triangle  5
        0, 7, 8,   //  Triangle  6
        0, 8, 1    //  Triangle  7
    };
	RenderThreadD3D11::RenderThreadD3D11(Vector<GFXCommandBuffer*>& cmdBuffers, Vector<GFXCommandBuffer*>& computeBuffers) :
        RenderThread(cmdBuffers, computeBuffers)
	{
	}


	bool RenderThreadD3D11::InitRenderContext()
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
            &md3dDevice,
            &featureLevel,
            &md3dImmediateContext);
        if (FAILED(hr))
        {
            MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
            return  false;
        }
        if (featureLevel != D3D_FEATURE_LEVEL_11_0)
        {
            MessageBox(0, L"Direct3D FeatureLevel 11 unsupported.", 0, 0);
            return  false;
        }

        UINT  m4xMsaaQuality;
        hr = md3dDevice->CheckMultisampleQualityLevels(
            DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality);
        bool mEnable4xMsaa = m4xMsaaQuality > 0;

        DXGI_SWAP_CHAIN_DESC sd;
        sd.BufferDesc.Width = 800;    // 使用窗口客户区宽度
        sd.BufferDesc.Height = 600;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
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
        hr = md3dDevice->QueryInterface(__uuidof(IDXGIDevice),
            (void**)&dxgiDevice);
        IDXGIAdapter* dxgiAdapter = 0;
        hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter),
            (void**)&dxgiAdapter);
        // 获得IDXGIFactory 接口
        IDXGIFactory* dxgiFactory = 0;
        hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory),
            (void**)&dxgiFactory);
        // 现在，创建交换链
        hr = dxgiFactory->CreateSwapChain(md3dDevice, &sd, &mSwapChain);
        // 释放COM接口
        ReleaseCOM(dxgiDevice);
        ReleaseCOM(dxgiAdapter);
        ReleaseCOM(dxgiFactory);

        ID3D11Texture2D* backBuffer;
        mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
            reinterpret_cast<void**>(&backBuffer));
        md3dDevice->CreateRenderTargetView(backBuffer, 0, &mRenderTargetView);
        ReleaseCOM(backBuffer);

        D3D11_TEXTURE2D_DESC depthStencilDesc;
        depthStencilDesc.Width = 800;
        depthStencilDesc.Height = 600;
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.ArraySize = 1;
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

        hr = md3dDevice->CreateTexture2D(
            &depthStencilDesc, 0, &mDepthStencilBuffer);

        hr = md3dDevice->CreateDepthStencilView(
            mDepthStencilBuffer, 0, &mDepthStencilView);

        md3dImmediateContext->OMSetRenderTargets(
            1, &mRenderTargetView, mDepthStencilView);

        D3D11_VIEWPORT vp;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        vp.Width = static_cast<float>(800);
        vp.Height = static_cast<float>(600);
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        md3dImmediateContext->RSSetViewports(1, &vp);

        D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
        {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
                D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12,
                D3D11_INPUT_PER_VERTEX_DATA, 0}
        };
        D3D11_BUFFER_DESC vbd;
        vbd.Usage = D3D11_USAGE_IMMUTABLE;
        vbd.ByteWidth = sizeof(float) * 5 * 8;
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.CPUAccessFlags = 0;
        vbd.MiscFlags = 0;
        vbd.StructureByteStride = 0;
        D3D11_SUBRESOURCE_DATA vinitData;
        ZeroMemory(&vinitData, sizeof(D3D11_SUBRESOURCE_DATA));
        vinitData.pSysMem = vertices;
        hr = md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB);

        D3D11_BUFFER_DESC ibd;
        ibd.Usage = D3D11_USAGE_IMMUTABLE;
        ibd.ByteWidth = sizeof(UINT) * 24;
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.CPUAccessFlags = 0;
        ibd.MiscFlags = 0;
        ibd.StructureByteStride = 0;
        D3D11_SUBRESOURCE_DATA iinitData;
        ZeroMemory(&iinitData, sizeof(D3D11_SUBRESOURCE_DATA));
        iinitData.pSysMem = indices;
        hr = md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB);

        D3D11_RASTERIZER_DESC rsDesc;
        ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
        rsDesc.FillMode = D3D11_FILL_SOLID;
        rsDesc.CullMode = D3D11_CULL_NONE;
        rsDesc.FrontCounterClockwise = false;
        rsDesc.DepthClipEnable = true;

        hr = md3dDevice->CreateRasterizerState(&rsDesc, &mRS);

        DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
        shaderFlags |= D3D10_SHADER_DEBUG;
        shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

        FileSystem* fs = app->GetSubSystem<FileSystem>();
        std::string path = fs->GetResourceDir();
        path += "Shaders/hlsl/test.fx";
        File* file = fs->ReadFile(path.c_str());

        ID3D10Blob* compiledVSShader = 0;
        ID3D10Blob* compilationVSMsgs = 0;
        UINT flags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
        flags |= D3DCOMPILE_DEBUG;
#endif
        hr = D3DCompile(file->GetBuffer(), file->Size(), "TEST", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS",
            "vs_5_0", flags, 0, &compiledVSShader, &compilationVSMsgs);


        // compilationMsgs中包含错误或警告信息
        if (compilationVSMsgs != 0)
        {
            MessageBoxA(0, (char*)compilationVSMsgs->GetBufferPointer(), 0, 0);
            ReleaseCOM(compilationVSMsgs);
        }
        md3dDevice->CreateVertexShader(compiledVSShader->GetBufferPointer(), compiledVSShader->GetBufferSize(), NULL, &vs);

        ID3D10Blob* compiledPSShader = 0;
        ID3D10Blob* compilationPSMsgs = 0;
        hr = D3DCompile(file->GetBuffer(), file->Size(), "TEST", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS",
            "ps_5_0", flags, 0, &compiledPSShader, &compilationPSMsgs);

        // compilationMsgs中包含错误或警告信息
        if (compilationPSMsgs != 0)
        {
            MessageBoxA(0, (char*)compilationPSMsgs->GetBufferPointer(), 0, 0);
            ReleaseCOM(compilationPSMsgs);
        }

        md3dDevice->CreatePixelShader(compiledPSShader->GetBufferPointer(), compiledPSShader->GetBufferSize(), NULL, &ps);

        hr = md3dDevice->CreateInputLayout(vertexDesc, 2, compiledVSShader->GetBufferPointer(), compiledVSShader->GetBufferSize(), &mInputLayout);
        // 就算没有compilationMsgs，也需要确保没有其他错误
        if (FAILED(hr))
        {
        }

        //第六,设置(常量)缓存形容结构体,并创建常量缓存
        D3D11_BUFFER_DESC matrixBufferDesc;
        ZeroMemory(&matrixBufferDesc, sizeof(matrixBufferDesc));
        matrixBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        matrixBufferDesc.ByteWidth = sizeof(Matrix4x4f);   //结构体大小,必须为16字节倍数
        matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        matrixBufferDesc.CPUAccessFlags = 0;

        hr = md3dDevice->CreateBuffer(&matrixBufferDesc, NULL, &mCB);

        String texpath = "Textures/marble.jpg";
        Texture2D* tex2D = new Texture2D(app->GetEngineContext());
        tex2D->TextureFromImage(texpath);

        D3D11_TEXTURE2D_DESC tdesc;
        ZeroMemory(&tdesc, sizeof(tdesc));
        tdesc.Width = tex2D->GetWidth();
        tdesc.Height = tex2D->GetHeight();
        tdesc.MipLevels = 1;
        tdesc.ArraySize = 1;
        tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        tdesc.SampleDesc.Count = 1;		// 不使用多重采样
        tdesc.SampleDesc.Quality = 0;
        tdesc.Usage = D3D11_USAGE_DEFAULT;
        tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        tdesc.CPUAccessFlags = 0;
        tdesc.MiscFlags = 0;	// 指定需要生成mipmap
        D3D11_SUBRESOURCE_DATA tinitdata;
        ZeroMemory(&tinitdata, sizeof(tinitdata));
        tinitdata.pSysMem = tex2D->GetData();
        tinitdata.SysMemPitch = tex2D->GetWidth() * 4;
        md3dDevice->CreateTexture2D(&tdesc, &tinitdata, &mDiffTex);

        D3D11_SHADER_RESOURCE_VIEW_DESC vdesc;
        ZeroMemory(&vdesc, sizeof(vdesc));
        vdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        vdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        vdesc.Texture2D.MostDetailedMip = 0;
        vdesc.Texture2D.MipLevels = 1;
        hr = md3dDevice->CreateShaderResourceView(mDiffTex, &vdesc, &mDiffSRV);

        D3D11_SAMPLER_DESC sampDesc;
        ZeroMemory(&sampDesc, sizeof(sampDesc));
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;//过滤器
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;//寻址模式
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
        // 创建采样器状态
        //md3dDevice->CreateSamplerState(&sampDesc, m_pSamplerState.GetAddressOf());
        md3dDevice->CreateSamplerState(&sampDesc, &mSampleState);

        D3D11_BLEND_DESC blendDesc = { 0 };
        blendDesc.AlphaToCoverageEnable = false;
        blendDesc.IndependentBlendEnable = false;

        blendDesc.RenderTarget[0].BlendEnable = true;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        hr = md3dDevice->CreateBlendState(&blendDesc, &mBlendState);

        D3D11_DEPTH_STENCIL_DESC dsDesc{0};
        dsDesc.DepthEnable = true;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
        dsDesc.StencilEnable = true;
        dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
        dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
        dsDesc.BackFace = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP , D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS };
        dsDesc.FrontFace = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP , D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS };
        hr = md3dDevice->CreateDepthStencilState(&dsDesc, &mDepthStencilState);

	}

    void RenderThreadD3D11::PrepareCompute()
    {
        D3D11_TEXTURE2D_DESC blurredTexDesc;
        blurredTexDesc.Width = 512;
        blurredTexDesc.Height = 512;
        blurredTexDesc.MipLevels = 1;
        blurredTexDesc.ArraySize = 1;
        blurredTexDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        blurredTexDesc.SampleDesc.Count = 1;
        blurredTexDesc.SampleDesc.Quality = 0;
        blurredTexDesc.Usage = D3D11_USAGE_DEFAULT;
        blurredTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        blurredTexDesc.CPUAccessFlags = 0;
        blurredTexDesc.MiscFlags = 0;
        ID3D11Texture2D* blurredTex = 0;
        HRESULT hr = md3dDevice->CreateTexture2D(&blurredTexDesc, 0, &blurredTex);

        D3D11_SHADER_RESOURCE_VIEW_DESC bsrvDesc;
        bsrvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        bsrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        bsrvDesc.Texture2D.MostDetailedMip = 0;
        bsrvDesc.Texture2D.MipLevels = 1;
        ID3D11ShaderResourceView* mBlurredOutputTexSRV;
        hr = md3dDevice->CreateShaderResourceView(blurredTex, &bsrvDesc, &mBlurredOutputTexSRV);

        D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
        uavDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
        uavDesc.Texture2D.MipSlice = 0;
        ID3D11UnorderedAccessView* mBlurredOutputTexUAV;
        hr = md3dDevice->CreateUnorderedAccessView(blurredTex, &uavDesc, &mBlurredOutputTexUAV);
        // Views save a reference to the texture so we can release our reference.
        ReleaseCOM(blurredTex);

        int numElements = 1024;
        Vector<float> dataA;
        dataA.Resize(1024 * 3);
        for (auto& f : dataA)
            f = 0;

        D3D11_BUFFER_DESC inputDesc;
        inputDesc.Usage = D3D11_USAGE_DEFAULT;
        inputDesc.ByteWidth = sizeof(float) * 3 * numElements;
        inputDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        inputDesc.CPUAccessFlags = 0;
        inputDesc.StructureByteStride = sizeof(float) * 3;
        inputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

        D3D11_SUBRESOURCE_DATA vinitDataA;
        vinitDataA.pSysMem = &dataA[0];
        hr = md3dDevice->CreateBuffer(&inputDesc, &vinitDataA, &mInputBuffer);

        D3D11_BUFFER_DESC outputDesc;
        outputDesc.Usage = D3D11_USAGE_DEFAULT;
        outputDesc.ByteWidth = sizeof(float) * 3 * numElements;
        outputDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        outputDesc.CPUAccessFlags = 0;
        outputDesc.StructureByteStride = sizeof(float) * 3;
        outputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        hr = md3dDevice->CreateBuffer(&outputDesc, 0, &mOutputBuffer);

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
        srvDesc.BufferEx.FirstElement = 0;
        srvDesc.BufferEx.Flags = 0;
        srvDesc.BufferEx.NumElements = numElements;
        md3dDevice->CreateShaderResourceView(mInputBuffer, &srvDesc, &mInputASRV);
        //ID3D11ShaderResourceView* mInputBSRV;
        //md3dDevice->CreateShaderResourceView(, &srvDesc, &mInputBSRV);
        D3D11_UNORDERED_ACCESS_VIEW_DESC buavDesc;
        buavDesc.Format = DXGI_FORMAT_UNKNOWN;
        buavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        buavDesc.Buffer.FirstElement = 0;
        buavDesc.Buffer.Flags = 0;
        buavDesc.Buffer.NumElements = numElements;
        md3dDevice->CreateUnorderedAccessView(mOutputBuffer, &buavDesc, &mOutputUAV);

        D3D11_BUFFER_DESC outputDDesc;
        outputDDesc.Usage = D3D11_USAGE_STAGING;
        outputDDesc.BindFlags = 0;
        outputDDesc.ByteWidth = sizeof(float) * 3 * numElements;
        outputDDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        outputDDesc.StructureByteStride = sizeof(float) * 3;
        outputDDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        hr = md3dDevice->CreateBuffer(&outputDDesc, 0, &mOutputDebugBuffer);


        UINT flags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
        flags |= D3DCOMPILE_DEBUG;
#endif
        Application* app = Application::GetApplication();
        FileSystem* fs = app->GetSubSystem<FileSystem>();
        std::string path = fs->GetResourceDir();
        path += "Shaders/hlsl/cstest.fx";
        File* file = fs->ReadFile(path.c_str());
        ID3D10Blob* compiledCSShader = 0;
        ID3D10Blob* compilationCSMsgs = 0;
        hr = D3DCompile(file->GetBuffer(), file->Size(), "TEST", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "CS",
            "cs_5_0", flags, 0, &compiledCSShader, &compilationCSMsgs);

        // compilationMsgs中包含错误或警告信息
        if (compilationCSMsgs != 0)
        {
            MessageBoxA(0, (char*)compilationCSMsgs->GetBufferPointer(), 0, 0);
            ReleaseCOM(compilationCSMsgs);
        }

        md3dDevice->CreateComputeShader(compiledCSShader->GetBufferPointer(), compiledCSShader->GetBufferSize(), NULL, &cs);
    }

    void RenderThreadD3D11::ThreadFunc()
    {
        if (!bInit) {
            InitRenderContext();
            PrepareCompute();
            bInit = true;
            frameIndex = 0;
        }

        while (!bExit) {
            U32 idx = frameIndex % MAX_CMDBUFFERS_IN_FLIGHT;
            //always dispatch compute first, then draw
            while (!computeCmdBuffers[idx]->ready || !cmdBuffers[idx]->ready) {
                //busy wait
            }
            DispatchCompute();
            DrawScene();
            cmdBuffers[idx]->ready = false;
            computeCmdBuffers[idx]->ready = false;
            ++frameIndex;
        }
    }

    void RenderThreadD3D11::DispatchCompute()
    {
        md3dImmediateContext->CSSetShader(cs, NULL, 0);
        md3dImmediateContext->CSSetShaderResources(0, 1, &mInputASRV);
        md3dImmediateContext->CSSetUnorderedAccessViews(0, 1, &mOutputUAV, NULL);
        md3dImmediateContext->Dispatch(1024/32, 1, 1);
        // ...
        //
        // Compute shader finished!
        // Copy the output buffer to system memory.
        md3dImmediateContext->CopyResource(mOutputDebugBuffer, mOutputBuffer);
        // Map the data for reading.
        D3D11_MAPPED_SUBRESOURCE mappedData;
        md3dImmediateContext->Map(mOutputDebugBuffer, 0, D3D11_MAP_READ, 0, &mappedData);
        float* dataView = reinterpret_cast<float*>(mappedData.pData);
        std::vector<float> results;
        results.resize(1024 * 3);
        for (int i = 0; i < 1024; ++i)
        {
            results[i] = dataView[i];
        }
        md3dImmediateContext->Unmap(mOutputDebugBuffer, 0);
    }


    void RenderThreadD3D11::DrawScene() {
        const float backColor[]{ 0.0f, 0.0f, 1.0f, 0.0 };
        md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, backColor);
        md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        md3dImmediateContext->IASetInputLayout(mInputLayout);
        md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        UINT stride = sizeof(float) * 5;
        UINT offset = 0;
        md3dImmediateContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
        md3dImmediateContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
        md3dImmediateContext->RSSetState(mRS);

        md3dImmediateContext->VSSetShader(vs, NULL, 0);
        md3dImmediateContext->PSSetShader(ps, NULL, 0);

        //CBMatrix* cbPtr;
        unsigned int bufferNum;

        //将矩阵转置,在传入常量缓存前进行转置,因为GPU对矩阵数据会自动进行一次转置
        Matrix4x4f mat = Matrix4x4f::identity;
        md3dImmediateContext->UpdateSubresource(mCB, 0, NULL, &mat, 0, 0);
        bufferNum = 0;

        md3dImmediateContext->VSSetConstantBuffers(bufferNum, 1, &mCB);
        md3dImmediateContext->PSSetShaderResources(bufferNum, 1, &mDiffSRV);
        md3dImmediateContext->PSSetSamplers(bufferNum, 1, &mSampleState);

        float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        md3dImmediateContext->OMSetBlendState(mBlendState, blendFactor, 0xffffffff);

        md3dImmediateContext->OMSetDepthStencilState(mDepthStencilState, 0);

        md3dImmediateContext->DrawIndexed(24, 0, 0);

        mSwapChain->Present(0, 0);
    }
}
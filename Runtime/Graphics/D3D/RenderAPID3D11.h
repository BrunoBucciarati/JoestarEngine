#pragma once
#include "../RenderAPIProtocol.h"
#include "../../Platform/Platform.h"
#include "../../Container/Vector.h"
#include "RenderStructsD3D11.h"

namespace Joestar
{
	class RenderAPID3D11 : public RenderAPIProtocol
	{
	public:
		void CreateDevice() override;
		void CreateSwapChain() override;
		void CreateSyncObjects(U32 num = 1) override;
		void CreateCommandPool(GPUResourceHandle handle, GPUQueue queue) override;
		void CreateCommandBuffers(GPUResourceHandle handle, GPUCommandBufferCreateInfo& createInfo) override;
		void CreateFrameBuffers(GPUResourceHandle handle, GPUFrameBufferCreateInfo& createInfo) override;
		void CreateBackBuffers(GPUFrameBufferCreateInfo& createInfo) override;
		void CreateImage(GPUResourceHandle handle, GPUImageCreateInfo& createInfo) override;
		void CreateImageView(GPUResourceHandle handle, GPUImageViewCreateInfo& createInfo) override;
		void CreateSampler(GPUResourceHandle handle, GPUSamplerCreateInfo& createInfo) override;
		void CreateTexture(GPUResourceHandle handle, GPUTextureCreateInfo& createInfo) override;
		void CreateIndexBuffer(GPUResourceHandle handle, GPUIndexBufferCreateInfo& createInfo) override;
		void CreateVertexBuffer(GPUResourceHandle handle, GPUVertexBufferCreateInfo& createInfo) override;
		void CreateUniformBuffer(GPUResourceHandle handle, GPUUniformBufferCreateInfo& createInfo) override;
		void SetUniformBuffer(GPUResourceHandle handle, U8* data, U32 size) override;
		void CreateRenderPass(GPUResourceHandle handle, GPURenderPassCreateInfo& createInfo) override;
		void CreateDescriptorPool(U32 num = 1) override;
		void CreateShader(GPUResourceHandle handle, GPUShaderCreateInfo& createInfo) override;
		void CreateGraphicsPipelineState(GPUResourceHandle handle, GPUGraphicsPipelineStateCreateInfo& createInfo) override;
		void CreateComputePipelineState(GPUResourceHandle handle, GPUComputePipelineStateCreateInfo& createInfo) override;
		//virtual void CreatePipelineLayout(GPUResourceHandle handle, GPUPipelineLayoutCreateInfo& createInfo) = 0;
		//virtual void CreateDescriptorSetLayout(GPUResourceHandle handle, PODVector<GPUDescriptorSetLayoutBinding>& bindings) {};
		//virtual void CreateDescriptorSets(GPUResourceHandle handle, GPUDescriptorSetsCreateInfo& createInfo) = 0;
		//virtual void UpdateDescriptorSets(GPUResourceHandle handle, GPUDescriptorSetsUpdateInfo& updateInfo) = 0;

		//override for D3D
		void CreateColorBlendState(GPUResourceHandle handle, GPUColorBlendStateCreateInfo& createInfo) override;
		void CreateDepthStencilState(GPUResourceHandle handle, GPUDepthStencilStateCreateInfo& createInfo) override;
		void CreateRasterizationState(GPUResourceHandle handle, GPURasterizationStateCreateInfo& createInfo) override;

		DepthStencilStateD3D11* GetDepthStencilState(GPUResourceHandle handle)
		{
			return mDepthStencilStates[handle];
		}

		ColorBlendStateD3D11* GetColorBlendState(GPUResourceHandle handle)
		{
			return mColorBlendStates[handle];
		}

		RasterizationStateD3D11* GetRasterizationState(GPUResourceHandle handle)
		{
			return mRasterizationStates[handle];
		}

		VertexBufferD3D11* GetVertexBuffer(GPUResourceHandle handle)
		{
			return mVertexBuffers[handle];
		}

		ShaderD3D11* GetShader(GPUResourceHandle handle)
		{
			return mShaders[handle];
		}
	private:
		bool      mAppPaused;    // �����Ƿ�����ͣ״̬
		bool      mMinimized;    // �����Ƿ���С��
		bool      mMaximized;    // �����Ƿ����
		bool      mResizing;     // �����Ƿ��ڸı��С��״̬
		UINT      m4xMsaaQuality;// 4X MSAA�����ȼ�

		//  D3D11�豸(��4.2.1)��������(��4.2.4)���������/ģ�建���2D����(��4.2.6)��
		//  ��ȾĿ��(��4.2.5)�����/ģ����ͼ(��4.2.6)�����ӿ�(��4.2.8)��
		ID3D11Device* mDevice;
		ID3D11DeviceContext* mImmediateContext;
		SwapChainD3D11* mSwapChain;
		Vector<FrameBufferD3D11*> mFrameBuffers;
		Vector<GPUQueue*> mCommandPools;
		Vector<CommandBufferD3D11*> mCommandBuffers;
		Vector<ImageD3D11*> mImages;
		Vector<ImageViewD3D11*> mImageViews;
		Vector<SamplerD3D11*> mSamplers;
		Vector<TextureD3D11*> mTextures;
		Vector<IndexBufferD3D11*> mIndexBuffers;
		Vector<VertexBufferD3D11*> mVertexBuffers;
		Vector<UniformBufferD3D11*> mUniformBuffers;
		Vector<RenderPassD3D11*> mRenderPasses;
		Vector<ColorBlendStateD3D11*> mColorBlendStates;
		Vector<DepthStencilStateD3D11*> mDepthStencilStates;
		Vector<RasterizationStateD3D11*> mRasterizationStates;
		Vector<GraphicsPipelineStateD3D11*> mGraphicsPipelineStates;
		Vector<ComputePipelineStateD3D11*> mComputePipelineStates;
		Vector<ShaderD3D11*> mShaders;
		D3D11_VIEWPORT mScreenViewport;

		//  ����ı�������D3DApp���캯�������õġ����ǣ������������������д��Щֵ��

		//  ���ڱ��⡣D3DApp��Ĭ�ϱ�����"D3D11 Application"��
		//std::wstring mMainWndCaption;

		//  Hardware device����reference device��D3DAppĬ��ʹ��D3D_DRIVER_TYPE_HARDWARE��
		D3D_DRIVER_TYPE md3dDriverType;
		//  ����Ϊtrue��ʹ��4XMSAA(��4.1.8)��Ĭ��Ϊfalse��
		bool mEnable4xMsaa;

		ID3D11RasterizerState* mRS;
		ID3D11InputLayout* mInputLayout;
		ID3D11VertexShader* vs;
		ID3D11PixelShader* ps;
		ID3D11Texture2D* mDiffTex;
		ID3D11ShaderResourceView* mDiffSRV;
		ID3D11SamplerState* mSampleState;
		ID3D11BlendState* mBlendState;
		ID3D11DepthStencilState* mDepthStencilState;

		//compute
		ID3D11Buffer* mInputBuffer;
		ID3D11Buffer* mOutputBuffer;
		ID3D11Buffer* mOutputDebugBuffer;
		ID3D11ShaderResourceView* mInputASRV;
		ID3D11UnorderedAccessView* mOutputUAV;
		ID3D11ComputeShader* cs;
	};
}

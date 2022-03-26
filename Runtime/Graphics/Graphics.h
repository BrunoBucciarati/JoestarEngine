#pragma once
#include "Mesh.h"
#include "../Thread/RenderThread.h"
#include "../Core/SubSystem.h"
#include "../Core/EngineContext.h"
#include "RenderCommand.h"
#include "../Math/Vector4.h"
#include "GraphicDefines.h"
#include "VertexData.h"
#include "Material.h"
#include "../Math/Matrix4x4.h"
#include "Shader/Shader.h"
#include "ProgramCPU.h"
#include "UniformData.h"
#include "FrameBufferDef.h"
#include "CommandBuffer.h"
#include "PipelineState.h"
#include "GPUResource.h"
#include "FrameBuffer.h"
namespace Joestar {
	class VertexBuffer;
	class IndexBuffer;
	class GFXCommandList;
	class CommandBuffer;
	class FrameBuffer;
	class SwapChain;
	class Shader;
	//class GPUImage;
	//class GPUImageView;
	//class GPUMemory;
	//class GPUVertexBuffer;
	//class GPUIndexBuffer;
	//class RenderPass;
	//class PipelineState;
	//class ComputePipelineState;
	//class GraphicsPipelineState;

	class Graphics : public SubSystem {
		REGISTER_SUBSYSTEM(Graphics)
	public:
		explicit Graphics(EngineContext* context);
		void Init();
		virtual void DrawTriangle() {}
		void MainLoop();
		//void DrawM esh(Mesh* mesh, Material* mat);
		void Clear();
		void UpdateBuiltinMatrix(BUILTIN_VALUE typ, Matrix4x4f& mat);
		void UpdateBuiltinVec3(BUILTIN_VALUE typ, Vector3f& mat);
		void FlushUniformBuffer(U32 hash = 0);
		void FlushUniformBuffer(const char*);
		void UpdateLightBlock(LightBlocks& lb);
		//void UpdateVertexBuffer(VertexBuffer* vb);
		//void UpdateIndexBuffer(IndexBuffer* ib);
		void UpdateMaterial(Material* mat);
		void UseShader(Shader* shader);
		void UpdateTexture(Texture*, U8 binding = 0);
		void UpdateProgram(ProgramCPU* p);
		void BeginRenderPass(String name);
		void EndRenderPass(String name);
		void SetDepthCompare(DepthCompareFunc fun);
		void SetPolygonMode(PolygonMode fun);
		//void UpdateInstanceBuffer(InstanceBuffer* ib);
		void SetFrameBuffer(FrameBufferDef* def);

		void DispatchCompute(U32 group[3]);
		void BeginCompute(const char* name);
		void EndCompute(const char* name);
		//void DrawMeshInstanced(Mesh* mesh, Material* mat, InstanceBuffer* ib);
		//void UpdateComputeBuffer(ComputeBuffer* cb, U8 binding = 0);
		void UpdatePushConstant(void* data, U32 size);
		void WriteBackComputeBuffer();

		//NEW GRAPHICS
		CommandBuffer* GetMainCommandBuffer();
		CommandBuffer* CreateCommandBuffer();

		FrameBuffer* GetBackBuffer();
		FrameBuffer* CreateFrameBuffer();
		void CreateDescriptorPool();

		void CreateImage(GPUImage*, U32);
		void CreateImageView(GPUImageView*, U32);
		GPUMemory* CreateGPUMemory();
		void SetGPUMemory(GPUMemory*);
		GPUVertexBuffer* CreateGPUVertexBuffer(VertexBuffer*);
		void RemoveGPUVertexBuffer(GPUVertexBuffer* vb);
		GPUIndexBuffer* CreateGPUIndexBuffer(IndexBuffer*);
		void RemoveGPUIndexBuffer(GPUIndexBuffer* vb);
		GPUUniformBuffer* CreateGPUUniformBuffer(U32 hash, const UniformType& type);
		GPUUniformBuffer* CreateGPUUniformBuffer(const String& name,const UniformType& type);
		void CreateRenderPass(RenderPass*);
		void CreateGraphicsPipelineState(GraphicsPipelineState*);
		RenderPass* GetMainRenderPass();

		void CreateSwapChain();
		void CreateSyncObjects();
		void CreateShader(Shader* shader);
		GFXCommandList* GetMainCmdList();

		RasterizationState* GetDefaultRasterizationState()
		{
			return mRasterizationStates[0];
		}
		MultiSampleState* GetDefaultMultiSampleState()
		{
			return mMultiSampleStates[0];
		}
		DepthStencilState* GetDefaultDepthStencilState()
		{
			return mDepthStencilStates[0];
		}
		ColorBlendState* GetDefaultColorBlendState()
		{
			return mColorBlendStates[0];
		}
		void CreateRasterizationState(RasterizationState*);
		void CreateMultiSampleState(MultiSampleState*);
		void CreateDepthStencilState(DepthStencilState*);
		void CreateColorBlendState(ColorBlendState*);

	private:
		void CreateBuiltinUniforms();
		void CreateMainRenderPass();
		void CreateDefaultStates();
		RenderThread* renderThread;
		Vector<GFXCommandBuffer*> cmdBuffers;
		Vector<GFXCommandBuffer*> computeCmdBuffers;
		GFXCommandBuffer* cmdBuffer;//current one
		GFXCommandBuffer* computeCmdBuffer;//current one
		Vector4f defaultClearColor;
		bool isCompute = false;
		U32 frameIdx = 0;

		Vector<SharedPtr<CommandBuffer>> mCommandBuffers;
		Vector<SharedPtr<FrameBuffer>> mFrameBuffers;
		Vector<SharedPtr<GPUImage>> mImages;
		Vector<SharedPtr<GPUImageView>> mImageViews;
		Vector<SharedPtr<GPUMemory>> mGPUMemories;
		Vector<SharedPtr<GPUVertexBuffer>> mGPUVertexBuffers;
		Vector<SharedPtr<GPUIndexBuffer>> mGPUIndexBuffers;
		Vector<SharedPtr<GPUUniformBuffer>> mGPUUniformBuffers;
		Vector<SharedPtr<RenderPass>> mRenderPasses;
		Vector<SharedPtr<RasterizationState>> mRasterizationStates;
		Vector<SharedPtr<MultiSampleState>> mMultiSampleStates;
		Vector<SharedPtr<DepthStencilState>> mDepthStencilStates;
		Vector<SharedPtr<ColorBlendState>> mColorBlendStates;
		Vector<SharedPtr<GraphicsPipelineState>> mGraphicsPSOs;
		Vector<SharedPtr<ComputePipelineState>> mComputePSOs;
		Vector<SharedPtr<GPUShader>> mShaders;
		SwapChain* mSwapChain;

		struct ThreadCommandList
		{
			GFXCommandList* cmdList[MAX_CMDLISTS_IN_FLIGHT];
		};
		Vector<ThreadCommandList> mThreadCommandLists;
		U32 msaaSamples{ 0 };
		bool bFloatingPointRT{ false };
	};
}
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
namespace Joestar {
	class PipelineState;
	class CommandBuffer;
	class GFXCommandList;
	class FrameBuffer;
	class SwapChain;
	class GPUImage;
	class GPUImageView;
	class GPUMemory;
	class VertexBuffer;
	class IndexBuffer;
	class GPUVertexBuffer;
	class GPUIndexBuffer;
	class RenderPass;

	class Graphics : public SubSystem {
		REGISTER_SUBSYSTEM(Graphics)
	public:
		explicit Graphics(EngineContext* context);
		void Init();
		virtual void DrawTriangle() {}
		void MainLoop();
		void DrawMesh(Mesh* mesh, Material* mat);
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
		void DrawIndexed(Mesh* mesh, U32 instanceCount = 0);
		void DrawArray(Mesh* mesh, U32 instanceCount = 0);
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

		CommandBuffer* GetMainCommandBuffer();
		CommandBuffer* CreateCommandBuffer();

		FrameBuffer* GetBackBuffer();
		FrameBuffer* CreateFrameBuffer();

		void CreateImage(GPUImage*, U32);
		void CreateImageView(GPUImageView*, U32);
		GPUMemory* CreateGPUMemory();
		GPUVertexBuffer* CreateGPUVertexBuffer(VertexBuffer*);
		GPUIndexBuffer* CreateGPUIndexBuffer(IndexBuffer*);
		GPUUniformBuffer* CreateGPUUniformBuffer(U32 hash);
		GPUUniformBuffer* CreateGPUUniformBuffer(const String& name);
		void CreateRenderPass(RenderPass*);
		RenderPass* GetMainRenderPass();

		void CreateSwapChain();
		void CreateSyncObjects();
		GFXCommandList* GetMainCmdList();

	private:
		void CreateBuiltinUniforms();
		void CreateMainRenderPass();
		RenderThread* renderThread;
		Vector<GFXCommandBuffer*> cmdBuffers;
		Vector<GFXCommandBuffer*> computeCmdBuffers;
		GFXCommandBuffer* cmdBuffer;//current one
		GFXCommandBuffer* computeCmdBuffer;//current one
		Vector4f defaultClearColor;
		bool isCompute = false;
		U32 frameIdx = 0;

		Vector<CommandBuffer*> mCommandBuffers;
		Vector<FrameBuffer*> mFrameBuffers;
		Vector<GPUImage*> mImages;
		Vector<GPUImageView*> mImageViews;
		Vector<GPUMemory*> mGPUMemories;
		Vector<GPUVertexBuffer*> mGPUVertexBuffers;
		Vector<GPUIndexBuffer*> mGPUIndexBuffers;
		Vector<GPUUniformBuffer*> mGPUUniformBuffers;
		Vector<RenderPass*> mRenderPasses;
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
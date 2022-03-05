#pragma once
#include "Mesh.h"
#include "../Thread/RenderThread.h"
#include "../Base/SubSystem.h"
#include "../Base/EngineContext.h"
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
		void UpdateVertexBuffer(VertexBuffer* vb);
		void UpdateIndexBuffer(IndexBuffer* ib);
		void UpdateMaterial(Material* mat);
		void UseShader(Shader* shader);
		void UpdateTexture(Texture*, U8 binding = 0);
		void DrawIndexed(Mesh* mesh, U32 instanceCount = 0);
		void DrawArray(Mesh* mesh, U32 instanceCount = 0);
		void UpdateProgram(ProgramCPU* p);
		void BeginRenderPass(std::string name);
		void EndRenderPass(std::string name);
		void SetDepthCompare(DepthCompareFunc fun);
		void SetPolygonMode(PolygonMode fun);
		void UpdateInstanceBuffer(InstanceBuffer* ib);
		void SetFrameBuffer(FrameBufferDef* def);

		void DispatchCompute(U32 group[3]);
		void BeginCompute(const char* name);
		void EndCompute(const char* name);
		void DrawMeshInstanced(Mesh* mesh, Material* mat, InstanceBuffer* ib);
		void UpdateComputeBuffer(ComputeBuffer* cb, U8 binding = 0);
		void UpdatePushConstant(void* data, U32 size);
		void WriteBackComputeBuffer();

	private:
		RenderThread* renderThread;
		std::vector<GFXCommandBuffer*> cmdBuffers;
		std::vector<GFXCommandBuffer*> computeCmdBuffers;
		GFXCommandBuffer* cmdBuffer;//current one
		GFXCommandBuffer* computeCmdBuffer;//current one
		Vector4f defaultClearColor;
		bool isCompute = false;
		U32 frameIdx = 0;
	};
}
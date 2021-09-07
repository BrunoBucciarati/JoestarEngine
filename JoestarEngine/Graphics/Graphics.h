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
		void UpdateVertexBuffer(VertexBuffer* vb);
		void UpdateIndexBuffer(IndexBuffer* ib);
		void UpdateMaterial(Material* mat);
		void UseShader(const Shader* shader);
		void UpdateTexture(Texture*, U8 binding = 0);
		void DrawIndexed(Mesh* mesh, U32 instanceCount = 0);
		void DrawArray(Mesh* mesh, U32 instanceCount = 0);
		void UpdateProgram(ProgramCPU* p);
		void BeginRenderPass(const char* name);
		void EndRenderPass(const char* name);
		void SetDepthCompare(DepthCompareFunc fun);
		void SetPolygonMode(PolygonMode fun);
		void UpdateInstanceBuffer(InstanceBuffer* ib);
		void DispatchCompute(U32 group[3]);
		void BeginCompute(const char* name);
		void EndCompute(const char* name);
		void DrawMeshInstanced(Mesh* mesh, Material* mat, InstanceBuffer* ib);
		void UpdateComputeBuffer(ComputeBuffer* cb, U16 binding = 0);
		void UpdatePushConstant(void* data, U32 size);
		void WriteBackComputeBuffer();

	private:
		RenderThread* renderThread;
		std::vector<RenderCommand> cmdBuffer;
		std::vector<ComputeCommand> computeCmdBuffer;
		U32 cmdIdx;
		U32 computeCmdIdx;
		Vector4f defaultClearColor;
		bool isCompute = false;
	};
}
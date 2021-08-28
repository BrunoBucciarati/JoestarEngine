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
		void UpdateBuiltinMatrix(BUILTIN_MATRIX typ, Matrix4x4f& mat);
		void UpdateVertexBuffer(VertexBuffer* vb);
		void UpdateIndexBuffer(IndexBuffer* ib);
		void UpdateMaterial(Material* mat);
		void UseShader(const Shader* shader);
		void UpdateTexture(Texture*, U8 binding = 0);
		void DrawIndexed(Mesh* mesh);
		void UpdateProgram(ProgramCPU* p);
		void BeginRenderPass(const char* name);
		void EndRenderPass(const char* name);

	private:
		RenderThread* renderThread;
		std::vector<RenderCommand> cmdBuffer;
		uint16_t cmdIdx;
		Vector4f defaultClearColor;
	};
}
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
namespace Joestar {
	class Graphics : public SubSystem {
		REGISTER_SUBSYSTEM(Graphics)
	public:
		explicit Graphics(EngineContext* context);
		void Init();
		virtual void DrawTriangle() {}
		void MainLoop();
		virtual void DrawMesh(Mesh* mesh) {}
		void Clear();
		void UpdateBuiltinMatrix(BUILTIN_MATRIX typ, Matrix4x4f& mat);
		void UpdateVertexBuffer(VertexBuffer* vb);
		void UpdateIndexBuffer(IndexBuffer* ib);
		void UpdateMaterial(Material* mat);
		void UseShader(std::string& name);
		void UpdateTexture(Texture*);
		void DrawIndexed();

	private:
		RenderThread* renderThread;
		std::vector<RenderCommand> cmdBuffer;
		uint16_t cmdIdx;
		Vector4f defaultClearColor;
	};
}
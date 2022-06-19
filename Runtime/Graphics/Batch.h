#pragma once
#include "../Container/Vector.h"
namespace Joestar
{
	class Renderer;
	class CommandBuffer;
	class PipelineLayout;
	class MeshRenderer;
	class View;
	class GraphicsPipelineState;
	class MaterialInstance;
	class Mesh;
	class ShaderProgram;
	struct Batch
	{
		MaterialInstance* mMaterial;
		U32 mKey;
		Mesh* mMesh;
		Renderer* mRenderer;
		ShaderProgram* mShaderProgram;
		Batch();
		Batch(MeshRenderer* renderer);
		void Render(View* view, CommandBuffer* cb);
		GraphicsPipelineState* PreparePipelineState(View* view, CommandBuffer* cb);
		void CalculateKey();
	};
}
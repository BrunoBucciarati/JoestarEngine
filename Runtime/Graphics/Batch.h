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
	class Camera;
	class ShaderProgram;
	struct Batch
	{
		MaterialInstance* mMaterial;
		U32 mKey{ 0 };
		Mesh* mMesh;
		Renderer* mRenderer;
		ShaderProgram* mShaderProgram;
		Batch();
		Batch(MeshRenderer* renderer);
		void Render(View* view, CommandBuffer* cb, Camera* camera);
		GraphicsPipelineState* PreparePipelineState(View* view, CommandBuffer* cb);
		void CalculateKey();
		U32 mNumInstances{1};
	};
}
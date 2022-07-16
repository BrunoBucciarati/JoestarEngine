#pragma once
#include "Renderer.h"
#include "../Container/Ptr.h"
#include "../Graphics/Mesh.h"
namespace Joestar
{
	class MeshRenderer : public Renderer
	{
		REGISTER_COMPONENT(MeshRenderer, Renderer);
	public:
		void SetMesh(Mesh* mesh)
		{
			mMesh = mesh;
		}


		Mesh* GetMesh()
		{
			return mMesh;
		}
		void Render(CommandBuffer* cb) override;

		const AABB& GetBoundingBox();
	private:
		//SharedPtr<GraphicsPipelineState> GetPipelineState(CommandBuffer* pass);
		//SharedPtr<GraphicsPipelineState> GetDefaultPipelineState(CommandBuffer* pass);
		SharedPtr<Mesh> mMesh;
		AABB mWorldAABB;
	};
}
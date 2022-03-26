#include "MeshRenderer.h"

namespace Joestar {
	MeshRenderer::~MeshRenderer()
	{}

	void MeshRenderer::Render(CommandBuffer* cb)
	{
		auto& pso = GetPipelineState(cb);
		cb->BindPipelineState(pso);
		cb->BindVertexBuffer(mMesh->GetVertexBuffer());
		cb->BindIndexBuffer(mMesh->GetIndexBuffer());
		cb->DrawIndexed(mMesh->GetIndexCount());
	}
}
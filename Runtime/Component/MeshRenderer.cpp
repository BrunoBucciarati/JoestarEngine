#include "MeshRenderer.h"
#include "../Graphics/Graphics.h"
#include "../Base/GameObject.h"
#include "Transform.h"

namespace Joestar {
	MeshRenderer::~MeshRenderer()
	{}

	const AABB& MeshRenderer::GetBoundingBox()
	{
		if (mMesh)
		{
			auto& localAABB = mMesh->GetBoundingBox();
			auto worldTransform = mGameObject->GetAfflineTransform();
			Vector3f center = worldTransform.MultiplyPoint3(localAABB.Center());
			Vector3f scale = worldTransform.GetScale();
			mWorldAABB.Define(center, localAABB.Length() * scale);
		}
		return mWorldAABB;
	}
	void MeshRenderer::Render(CommandBuffer* cb)
	{
		//if (!mUniformBuffers.Empty())
		//{
		//	SetUniformBuffer(PerObjectUniforms::MODEL_MATRIX, (U8*)mGameObject->GetComponent<Transform>()->GetAfflineTransform());
		//	mGraphics->SetUniformBuffer(mUniformBuffers[0]);
		//	mGraphics->UpdateDescriptorSets(mDescriptorSets);
		//	cb->BindDescriptorSets(UniformFrequency::OBJECT, mShaderProgram->GetPipelineLayout(), mDescriptorSets);
		//}
		//auto pso = GetPipelineState(cb);
		//cb->BindPipelineState(pso);
		//cb->BindVertexBuffer(mMesh->GetVertexBuffer());
		////后面外面要按材质排序，这个BATCH的应该放在外面 --todo
		//cb->BindDescriptorSets(UniformFrequency::PASS, mShaderProgram->GetPipelineLayout(), cb->GetPassDescriptorSets());
		//cb->BindIndexBuffer(mMesh->GetIndexBuffer());
		//cb->DrawIndexed(mMesh->GetIndexCount());
	}

	//SharedPtr<GraphicsPipelineState> MeshRenderer::GetPipelineState(CommandBuffer* cb)
	//{
	//	RenderPass* pass = cb->GetRenderPass();
	//	U32 hashIndex = pass->Hash() << 5 + mMaterial->Hash();
	//	for (auto& pso : mPSOs)
	//	{
	//		if (pso->GetHashIndex() == hashIndex)
	//		{
	//			return pso;
	//		}
	//	}
	//	auto pso = GetDefaultPipelineState(cb);
	//	if (mMaterial->GetDepthStencilState())
	//	{
	//		pso->SetDepthStencilState(mMaterial->GetDepthStencilState());
	//	}
	//	pso->SetHashIndex(hashIndex);
	//	mGraphics->CreateGraphicsPipelineState(pso);
	//	return pso;
	//}

	//SharedPtr<GraphicsPipelineState> MeshRenderer::GetDefaultPipelineState(CommandBuffer* cb)
	//{
		//RenderPass* pass = cb->GetRenderPass();
		////没找到对应的，创建一个新的PSO
		//SharedPtr<GraphicsPipelineState>& pso = mPSOs.EmplaceBack();
		//pso = JOJO_NEW(GraphicsPipelineState, MEMORY_GFX_STRUCT);
		//pso->SetRenderPass(pass);
		//pso->SetShaderProgram(GetShaderProgram());
		//pso->SetViewport(cb->GetViewport());
		//pso->SetPipelineLayout(mShaderProgram->GetPipelineLayout());
		////这些值先用default的，后面加了材质设置需要这里做些新的逻辑 --todo
		//pso->SetDepthStencilState(mGraphics->GetDefaultDepthStencilState());
		//pso->SetColorBlendState(mGraphics->GetDefaultColorBlendState());
		//pso->SetMultiSampleState(mGraphics->GetDefaultMultiSampleState());
		//pso->SetRasterizationState(mGraphics->GetDefaultRasterizationState());


		//PODVector<InputAttribute> inputAttributes = GetShaderProgram()->GetInputAttributes();
		////处理下数据不是一比一对齐的情况
		//VertexBuffer* vb = mMesh->GetVertexBuffer();
		//PODVector<VertexElement>& elements = vb->GetVertexElements();
		//for (U32 i = 0; i < inputAttributes.Size(); ++i)
		//{
		//	VertexSemantic semantic = GetMatchingSemantic(inputAttributes[i].name.CString());
		//	inputAttributes[i].offset = vb->GetElementOffset(semantic);
		//	inputAttributes[i].binding = 0;
		//}
		//pso->SetInputAttributes(inputAttributes);

		//PODVector<InputBinding> inputBindings;
		//inputBindings.Push({
		//	0, vb->GetVertexSize(), false
		//});
		//pso->SetInputBindings(inputBindings);
		//return pso;
	//}
}
#include "MeshRenderer.h"
#include "../Graphics/Graphics.h"
#include "../Base/GameObject.h"
#include "Transform.h"

namespace Joestar {
	MeshRenderer::~MeshRenderer()
	{}

	void MeshRenderer::Render(CommandBuffer* cb)
	{
		SetUniformBuffer(PerObjectUniforms::MODEL_MATRIX, (float*)mGameObject->GetComponent<Transform>()->GetAfflineTransform());
		//mGraphics->UpdateDescriptorSets(mMaterial->GetDescriptorSets());
		mGraphics->UpdateDescriptorSets(GetDescriptorSets());
		auto pso = GetPipelineState(cb);
		cb->BindPipelineState(pso);
		cb->BindVertexBuffer(mMesh->GetVertexBuffer());
		//后面外面要按材质排序，这个BATCH的应该放在外面--todo
		cb->BindDescriptorSets((U32)UniformFrequency::BATCH, mMaterial->GetDescriptorSets());
		cb->BindDescriptorSets((U32)UniformFrequency::OBJECT, GetDescriptorSets());
		cb->BindIndexBuffer(mMesh->GetIndexBuffer());
		cb->DrawIndexed(mMesh->GetIndexCount());
	}


	SharedPtr<GraphicsPipelineState> MeshRenderer::GetPipelineState(CommandBuffer* cb)
	{
		RenderPass* pass = cb->GetRenderPass();
		for (auto& pso : mPSOs)
		{
			if (pso->GetRenderPass() == pass && pso->GetViewport() == cb->GetViewport())
			{
				return pso;
			}
		}

		//没找到对应的，创建一个新的PSO
		SharedPtr<GraphicsPipelineState>& pso = mPSOs.EmplaceBack();
		pso = JOJO_NEW(GraphicsPipelineState, MEMORY_GFX_STRUCT);
		pso->SetRenderPass(pass);
		pso->SetShaderProgram(GetShaderProgram());
		pso->SetViewport(cb->GetViewport());
		//这些值先用default的，后面加了材质设置需要这里做些新的逻辑 --todo
		pso->SetDepthStencilState(mGraphics->GetDefaultDepthStencilState());
		pso->SetColorBlendState(mGraphics->GetDefaultColorBlendState());
		pso->SetMultiSampleState(mGraphics->GetDefaultMultiSampleState());
		pso->SetRasterizationState(mGraphics->GetDefaultRasterizationState());


		PODVector<InputAttribute> inputAttributes = GetShaderProgram()->GetInputAttributes();
		//处理下数据不是一比一对齐的情况
		VertexBuffer* vb = mMesh->GetVertexBuffer();
		PODVector<VertexElement>& elements = vb->GetVertexElements();
		for (U32 i = 0; i < inputAttributes.Size(); ++i)
		{
			VertexSemantic semantic = GetMatchingSemantic(inputAttributes[i].name.CString());
			inputAttributes[i].offset = vb->GetElementOffset(semantic);
			inputAttributes[i].binding = 0;
		}
		pso->SetInputAttributes(inputAttributes);

		PODVector<InputBinding> inputBindings;
		inputBindings.Push({
			0, vb->GetVertexSize(), false
		});
		pso->SetInputBindings(inputBindings);
		mGraphics->CreateGraphicsPipelineState(pso);
		return pso;
	}
}
#include "Batch.h"
#include "CommandBuffer.h"
#include "../Component/Renderer.h"
#include "../Component/MeshRenderer.h"
#include "../Base/GameObject.h"
#include "View.h"
#include "Graphics.h"
#include "PipelineState.h"
#include "MaterialInstance.h"
#include "Shader/ShaderProgram.h"
#include "Mesh.h"
namespace Joestar
{
	void Batch::Render(View* view, CommandBuffer* cb)
	{
		bool flag = mMaterial->Update();
		cb->BindDescriptorSets(UniformFrequency::BATCH, mShaderProgram->GetPipelineLayout(), mMaterial->GetBatchDescriptorSets());
		
		auto model = mRenderer->GetGameObject()->GetAfflineTransform();
		mMaterial->SetUniformBuffer(PerObjectUniforms::MODEL_MATRIX, (U8*)&model);
		mMaterial->UpdateDescriptorSets();

		cb->BindDescriptorSets(UniformFrequency::OBJECT, mShaderProgram->GetPipelineLayout(), mMaterial->GetObjectDescriptorSets());

		GraphicsPipelineState* pso = PreparePipelineState(view, cb);
		cb->BindPipelineState(pso);
		cb->BindVertexBuffer(mMesh->GetVertexBuffer());
		//后面外面要按材质排序，这个BATCH的应该放在外面 --todo
		cb->BindDescriptorSets(UniformFrequency::PASS, mShaderProgram->GetPipelineLayout(), cb->GetPassDescriptorSets());
		cb->BindIndexBuffer(mMesh->GetIndexBuffer());
		cb->DrawIndexed(mMesh->GetIndexCount());
	}

	Batch::Batch() = default;

	Batch::Batch(MeshRenderer* renderer) :
		mMaterial(renderer->GetMaterial()),
		mMesh(renderer->GetMesh()),
		mShaderProgram(mMaterial->GetShaderProgram()),
		mRenderer(renderer)
	{
	}


	void Batch::CalculateKey()
	{
		mKey = mMaterial->Hash() * 10000 + *((U32*)&mMesh);
	}

	GraphicsPipelineState* Batch::PreparePipelineState(View* view, CommandBuffer* cb)
	{
		GraphicsPipelineState* pso = mMaterial->GetPipelineState();
		pso->SetRenderPass(cb->GetRenderPass());
		pso->SetViewport(cb->GetViewport());

		PODVector<InputAttribute> inputAttributes = mShaderProgram->GetInputAttributes();
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

		Graphics* graphics = view->GetGraphics();
		bool bExist = graphics->GetGraphicsPipelineState(pso);
		if (!bExist)
		{
			graphics->CreateGraphicsPipelineState(pso);
		}
		return pso;
	}
}
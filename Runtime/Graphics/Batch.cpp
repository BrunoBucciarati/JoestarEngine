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
#include "Camera.h"
namespace Joestar
{
	void Batch::Render(View* view, CommandBuffer* cb, Camera* camera)
	{
		//bool flag = mMaterial->Update();
		mRenderer->Update(camera);
		GraphicsPipelineState* pso = PreparePipelineState(view, cb);
		cb->BindPipelineState(pso);
		
		auto model = mRenderer->GetGameObject()->GetAfflineTransform();
		if (!view->GetGraphics()->IsColumnMajor())
		{
			model.Transponse();
		}
		mMaterial->SetUniformBuffer(PerObjectUniforms::MODEL_MATRIX, (U8*)&model);
		mMaterial->UpdateDescriptorSets();

		if (mMaterial->GetBatchDescriptorSets())
			cb->BindDescriptorSets(UniformFrequency::BATCH, mShaderProgram->GetPipelineLayout(), mMaterial->GetBatchDescriptorSets());

		if (mMaterial->GetObjectDescriptorSets())
			cb->BindDescriptorSets(UniformFrequency::OBJECT, mShaderProgram->GetPipelineLayout(), mMaterial->GetObjectDescriptorSets());

		cb->BindVertexBuffer(mMesh->GetVertexBuffer());
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
	{}

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
		//??????????????????????????????
		VertexBuffer* vb = mMesh->GetVertexBuffer();
		PODVector<VertexElement>& elements = vb->GetVertexElements();
		for (U32 i = 0; i < inputAttributes.Size(); ++i)
		{
			if (VertexSemantic::INVALID == inputAttributes[i].semantic)
			{
				VertexSemantic semantic = GetMatchingSemantic(inputAttributes[i].name.CString());
				inputAttributes[i].semantic = semantic;
			}
			inputAttributes[i].offset = vb->GetElementOffset(inputAttributes[i].semantic);
			inputAttributes[i].binding = 0;
		}
		pso->SetInputAttributes(inputAttributes);
		pso->SetTessellationPatchControlPoints(mMaterial->GetTessellationControlPoints());

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
#pragma once
#include "Renderer.h"
#include "../Container/Ptr.h"
#include "../Graphics/Mesh.h"
namespace Joestar
{
	class MeshRenderer : public Renderer
	{
	public:
		REGISTER_COMPONENT(MeshRenderer, Renderer);
		void SetMesh(Mesh* mesh)
		{
			mMesh = mesh;
		}


		Mesh* GetMesh()
		{
			return mMesh;
		}
		void Render(CommandBuffer* cb) override;
	private:
		SharedPtr<Mesh> mMesh;
	};
}
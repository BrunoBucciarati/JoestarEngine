#pragma once
#include "Renderer.h"
#include "../Container/Ptr.h"
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
		void SetMaterial(Material* mat)
		{
			mMaterial = mat;
		}

		Mesh* GetMesh()
		{
			return mMesh;
		}

		Material* GetMaterial()
		{
			return mMaterial;
		}
	private:
		SharedPtr<Mesh> mMesh;
		SharedPtr<Material> mMaterial;
	};
}
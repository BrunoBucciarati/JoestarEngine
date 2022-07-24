#pragma once
#include "MeshRenderer.h"
namespace Joestar
{
	class Camera;
	class TerrainMeshRenderer : public MeshRenderer
	{
		REGISTER_COMPONENT(TerrainMeshRenderer, MeshRenderer);
	public:
		void Init();
		virtual void Update(Camera* cam);
	};
}
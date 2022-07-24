#include "TerrainMeshRenderer.h"
#include "../Graphics/Material.h"
#include "../Graphics/Camera.h"

namespace Joestar
{
	void TerrainMeshRenderer::Init()
	{

	}
	TerrainMeshRenderer::~TerrainMeshRenderer()
	{

	}

	void TerrainMeshRenderer::Update(Camera* cam)
	{
		Super::Update(cam);
		Vector4f planes[6];
		Frustum frustum = cam->GetFrustum();
		for (U32 i = 0; i < FRUSTUM_PLANE_COUNT; ++i)
		{
			planes[i] = frustum.planes[i].normal;
			planes[i].w = frustum.planes[i].d;
		}
		mMaterial->SetUniformBuffer(PerObjectUniforms::FRUSTUM_PLANES, (U8*)planes);
	}
}
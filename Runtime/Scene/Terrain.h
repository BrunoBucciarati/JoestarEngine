#pragma once
#include "../Core/Minimal.h"
#include "../Base/GameObject.h"
#include "../Graphics/Image.h"
#include "../Graphics/Texture2D.h"
#include "../Graphics/Mesh.h"
namespace Joestar
{
	class TerrainMeshRenderer;
	class MaterialInstance;
	class Terrain : public GameObject
	{
		REGISTER_OBJECT(Terrain, GameObject);
	public:
		explicit Terrain(EngineContext* ctx);
		void BuildMesh(U32 width, U32 height);
	private:
		void InitHeightMap();
		SharedPtr<Texture2D> mHeightMap;
		SharedPtr<Mesh> mMesh;
		U32 mWidth;
		U32 mHeight;
		U32 mPatchWidth{ 4u };
		U32 mPatchHeight{ 4u };
		WeakPtr<TerrainMeshRenderer> mMeshRenderer;
		SharedPtr<MaterialInstance> mMaterial;
		F32 mMaxHeight;
		F32 mMinHeight;
	};
}
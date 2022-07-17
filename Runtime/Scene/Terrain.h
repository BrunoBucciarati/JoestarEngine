#pragma once
#include "../Core/Minimal.h"
#include "../Graphics/Image.h"
#include "../Graphics/Texture2D.h"
#include "../Graphics/Mesh.h"
namespace Joestar
{
	class Terrain : public Object
	{
		REGISTER_OBJECT(Terrain, Object);
	public:
		explicit Terrain(EngineContext* ctx);
	private:
		void InitHeightMap();
		SharedPtr<Texture2D> mHeightMap;
		SharedPtr<Mesh> mMesh;
	};
}
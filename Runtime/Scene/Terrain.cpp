#include "Terrain.h"
namespace Joestar
{
	Terrain::Terrain(EngineContext* ctx) : Super(ctx),
		mHeightMap(JOJO_NEW(Texture2D(ctx), MEMORY_TEXTURE))
	{
		InitHeightMap();
	}

	void Terrain::InitHeightMap()
	{
		Image image(mContext);
		image.LoadFile("Textures/terrain/heightmap.png");
		PODVector<float> heightMap;
		heightMap.Resize(image.GetWidth() * image.GetHeight());
		float* imageData = reinterpret_cast<float*>(image.GetData());
		for (U32 i = 0; i < image.GetHeight(); ++i)
			for (U32 j = 0; j < image.GetWidth(); ++j)
			{
				U32 idx = i * image.GetWidth() + j;
				float height = imageData[idx * image.GetNumChannels()];
				heightMap[idx] = height;
			}

		mHeightMap->SetFormat(ImageFormat::R32);
		mHeightMap->SetWidth(image.GetWidth());
		mHeightMap->SetHeight(image.GetHeight());
		mHeightMap->SetData((U8*)heightMap.Buffer());
	}
}
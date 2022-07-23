#include "Terrain.h"
#include "../Component/MeshRenderer.h"
#include "../Graphics/MaterialInstance.h"
#include "../Graphics/Material.h"
namespace Joestar
{
	Terrain::Terrain(EngineContext* ctx) : Super(ctx),
		mHeightMap(JOJO_NEW(Texture2D(ctx), MEMORY_TEXTURE))
	{
		InitHeightMap();
		mMeshRenderer = GetComponent<MeshRenderer>();

		Material* mat = NEW_OBJECT(Material);
		mat->SetShader("terrain", ShaderStage::VS_HS_DS_PS);
		mMaterial = NEW_OBJECT(MaterialInstance, mat);
		mMeshRenderer->SetMaterial(mMaterial);
	}

	Terrain::~Terrain()
	{

	}

	void Terrain::InitHeightMap()
	{
		Image image(mContext);
		image.Load("Textures/terrain/heightmap.png");
		PODVector<float> heightMap;
		heightMap.Resize(image.GetWidth() * image.GetHeight());
		U8* imageData = reinterpret_cast<U8*>(image.GetData());
		for (U32 i = 0; i < image.GetHeight(); ++i)
			for (U32 j = 0; j < image.GetWidth(); ++j)
			{
				U32 idx = i * image.GetWidth() + j;
				U8 height = imageData[idx * image.GetNumChannels()];
				heightMap[idx] = (F32)height / 255.0F;
			}

		mHeightMap->SetFormat(ImageFormat::R32);
		mHeightMap->SetWidth(image.GetWidth());
		mHeightMap->SetHeight(image.GetHeight());
		mHeightMap->SetData((U8*)heightMap.Buffer());
	}

	void Terrain::BuildMesh(U32 width, U32 height)
	{
		mMesh = NEW_OBJECT(Mesh);
		VertexBuffer* vb = NEW_OBJECT(VertexBuffer);
		IndexBuffer* ib = NEW_OBJECT(IndexBuffer);
		PODVector<VertexElement> elements;
		elements.Push({ VertexSemantic::POSITION, VertexType::VEC3 });
		elements.Push({ VertexSemantic::NORMAL, VertexType::VEC3 });
		elements.Push({ VertexSemantic::TEXCOORD0, VertexType::VEC2 });

		PODVector<float> vertData;
		PODVector<U32> indexData;
		U32 patchNumX = width / mPatchWidth;
		U32 patchNumY = height / mPatchHeight;
		U32 patchCount = patchNumX * patchNumY;
		U32 vertCount = (patchNumX + 1) * (patchNumY + 1);
		F32 halfWidth = (float)width * 0.5;
		F32 halfHeight = (float)height * 0.5;
		vb->SetSize(vertCount, elements);
		ib->SetSize(patchCount * 4, true);
		vertData.Reserve(vertCount * 8);
		indexData.Reserve(patchCount * 4);

		for (U32 i = 0; i <= patchNumX; ++i)
		{
			for (U32 j = 0; j <= patchNumY; ++j)
			{
				vertData.Push((float)mPatchWidth * i- halfWidth);
				vertData.Push(0.F);
				vertData.Push((float)mPatchHeight * j- halfHeight);
				//Normal
				vertData.Push(0.F);
				vertData.Push(1.F);
				vertData.Push(0.F);
				//Texcoord
				vertData.Push((float)i / patchNumX);
				vertData.Push((float)j / patchNumY);
			}
		}

		for (U32 i = 0; i < patchNumX; ++i)
		{
			for (U32 j = 0; j < patchNumY; ++j)
			{
				indexData.Push(i * patchNumY + j);
				indexData.Push((i + 1) * patchNumY + j);
				indexData.Push(i * patchNumY + j + 1);
				indexData.Push((i + 1) * patchNumY + j + 1);
			}
		}

		ib->SetData((U8*)indexData.Buffer());
		vb->SetData((U8*)vertData.Buffer());
		mMesh->SetIndexBuffer(ib);
		mMesh->SetVertexBuffer(vb);

		mMeshRenderer->SetMesh(mMesh);
	}
}
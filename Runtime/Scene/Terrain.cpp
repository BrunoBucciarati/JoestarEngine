#include "Terrain.h"
#include "../Component/TerrainMeshRenderer.h"
#include "../Graphics/MaterialInstance.h"
#include "../Graphics/Material.h"
namespace Joestar
{
	Terrain::Terrain(EngineContext* ctx) : Super(ctx),
		mHeightMap(JOJO_NEW(Texture2D(ctx), MEMORY_TEXTURE))
	{
		mMeshRenderer = GetComponent<TerrainMeshRenderer>();

		Material* mat = NEW_OBJECT(Material);
		mat->SetShader("terrain", ShaderStage::VS_HS_DS_PS);
		mMaterial = NEW_OBJECT(MaterialInstance, mat);
		mMaterial->SetTessellationControlPoints(4U);
		mMaterial->SetPolygonMode(PolygonMode::LINE);
		mMeshRenderer->SetMaterial(mMaterial);

		InitHeightMap();
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
		mMaxHeight = FLT_MIN;
		mMinHeight = FLT_MAX;
		for (U32 i = 0; i < image.GetHeight(); ++i)
			for (U32 j = 0; j < image.GetWidth(); ++j)
			{
				U32 idx = i * image.GetWidth() + j;
				U8 height = imageData[idx * image.GetNumChannels()];
				//test code
				{
					height = 0.F;
				}

				heightMap[idx] = (F32)height;

				mMaxHeight = Max(height, mMaxHeight);
				mMinHeight = Min(height, mMinHeight);
			}

		mHeightMap->SetFormat(ImageFormat::R32);
		mHeightMap->SetWidth(image.GetWidth());
		mHeightMap->SetHeight(image.GetHeight());
		mHeightMap->SetData((U8*)heightMap.Buffer());

		mMaterial->SetTexture(mHeightMap, 1);
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
		elements.Push({ VertexSemantic::TEXCOORD1, VertexType::VEC2 });

		PODVector<F32> vertData;
		PODVector<U32> indexData;
		U32 patchNumX = width / mPatchWidth;
		U32 patchNumY = height / mPatchHeight;
		U32 patchCount = patchNumX * patchNumY;
		U32 vertCount = (patchNumX + 1) * (patchNumY + 1);
		F32 halfWidth = (F32)width * 0.5;
		F32 halfHeight = (F32)height * 0.5;
		vb->SetSize(vertCount, elements);
		ib->SetSize(patchCount * 4, true);
		vertData.Reserve(vertCount * 10);
		indexData.Reserve(patchCount * 4);

		for (U32 i = 0; i <= patchNumX; ++i)
		{
			for (U32 j = 0; j <= patchNumY; ++j)
			{
				vertData.Push((F32)mPatchWidth * i- halfWidth);
				vertData.Push(0.F);
				vertData.Push((F32)mPatchHeight * j- halfHeight);
				//Normal
				vertData.Push(0.F);
				vertData.Push(1.F);
				vertData.Push(0.F);
				//Texcoord
				vertData.Push((F32)i / patchNumX);
				vertData.Push((F32)j / patchNumY);
				//BoundsY
				vertData.Push(mMinHeight);
				vertData.Push(mMaxHeight);
			}
		}

		for (U32 i = 0; i < patchNumX; ++i)
		{
			for (U32 j = 0; j < patchNumY; ++j)
			{
				indexData.Push(i * (patchNumY + 1) + j);
				indexData.Push(i * (patchNumY + 1) + j + 1);
				indexData.Push((i + 1) * (patchNumY + 1) + j);
				indexData.Push((i + 1) * (patchNumY + 1) + j + 1);
			}
		}

		ib->SetData((U8*)indexData.Buffer());
		vb->SetData((U8*)vertData.Buffer());
		mMesh->SetIndexBuffer(ib);
		mMesh->SetVertexBuffer(vb);

		mMeshRenderer->SetMesh(mMesh);
	}
}
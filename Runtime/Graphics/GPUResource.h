#pragma once
#include "../Core/RefCount.h"
#include "../Core/Hashable.h"
#include "GraphicDefines.h"
namespace Joestar
{
	typedef U32 GPUResourceHandle;

	class GPUResource : public RefCount, public Hashable
	{
	public:
		GPUResourceHandle handle;
		U32 Rehash()
		{
			mHash = 0;
			InsertAllHash();
			return mHash;
		}
	protected:
		template<typename T>
		void HashInsert(T val)
		{
			mHash *= 6;
			mHash += U32(val);
		}
		virtual void InsertAllHash() {};
	};

	class GPUMemory : public GPUResource
	{
	public:
		U32 size;
		U8* data;
	};

	class GPUVertexBuffer : public GPUResource
	{
	public:
		GPUMemory* memory;
	};

	class GPUIndexBuffer : public GPUResource
	{
	public:
		GPUMemory* memory;
	};

	class GPUUniformBuffer : public GPUResource
	{
		U32 hash;
	};
}
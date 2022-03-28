#pragma once
#include "../Core/RefCount.h"
#include "../Core/Hashable.h"
#include "GraphicDefines.h"
namespace Joestar
{
	typedef U32 GPUResourceHandle;

	class GPUResource : public RefCount, public Hashable
	{
		static const U32 INVALID_HANDLE = 0xFFFFFFFF;
	public:
		GPUResourceHandle handle{ INVALID_HANDLE };
		U32 Rehash()
		{
			mHash = 0;
			InsertAllHash();
			return mHash;
		}
		bool operator == (const GPUResource& rhs)
		{
			return handle == rhs.handle;
		}
		bool IsValid()
		{
			return handle != INVALID_HANDLE;
		}
		GPUResourceHandle GetHandle()
		{
			return handle;
		}
		void SetHandle(GPUResourceHandle h)
		{
			handle = h;
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
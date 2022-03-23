#pragma once
#include "../Core/Platform.h"
#include "MemoryManager.h"
namespace Joestar
{
	class MemoryRWBase
	{
	public:
		MemoryRWBase()
		{
			data = JOJO_NEW(U8[size]);
			memset(data, 0, size);
			cursor = 0;
		}
		bool Empty() const {
			return cursor == 0 && last == 0;
		}
		void Clear() {
			memset(data, 0, size);
			cursor = 0;
		}
	protected:
		U32 size{ 64 };
		U8* data{ nullptr };
		U32 cursor{ 0 };
		U32 last{ 0 };
	};

	class MemoryWriter : public MemoryRWBase
	{
		template<typename T>
		void WriteBuffer(T& t)
		{
			U32 sz = sizeof(T);
			while (sz + cursor > size) {
				DoubleSizeBuffer();
			}
			memcpy(data + cursor, &t, sz);
			cursor += sz;
		}

		template<typename T>
		void WriteBufferWithSize(T& t)
		{
			U32 sz = sizeof(T);
			while (sz + cursor > size) {
				DoubleSizeBuffer();
			}
			WriteBuffer<U32>(sz);
			memcpy(data + cursor, &t, sz);
			cursor += sz;
		}

		void WriteBufferPtr(void* p, U32 sz) {
			while (sz + cursor > size) {
				DoubleSizeBuffer();
			}
			memcpy(data + cursor, p, sz);
			cursor += sz;
		}

		void Flush() {
			last = cursor;
			cursor = 0;
		}
	private:
		void DoubleSizeBuffer() {
			U8* oldData = data;
			data = JOJO_NEW(U8[2 * size], MEMORY_CONTAINER);
			memset(data, 0, 2 * size);
			memcpy(data, oldData, size);
			size *= 2;
		}
	};

	class MemoryReader : public MemoryRWBase
	{
	public:
		template<typename T>
		bool ReadBuffer(T& t) {
			U32 sz = sizeof(T);
			if (cursor + sz > last) return false;
			memcpy(&t, data + cursor, sizeof(T));
			cursor += sz;
			return true;
		}

		template<typename T>
		bool ReadBufferWithSize(T& t, U32& sz) {
			if (!ReadBuffer<U32>(sz)) return false;
			if (cursor + sz > last) return false;
			memcpy(t, data + cursor, sz);
			cursor += sz;
			return true;
		}

		void SetData(U8* d, U32 sz)
		{
			data = d;
			size = sz;
			cursor = 0;
		}
	};

	class MemoryReadWriter : public MemoryRWBase
	{
	public:
		template<typename T>
		bool ReadBuffer(T& t) {
			U32 sz = sizeof(T);
			if (cursor + sz > last) return false;
			memcpy(&t, data + cursor, sizeof(T));
			cursor += sz;
			return true;
		}

		template<typename T>
		bool ReadBufferWithSize(T& t, U32& sz) {
			if (!ReadBuffer<U32>(sz)) return false;
			if (cursor + sz > last) return false;
			memcpy(t, data + cursor, sz);
			cursor += sz;
			return true;
		}

		void SetData(U8* d, U32 sz)
		{
			data = d;
			size = sz;
			cursor = 0;
		}
		template<typename T>
		void WriteBuffer(T& t)
		{
			U32 sz = sizeof(T);
			while (sz + cursor > size) {
				DoubleSizeBuffer();
			}
			memcpy(data + cursor, &t, sz);
			cursor += sz;
		}

		template<typename T>
		void WriteBufferWithSize(T& t)
		{
			U32 sz = sizeof(T);
			while (sz + cursor > size) {
				DoubleSizeBuffer();
			}
			WriteBuffer<U32>(sz);
			memcpy(data + cursor, &t, sz);
			cursor += sz;
		}

		void WriteBufferPtr(void* p, U32 sz) {
			while (sz + cursor > size) {
				DoubleSizeBuffer();
			}
			memcpy(data + cursor, p, sz);
			cursor += sz;
		}

		virtual void Flush() {
			last = cursor;
			cursor = 0;
		}
	private:
		void DoubleSizeBuffer() {
			U8* oldData = data;
			data = JOJO_NEW(U8[2 * size], MEMORY_CONTAINER);
			memset(data, 0, 2 * size);
			memcpy(data, oldData, size);
			size *= 2;
		}
	};
}
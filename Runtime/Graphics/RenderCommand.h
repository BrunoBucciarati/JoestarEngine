#pragma once
#include <stdint.h>
#include "../IO/MemoryManager.h"
#include <atomic>
namespace Joestar {

	class GFXCommandBuffer {
	public:
		GFXCommandBuffer(U32 sz) {
			size = sz;
			data = JOJO_NEW(U8[sz], MEMORY_GFX_STRUCT);
			memset(data, 0, sz);
			cursor = 0;
		}

		bool Empty() {
			return cursor == 0 && last == 0;
		}

		void WriteCommandType(RenderCommandType t) {
			WriteBuffer<RenderCommandType>(t);
		}

		void WriteCommandType(ComputeCommandType t) {
			WriteBuffer<ComputeCommandType>(t);
		}

		template<typename T>
		void WriteBuffer(T& t);

		void WriteBufferPtr(U8* p, U32 sz) {
			while (sz + cursor > size) {
				DoubleSizeBuffer();
			}
			memcpy(data + cursor, p, sz);
			cursor += sz;
		}

		bool ReadBufferPtr(U8* p, U32 sz) {
			if (sz + cursor > last) {
				return false;
			}
			memcpy(p, data + cursor, sz);
			cursor += sz;
			return true;
		}

		template<typename T>
		void WriteBufferWithSize(T& t);

		template<typename T>
		bool ReadBufferWithSize(T& t, U32& size);

		template<typename T>
		bool ReadBuffer(T& t);
		void Flush() {
			last = cursor;
			cursor = 0;
			ready = true;
		}
		void Clear() {
			memset(data, 0, size);
			cursor = 0;
		}
		std::atomic<bool> ready = false;

	private:
		void DoubleSizeBuffer() {
			U8* oldData = data;
			data = JOJO_NEW(U8[2 * size], MEMORY_GFX_STRUCT);
			memset(data, 0, 2 * size);
			memcpy(data, oldData, size);
			size *= 2;
		}
		U8* data;
		U32 size;
		U32 cursor;
		U32 last;
	};

	template<typename T>
	void GFXCommandBuffer::WriteBuffer(T& t) {
		U32 sz = sizeof(T);
		while (sz + cursor > size) {
			DoubleSizeBuffer();
		}
		memcpy(data + cursor, &t, sz);
		cursor += sz;
	}

	template<typename T>
	void GFXCommandBuffer::WriteBufferWithSize(T& t) {
		U32 sz = sizeof(T);
		WriteBuffer<U32>(sz);
		while (sz + cursor > size) {
			DoubleSizeBuffer();
		}
		memcpy(data + cursor, &t, sz);
		cursor += sz;
	}

	template<typename T>
	bool GFXCommandBuffer::ReadBuffer(T& t) {
		U32 sz = sizeof(T);
		if (cursor + sz > last) return false;
		memcpy(&t, data + cursor, sizeof(T));
		cursor += sz;
		return true;
	}

	template<typename T>
	bool GFXCommandBuffer::ReadBufferWithSize(T& t, U32& sz) {
		if (!ReadBuffer<U32>(sz)) return false;
		if (cursor + sz > last) return false;
		memcpy(t, data + cursor, sz);
		cursor += sz;
		return true;
	}
}
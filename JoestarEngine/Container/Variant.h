#pragma once
#include "../Core/Platform.h"
#include "../IO/MemoryManager.h"
#include "Str.h"
namespace Joestar {
	class Variant {
	public:
		Variant() = default;

		template<typename T>
		Variant(const T& value)
		{
			data = JOJO_NEW_ARRAY(U8, sizeof(T), MEMORY_CONTAINER);
			memcpy(data, &value, sizeof(T));
		}

		Variant(const String& value)
		{
			U32 length = value.Length();
			data = JOJO_NEW_ARRAY(U8, sizeof(length + 1), MEMORY_CONTAINER);
			memcpy(data, value.CString(), length + 1);
		}

		template <typename T>
		operator T() const
		{
			return *((T*)data);
		}

		operator String() const
		{
			return String((const char*)data);
		}
	private:
		U8* data;
	};
}
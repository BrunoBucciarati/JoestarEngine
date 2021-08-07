#pragma once
#include <map>
#include "Object.h"
namespace Joestar {
	class EngineContext {
		friend class Object;
	public:
		std::map<uint32_t, Object*> subSystems;
		Object* GetSubSystem(uint32_t classID);
		template<class T>
		T* GetSubSystem() {
			Object* obj = GetSubSystem(T::GetClassID());
			return static_cast<T*>(obj);
		}
	};
}
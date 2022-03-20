#pragma once
#include "../Container/HashMap.h"
#include "Object.h"
namespace Joestar {
	class EngineContext {
		friend class Object;
	public:
		HashMap<uint32_t, Object*> subSystems;
		Object* GetSubSystem(uint32_t classID);
		template<class T>
		T* GetSubSystem();
	};

	template<class T>
	T* EngineContext::GetSubSystem() {
		Object* obj = GetSubSystem(T::GetClassIDStatic());
		return static_cast<T*>(obj);
	}
}
#pragma once
#include "../Container/HashMap.h"
#include "Object.h"
namespace Joestar {
	class EngineContext {
		friend class Object;
	public:
		HashMap<uint32_t, Object*> subSystems;
		Object* GetSubsystem(uint32_t classID);
		template<class T>
		T* GetSubsystem();
	};

	template<class T>
	T* EngineContext::GetSubsystem() {
		Object* obj = GetSubsystem(T::GetClassIDStatic());
		return static_cast<T*>(obj);
	}
}
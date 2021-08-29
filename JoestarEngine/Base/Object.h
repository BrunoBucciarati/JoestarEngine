#pragma once
#include "ObjectDefines.h"
#include "ClassIDs.h"
#include <stdint.h>
namespace Joestar {
	class EngineContext;
	class Object {
		friend class EngineContext;
		REGISTER_OBJECT_ROOT(Object)

		explicit Object(EngineContext* context);

		Object* GetSubsystem(uint32_t classID) const;

		template <class T>
		T* GetSubsystem() const {
			return static_cast<T*>(GetSubsystem(T::GetClassIDStatic()));
		}
	protected:
		EngineContext* mContext;
	};

}
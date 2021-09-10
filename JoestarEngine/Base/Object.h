#pragma once
#include "ObjectDefines.h"
#include "ClassIDs.h"
#include <stdint.h>
#include <string>
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

		std::string GetName() { return mName; }
		void SetName(std::string n) { mName = n; }

		virtual void Destroy() { delete this; }
	protected:
		EngineContext* mContext;
		std::string mName;
	};

}
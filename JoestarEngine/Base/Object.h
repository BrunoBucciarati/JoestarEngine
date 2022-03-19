#pragma once
#include "RefCount.h"
#include "ObjectDefines.h"
#include "ClassIDs.h"
#include <string>
namespace Joestar {
	class EngineContext;
	class Object : public RefCount {
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
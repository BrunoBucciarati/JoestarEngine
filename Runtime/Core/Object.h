#pragma once
#include "RefCount.h"
#include "ObjectDefines.h"
#include "ClassIDs.h"
#include "../Container/Str.h"
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

		String GetName() { return mName; }
		void SetName(String n) { mName = n; }

		virtual void Destroy() { delete this; }
	protected:
		EngineContext* mContext;
		String mName;
	};

}
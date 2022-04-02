#include "Object.h"
#include "EngineContext.h"

namespace Joestar {
	Object::Object(EngineContext* context)
	{
		mContext = context;
	}

	Object::~Object()
	{

	}

	Object* Object::GetSubsystem(uint32_t classID) const
	{
		return mContext->GetSubsystem(classID);
	}
}
#include "EngineContext.h"
#include "ClassIDs.h"
namespace Joestar {
	Object* EngineContext::GetSubsystem(U32 classID) {
		HashMap<U32, Object*>::Iterator it = subSystems.Find(classID);
		if (it != subSystems.End())
			return it->value;
		return nullptr;
	}
}
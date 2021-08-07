#include "EngineContext.h"
#include "ClassIDs.h"
namespace Joestar {
	Object* EngineContext::GetSubSystem(uint32_t classID) {
		std::map<uint32_t, Object*>::iterator it = subSystems.find(classID);
		if (it != subSystems.end())
			return it->second;
		return nullptr;
	}
}
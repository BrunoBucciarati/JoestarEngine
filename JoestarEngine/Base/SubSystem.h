#pragma once

#include <vector>
#include <string>
#include "Object.h"
#include "ObjectDefines.h"
namespace Joestar {
	class SubSystem : public Object {
		REGISTER_OBJECT(SubSystem, Object)
	public:
		explicit SubSystem(EngineContext* context);
		virtual void Update(float dt) {}
	};
}
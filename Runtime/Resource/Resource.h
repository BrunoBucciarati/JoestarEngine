#pragma once
#include "../Core/Object.h"

namespace Joestar
{
	class Resource : public Object
	{
		REGISTER_OBJECT_ROOT(Resource)
		explicit Resource(EngineContext* ctx) : Super(ctx) {}
	};
}
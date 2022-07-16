#pragma once
#include "../../Core/Minimal.h"
namespace Joestar
{
	class SubShader : public Object
	{
		REGISTER_OBJECT_ROOT(SubShader)
	public:
		explicit SubShader(EngineContext* ctx);
	};
}
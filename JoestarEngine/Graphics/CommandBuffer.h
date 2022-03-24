#pragma once
#include "GPUResource.h"
namespace Joestar {
	class CommandBuffer : public GPUResource
	{
	public:
		GPUQueue queue;
	};
}
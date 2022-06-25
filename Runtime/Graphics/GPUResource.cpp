#include "GPUResource.h"

namespace Joestar
{
	bool GPUResource::IsValid(GPUResourceHandle handle)
	{
		return handle != INVALID_HANDLE;
	}
}
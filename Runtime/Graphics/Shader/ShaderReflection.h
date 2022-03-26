#pragma once
#include "../../Container/Vector.h"
#include "../Descriptor.h"
#include "../PipelineState.h"
namespace Joestar
{
	class File;
	class ShaderReflection
	{
	public:
		bool ReflectSpirv(File* file);
	private:
		Vector<DescriptorSetLayout> mDescriptorSetLayouts;
		Vector<VertexInputAttribute> mInputAttributes;
	};
}
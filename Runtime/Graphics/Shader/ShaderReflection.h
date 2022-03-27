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
		Vector<DescriptorSetLayout>& GetDescriptorSetLayouts()
		{
			return mDescriptorSetLayouts;
		}
		U32 GetNumDescriptorSetLayouts()
		{
			return mDescriptorSetLayouts.Size();
		}

	private:
		Vector<DescriptorSetLayout> mDescriptorSetLayouts;
		Vector<VertexInputAttribute> mInputAttributes;
	};
}
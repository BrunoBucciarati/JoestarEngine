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
		bool ReflectSpirv(File* file, ShaderStage stage);
		Vector<DescriptorSetLayout>& GetDescriptorSetLayouts()
		{
			return mDescriptorSetLayouts;
		}
		U32 GetNumDescriptorSetLayouts()
		{
			return mDescriptorSetLayouts.Size();
		}
		Vector<InputAttribute>& GetInputAttributes()
		{
			return mInputAttributes;
		}

	private:
		Vector<DescriptorSetLayout> mDescriptorSetLayouts;
		Vector<InputAttribute> mInputAttributes;
	};
}
#include "ShaderReflection.h"
#include <spirv_reflect.h>
#include "../../IO/File.h"
#include "assert.h"

namespace Joestar
{
	bool ShaderReflection::ReflectSpirv(File* file)
	{
		SpvReflectShaderModule module;
		SpvReflectResult result = spvReflectCreateShaderModule(file->Size(), file->GetBuffer(), &module);
		assert(result == SPV_REFLECT_RESULT_SUCCESS);

		// Enumerate and extract shader's input variables
		U32 var_count = 0;
		result = spvReflectEnumerateInputVariables(&module, &var_count, NULL);
		assert(result == SPV_REFLECT_RESULT_SUCCESS);
		SpvReflectInterfaceVariable** input_vars =
			(SpvReflectInterfaceVariable**)malloc(var_count * sizeof(SpvReflectInterfaceVariable*));
		result = spvReflectEnumerateInputVariables(&module, &var_count, input_vars);
		assert(result == SPV_REFLECT_RESULT_SUCCESS);

        mInputAttributes.Resize(var_count);
        for (U32 i = 0; i < var_count; ++i)
        {
            SpvReflectInterfaceVariable* var = input_vars[i];
            mInputAttributes[i].location = var->location;
            if (var->format == SPV_REFLECT_FORMAT_R32G32_SFLOAT)
            {
                mInputAttributes[i].format = VertexType::VEC2;
            }
            else if (var->format == SPV_REFLECT_FORMAT_R32_SFLOAT)
            {
                mInputAttributes[i].format = VertexType::FLOAT;
            }
            else if (var->format == SPV_REFLECT_FORMAT_R32G32B32_SFLOAT)
            {
                mInputAttributes[i].format = VertexType::VEC3;
            }
            else if (var->format == SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT)
            {
                mInputAttributes[i].format = VertexType::VEC4;
            }
            else if (var->format == SPV_REFLECT_FORMAT_R32_UINT)
            {
                mInputAttributes[i].format = VertexType::UINT32;
            }
            mInputAttributes[i].name = var->name;
        }

		// Output variables, descriptor bindings, descriptor sets, and push constants
		// can be enumerated and extracted using a similar mechanism.
		U32 count = 0;
		result = spvReflectEnumerateDescriptorSets(&module, &count, NULL);
		assert(result == SPV_REFLECT_RESULT_SUCCESS);

		Vector<SpvReflectDescriptorSet*> sets(count);
		result = spvReflectEnumerateDescriptorSets(&module, &count, sets.Buffer());
		assert(result == SPV_REFLECT_RESULT_SUCCESS);

        U32 maxSets = 0;
        for (U32 i = 0; i < sets.Size(); ++i) {
            SpvReflectDescriptorSet& descriptorSet = *sets[i];
            maxSets = Max(maxSets, descriptorSet.set);
        }
        mDescriptorSetLayouts.Resize(maxSets + 1);
        U32 curSet = 0;
        for (U32 i = 0; i < sets.Size(); ++i) {
            SpvReflectDescriptorSet& descriptorSet = *sets[i];
            mDescriptorSetLayouts[descriptorSet.set].SetNumBindings(descriptorSet.binding_count);
            for (U32 binding = 0; binding < descriptorSet.binding_count; ++binding)
            {
                SpvReflectDescriptorBinding& descriptorBinding = *descriptorSet.bindings[binding];
                auto* layoutBinding = mDescriptorSetLayouts[descriptorSet.set].GetLayoutBinding(binding);
                layoutBinding->binding = descriptorBinding.binding;
                layoutBinding->type = DescriptorType(descriptorBinding.descriptor_type);
                layoutBinding->count = descriptorBinding.count;
                layoutBinding->size = descriptorBinding.block.size;
                layoutBinding->members.Resize(descriptorBinding.block.member_count);
                for (U32 member = 0; member < layoutBinding->members.Size(); ++member)
                {
                    layoutBinding->members[member].ID = GetUniformID(descriptorBinding.block.members[member].name);
                    layoutBinding->members[member].offset = descriptorBinding.block.members[member].offset;
                    layoutBinding->members[member].size = descriptorBinding.block.members[member].size;
                }
            }
        }

		// Destroy the reflection data when no longer required.
		spvReflectDestroyShaderModule(&module);

		return true;
	}
}
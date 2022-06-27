#include "ShaderReflection.h"
#include <spirv_reflect.h>
#include <d3dcompiler.h>
#include "../../IO/File.h"
#include "assert.h"

namespace Joestar
{
	bool ShaderReflection::ReflectSpirv(File* file, ShaderStage stage)
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
                layoutBinding->stage |= (U32)stage;
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

    U32 GetConstantBufferSetIdx(const char* name)
    {
        if (0 == strcmp(name, "cbPerPass"))
            return 0;
        if (0 == strcmp(name, "cbPerBatch"))
            return 1;
        if (0 == strcmp(name, "cbPerObject"))
            return 2;
    }

    bool ShaderReflection::ReflectHLSL(void* blob, ShaderStage stage)
    {
        ID3D10Blob* compiledShader = (ID3D10Blob*)blob;
        ID3D11ShaderReflection* pReflection;
        HRESULT hr = D3DReflect(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflection);
        if (FAILED(hr))
            return false;
        D3D11_SHADER_DESC shaderDesc;
        pReflection->GetDesc(&shaderDesc);
        mInputAttributes.Resize(shaderDesc.InputParameters);
        U32 offset = 0;
        for (U32 i = 0; i < shaderDesc.InputParameters; ++i)
        {
            D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
            pReflection->GetInputParameterDesc(i, &paramDesc);
            mInputAttributes[i].name = paramDesc.SemanticName;
            mInputAttributes[i].binding = paramDesc.Register;
            mInputAttributes[i].offset = offset;
            if (paramDesc.SemanticName == "POSITION")
            {
                mInputAttributes[i].semantic = VertexSemantic::POSITION;
            }
            else if (paramDesc.SemanticName == "NORMAL")
            {
                mInputAttributes[i].semantic = VertexSemantic::NORMAL;
            }
            else if (paramDesc.SemanticName == "TEXCOORD")
            {
                mInputAttributes[i].semantic = (VertexSemantic)((U32)VertexSemantic::TEXCOORD0 + paramDesc.SemanticIndex);
            }

            if (paramDesc.Mask == 1)
            {
                if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                {
                    mInputAttributes[i].format = VertexType::UINT32;
                }
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                {
                    mInputAttributes[i].format = VertexType::SINT32;
                }
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                {
                    mInputAttributes[i].format = VertexType::FLOAT;
                }
                offset += 4;
            }
            else if (paramDesc.Mask <= 3)
            {
                if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                {
                    mInputAttributes[i].format = VertexType::UVEC2;
                }
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                {
                    mInputAttributes[i].format = VertexType::SVEC2;
                }
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                {
                    mInputAttributes[i].format = VertexType::VEC2;
                }
                offset += 8;
            }
            else if (paramDesc.Mask <= 7)
            {
                if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                {
                    mInputAttributes[i].format = VertexType::UVEC3;
                }
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                {
                    mInputAttributes[i].format = VertexType::SVEC3;
                }
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                {
                    mInputAttributes[i].format = VertexType::VEC3;
                }
                offset += 12;
            }
            else if (paramDesc.Mask <= 15)
            {
                if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                {
                    mInputAttributes[i].format = VertexType::UVEC4;
                }
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                {
                    mInputAttributes[i].format = VertexType::SVEC4;
                }
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                {
                    mInputAttributes[i].format = VertexType::VEC4;
                }
                offset += 16;
            }
        }


        U32 maxSets = 0;
        for (U32 i = 0; i < shaderDesc.ConstantBuffers; ++i)
        {
            ID3D11ShaderReflectionConstantBuffer* pCBReflection = pReflection->GetConstantBufferByIndex(i);
            D3D11_SHADER_BUFFER_DESC desc;
            pCBReflection->GetDesc(&desc);
            U32 setIdx = GetConstantBufferSetIdx(desc.Name);
            maxSets = Max(maxSets, setIdx);
        }
        for (U32 i = 0; i < shaderDesc.ConstantBuffers; ++i)
        {
            ID3D11ShaderReflectionConstantBuffer* pCBReflection = pReflection->GetConstantBufferByIndex(i);
            D3D11_SHADER_BUFFER_DESC desc;
            pCBReflection->GetDesc(&desc);

            U32 setIdx = GetConstantBufferSetIdx(desc.Name);
            if (desc.Type == D3D_CT_CBUFFER)
            {
                
            }
            else if (desc.Type == D3D_CT_TBUFFER)
            {

            }
        }
    }
}
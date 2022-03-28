#include "GraphicDefines.h"
namespace Joestar
{
	const char* GetShaderSuffix(ShaderLanguage lang, ShaderStage stage)
	{
		if (ShaderLanguage::GLSL == lang)
		{
			if (ShaderStage::VS == stage)
				return ".vert";
			else if (ShaderStage::PS == stage)
				return ".frag";
		}
		if (ShaderLanguage::SPIRV == lang)
		{
			return ".spv";
		}
		return "";
	}


	VertexSemantic GetMatchingSemantic(const char* name)
	{
		if (strcmp(name, "aPosition") == 0)
			return VertexSemantic::POSITION;
		if (strcmp(name, "aNormal") == 0)
			return VertexSemantic::NORMAL;
		if (strcmp(name, "aTangent") == 0)
			return VertexSemantic::TANGENT;
		if (strcmp(name, "aTexCoord0") == 0)
			return VertexSemantic::TEXCOORD0;
		if (strcmp(name, "aTexCoord1") == 0)
			return VertexSemantic::TEXCOORD1;
		if (strcmp(name, "aTexCoord2") == 0)
			return VertexSemantic::TEXCOORD2;
		if (strcmp(name, "aTexCoord3") == 0)
			return VertexSemantic::TEXCOORD3;
		if (strcmp(name, "aTexCoord4") == 0)
			return VertexSemantic::TEXCOORD4;
	}

	const UniformDataType PerObjectUniformTypes[] = {
		UniformDataType::Mat4,
	};
	UniformDataType GetPerObjectUniformDataType(PerObjectUniforms uniform)
	{
		return PerObjectUniformTypes[(U32)uniform - (U32)PerObjectUniforms::MODEL_MATRIX];
	}
}
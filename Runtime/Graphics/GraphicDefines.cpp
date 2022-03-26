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
}
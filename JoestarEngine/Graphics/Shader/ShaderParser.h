#pragma once
#include "../GraphicDefines.h"
#include "../../Base/SubSystem.h"
#include <vector>
#include <string>
namespace Joestar {
	enum ShaderDataType {
		ShaderDataTypeInvalid = 0,
		ShaderDataTypeFloat,
		ShaderDataTypeVec2,
		ShaderDataTypeVec3,
		ShaderDataTypeVec4,
		ShaderDataTypeMat4,
		ShaderDataTypeSampler,
		SamplerType2D,
		SamplerTypeCube,
		SamplerType3D
	};
	struct VertexDef {
		VERTEX_ATTRIBUTE attr;
		ShaderDataType dataType;
	};
	struct UniformDef {
		std::string name;
		ShaderDataType dataType;
	};
/*	struct SamplerDef {
		std::string name;
		SamplerType dataType;
	}*/;
	struct UBODef {
		std::string name;
	};
	struct ShaderInfo {
		uint16_t version;
		std::vector<VertexDef> attrs;
		std::vector<UniformDef> uniforms;
		//std::vector<SamplerDef> samplers;
		std::vector<UBODef> ubos;
		std::string entryFunction = "main";
	};
	class ShaderParser : public SubSystem {
		REGISTER_SUBSYSTEM(ShaderParser)
	public:
		explicit ShaderParser(EngineContext* ctx) : Super(ctx) {}
		void ParseShader(std::string& name, ShaderInfo& si);
	};
}
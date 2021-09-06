#pragma once
#include "../GraphicDefines.h"
#include "../../Base/SubSystem.h"
#include <vector>
#include <string>
#include "ShaderDefs.h"

namespace Joestar {
	enum ShaderDataType {
		ShaderDataTypeInvalid = 0,
		ShaderDataTypeFloat,
		ShaderDataTypeVec2,
		ShaderDataTypeVec3,
		ShaderDataTypeVec4,
		ShaderDataTypeMat4,
		ShaderDataTypeUBO,
		ShaderDataTypePushConst,
		ShaderDataTypeBuffer,
		ShaderDataTypeImage2D,
		ShaderDataTypeImageCube,

		ShaderDataTypeSampler,
		SamplerType2D,
		SamplerTypeCube,
		SamplerType3D
	};
	struct VertexDef {
		VERTEX_ATTRIBUTE attr;
		ShaderDataType dataType;
		U16 location;
		bool instancing = false;
	};
	struct UniformDef {
		std::string name;
		ShaderDataType dataType;
		U16 binding;
		U32 stageFlag = 0;
		bool writeFlag = true;
		bool readFlag = true;
		U32 id = 0;
		
		bool IsSampler() {
			return dataType > ShaderDataTypeSampler;
		}

		bool IsImage() {
			return dataType == ShaderDataTypeImage2D || dataType == ShaderDataTypeImageCube;
		}

		bool IsPushConsts() {
			return dataType == ShaderDataTypePushConst;
		}

		bool IsBuffer() {
			return dataType == ShaderDataTypeBuffer;
		}

		bool IsUniform() {
			return dataType == ShaderDataTypeUBO;
		}
	};
	struct BufferDef {
		std::string name;
		ShaderDataType dataType;
		U16 binding;
		U32 stageFlag = 0;
	};
/*	struct SamplerDef {
		std::string name;
		SamplerType dataType;
	}*/;
	//struct UBODef {
	//	std::string name;
	//};
	struct ShaderInfo {
		U16 version;
		std::vector<VertexDef> attrs;
		std::vector<UniformDef> uniforms;
		//std::vector<BufferDef> buffers;
		std::string entryFunction = "main";
		ShaderStage curStage;
	};
	class ShaderParser : public SubSystem {
		REGISTER_SUBSYSTEM(ShaderParser)
	public:
		explicit ShaderParser(EngineContext* ctx) : Super(ctx) {}
		void ParseShader(std::string& name, ShaderInfo& si, U32 flag);
	private:
	};
}
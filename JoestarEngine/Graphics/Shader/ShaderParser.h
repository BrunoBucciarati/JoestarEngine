#pragma once
#include "../GraphicDefines.h"
#include "../../Core/SubSystem.h"
#include <vector>
#include <string>
#include "ShaderDefs.h"
#include "../../IO/FileSystem.h"

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
		String name;
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
		String name;
		ShaderDataType dataType;
		U16 binding;
		U32 stageFlag = 0;
	};
/*	struct SamplerDef {
		String name;
		SamplerType dataType;
	}*/;
	//struct UBODef {
	//	String name;
	//};
	struct ShaderInfo {
		U16 version;
		std::vector<VertexDef> attrs;
		std::vector<UniformDef> uniforms;
		//std::vector<BufferDef> buffers;
		String entryFunction = "main";
		ShaderStage curStage;
	};
	struct TokenStream;
	class ShaderParser : public SubSystem {
		REGISTER_SUBSYSTEM(ShaderParser)
	public:
		explicit ShaderParser(EngineContext* ctx);
		bool ParseInclude(TokenStream& tokenStream, String& oldStr, String& newFile);
		void ParseShader(String& name, ShaderInfo& si, U32 flag);
		void ParseVertexShader(char* buffer, U32 idx, U32 size, ShaderInfo& shaderInfo, String& newFile);
		void ParseFragmentShader(char* buffer, U32 idx, U32 size, ShaderInfo& shaderInfo, String& newFile);
		void ParseComputeShader(char* buffer, U32 idx, U32 size, ShaderInfo& shaderInfo, String& newFile);
		String& GetShaderOutputDir() { return outputDir; }
	private:
		String shaderDir;
		String outputDir;
		FileSystem* fs;
	};
}
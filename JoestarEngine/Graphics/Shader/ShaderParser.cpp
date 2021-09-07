#include "ShaderParser.h"
#include "../../IO/FileSystem.h"
#include "../../IO/Log.h"
#include <string>
#include <vector>
#define TOKEN_LAYOUT "layout"
#define TOKEN_BINDING "binding"
#define TOKEN_LOCATION "location"
#define TOKEN_EQ "="
#define TOKEN_LP "("
#define TOKEN_RP ")"
#define TOKEN_TYPE_VOID "void"
#define TOKEN_GL_VERSION "#version"
#define TOKEN_IN "in"
#define TOKEN_UNIFORM "uniform"
#define TOKEN_FLOAT "float"
#define TOKEN_VEC2 "vec2"
#define TOKEN_VEC3 "vec3"
#define TOKEN_VEC4 "vec4"
#define TOKEN_MAT4 "mat4"
#define TOKEN_SAMPLER2D "sampler2D"
#define TOKEN_SAMPLERCUBE "samplerCube"
#define TOKEN_SAMPLER3D "sampler3D"
#define TOKEN_IMAGE2D "image2D"
#define TOKEN_IMAGECUBE "imageCube"
#define TOKEN_LB "{"
#define TOKEN_RB "}"
#define TOKEN_SEMICOLON ";"
#define TOKEN_IN_POSITION "inPosition"
#define TOKEN_INSTANCE_POSITION "instancePos"
#define TOKEN_IN_COLOR "inColor"
#define TOKEN_IN_TEXCOORD "inTexCoord"
#define TOKEN_IN_NORMAL "inNormal"
#define TOKEN_PUSH_CONSTANT "push_constant"
#define TOKEN_BUFFER "buffer"
#define TOKEN_COMMA ","
#define TOKEN_READONLY "readonly"
#define TOKEN_WRITEONLY "writeonly"

#define SET_DATATYPE_BY_TOKEN(val, token) \
	if (token == TOKEN_FLOAT) val = ShaderDataTypeFloat; \
	if (token == TOKEN_VEC2) val = ShaderDataTypeVec2; \
	if (token == TOKEN_VEC3) val = ShaderDataTypeVec3; \
	if (token == TOKEN_VEC4) val = ShaderDataTypeVec4; \
	if (token == TOKEN_MAT4) val = ShaderDataTypeMat4;\
	if (token == TOKEN_SAMPLER2D) val = SamplerType2D; \
	if (token == TOKEN_SAMPLERCUBE) val = SamplerTypeCube; \
	if (token == TOKEN_SAMPLER3D) val = SamplerType3D; \
	if (token == TOKEN_IMAGE2D) val = ShaderDataTypeImage2D; \
	if (token == TOKEN_IMAGECUBE) val = ShaderDataTypeImageCube; 

#define SET_SAMPLERTYPE_BY_TOKEN(val, token) \
	if (token == TOKEN_SAMPLER2D) val = SamplerType2D; \
	if (token == TOKEN_SAMPLERCUBE) val = SamplerTypeCube; \
	if (token == TOKEN_SAMPLER3D) val = SamplerType3D;

namespace Joestar {
	struct TokenStream {
		std::vector<std::string> tokens;
		uint32_t curIdx;
		uint32_t size;

		void resize(uint32_t size) {
			tokens.resize(size);
			curIdx = 0;
		}

		std::string& operator[](int idx) {
			return tokens[idx];
		}

		bool AcceptToken(const char* token) {
			if (tokens[curIdx] == token) {
				++curIdx;
				return true;
			}
			return false;
		}

		bool AcceptTokenForward(const char* token) {
			while (curIdx < size) {
				if (tokens[curIdx] == token) {
					++curIdx;
					return true;
				} else {
					++curIdx;
				}
			}
			return false;
		}

		bool AcceptInt16(uint16_t& i) {
			int n = atoi(tokens[curIdx].c_str());
			i = n;
			++curIdx;
			return true;
		}

		bool AcceptInt(uint32_t& i) {
			int n = atoi(tokens[curIdx].c_str());
			i = n;
			++curIdx;
			return true;
		}

		bool AcceptString(std::string& str) {
			str = tokens[curIdx];
			++curIdx;
			return true;
		}

#define TRY_ACCEPT_DATATYPE_TOKEN(t) \
	if (AcceptToken(t)) { \
		str = t; \
		return true; \
	}

		bool AcceptDataTypeToken(std::string& str) {
			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_FLOAT);
			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_VEC2);
			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_VEC3);
			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_VEC4);
			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_MAT4);
			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_SAMPLER2D);
			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_SAMPLERCUBE);
			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_SAMPLER3D);
			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_IMAGE2D);
			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_IMAGECUBE);
			return false;
		}

		bool AcceptSamplerTypeToken(std::string& str) {
			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_SAMPLER2D);
			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_SAMPLERCUBE);
			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_SAMPLER3D);
			return false;
		}
	};

	bool TryParseBuffer(TokenStream& tokenStream, ShaderInfo& shaderInfo, uint16_t binding = 0) {
		if (tokenStream.AcceptToken(TOKEN_BUFFER)) {
			std::string tok;
			//must be a UBO
			tokenStream.AcceptString(tok);
			shaderInfo.uniforms.push_back(UniformDef{});
			UniformDef& def = shaderInfo.uniforms.back();
			def.name = tok;
			def.binding = binding;
			def.stageFlag |= shaderInfo.curStage;
			def.dataType = ShaderDataTypeBuffer;
			tokenStream.AcceptToken(TOKEN_LB);
			tokenStream.AcceptTokenForward(TOKEN_RB);
			tokenStream.AcceptTokenForward(TOKEN_SEMICOLON);
		}
		else {
			return false;
		}
		return true;
	}

	bool TryParseUniform(TokenStream& tokenStream, ShaderInfo& shaderInfo, uint16_t binding = 0) {
		if (tokenStream.AcceptToken(TOKEN_UNIFORM)) {
			bool readFlag = true, writeFlag = true;
			if (tokenStream.AcceptToken(TOKEN_READONLY)) {
				writeFlag = false;
			}
			if (tokenStream.AcceptToken(TOKEN_WRITEONLY)) {
				readFlag = false;
			}
			std::string tok;
			if (tokenStream.AcceptDataTypeToken(tok)) {
				shaderInfo.uniforms.push_back(UniformDef{});
				UniformDef& def = shaderInfo.uniforms.back();

				SET_DATATYPE_BY_TOKEN(def.dataType, tok)
				tokenStream.AcceptString(def.name);
				def.binding = binding;
				def.stageFlag |= shaderInfo.curStage;
				def.readFlag = readFlag;
				def.writeFlag = writeFlag;
				tokenStream.AcceptTokenForward(TOKEN_SEMICOLON);
			} else if (binding == 99) {
				//means push constant
				tokenStream.AcceptString(tok);
				shaderInfo.uniforms.push_back(UniformDef{});
				UniformDef& def = shaderInfo.uniforms.back();
				def.name = tok;
				def.binding = binding;
				def.stageFlag |= shaderInfo.curStage;
				def.dataType = ShaderDataTypePushConst;
				def.readFlag = readFlag;
				def.writeFlag = writeFlag;
				tokenStream.AcceptToken(TOKEN_LB);
				tokenStream.AcceptTokenForward(TOKEN_RB);
				tokenStream.AcceptTokenForward(TOKEN_SEMICOLON);
			} else {
				//must be a UBO
				tokenStream.AcceptString(tok);
				shaderInfo.uniforms.push_back(UniformDef{});
				UniformDef& def = shaderInfo.uniforms.back();
				def.name = tok;
				def.binding = binding;
				def.stageFlag |= shaderInfo.curStage;
				def.dataType = ShaderDataTypeUBO;
				def.readFlag = readFlag;
				def.writeFlag = writeFlag;
				tokenStream.AcceptToken(TOKEN_LB);
				tokenStream.AcceptTokenForward(TOKEN_RB);
				tokenStream.AcceptTokenForward(TOKEN_SEMICOLON);
			}
		} else {
			return false;
		}
		return true;
	}

	bool TryParseBindingUniform(TokenStream& tokenStream, ShaderInfo& shaderInfo) {
		uint16_t binding;
		if (tokenStream.AcceptToken(TOKEN_BINDING)) {
			tokenStream.AcceptToken(TOKEN_EQ);
			tokenStream.AcceptInt16(binding);
		} else {
			return false;
		}
		tokenStream.AcceptTokenForward(TOKEN_RP);
		if (shaderInfo.curStage == kComputeShader) {
			int c = 0;
		}
		if (!TryParseUniform(tokenStream, shaderInfo, binding)) {
			TryParseBuffer(tokenStream, shaderInfo, binding);
		}
	}

	bool TryParsePushConstant(TokenStream& tokenStream, ShaderInfo& shaderInfo) {
		if (!tokenStream.AcceptToken(TOKEN_PUSH_CONSTANT)) {
			return false;
		}
		tokenStream.AcceptToken(TOKEN_RP);
		return TryParseUniform(tokenStream, shaderInfo, 99);
	}

	bool TryParseAttribute(TokenStream& tokenStream, ShaderInfo& shaderInfo) {
		uint16_t location;
		if (tokenStream.AcceptToken(TOKEN_LOCATION)) {
			tokenStream.AcceptToken(TOKEN_EQ);
			tokenStream.AcceptInt16(location);
		} else {
			return false;
		}
		tokenStream.AcceptToken(TOKEN_RP);

		//attribute
		if (tokenStream.AcceptToken(TOKEN_IN)) {
			shaderInfo.attrs.resize(shaderInfo.attrs.size() + 1);
			VertexDef& vd = shaderInfo.attrs[shaderInfo.attrs.size() - 1];
			vd.location = location;
			//dataType
			{
				std::string tok;
				bool flag = tokenStream.AcceptDataTypeToken(tok);
				SET_DATATYPE_BY_TOKEN(vd.dataType, tok)
			}

			//attrib
			{
				if (tokenStream.AcceptToken(TOKEN_IN_POSITION)) {
					vd.attr = VERTEX_POS;
				}
				else if (tokenStream.AcceptToken(TOKEN_IN_COLOR)) {
					vd.attr = VERTEX_COLOR;
				}
				else if (tokenStream.AcceptToken(TOKEN_IN_TEXCOORD)) {
					vd.attr = VERTEX_TEXCOORD;
				}
				else if (tokenStream.AcceptToken(TOKEN_IN_NORMAL)) {
					vd.attr = VERTEX_NORMAL;
				}
				else if (tokenStream.AcceptToken(TOKEN_INSTANCE_POSITION)) {
					vd.attr = VERTEX_POS;
					vd.instancing = true;
				}
			}
			tokenStream.AcceptToken(TOKEN_SEMICOLON);
			return true;
		}

		return true;
	}

	void GetTokenStream(char* buffer, uint32_t idx, uint32_t size, TokenStream& tokenStream) {
		char c;
		int tokenIdx = 0;
		tokenStream.resize(10000);
		while (idx < size) {
			c = buffer[idx];
			switch (c) {
			case '\n':
			case ' ':
			case '\r':
			case '\0': if (!tokenStream[tokenIdx].empty()) ++tokenIdx; break;
			case '(':
			case '=':
			case ';':
			case ')': if (!tokenStream[tokenIdx].empty()) ++tokenIdx; tokenStream[tokenIdx] = c; ++tokenIdx; break;
			default: tokenStream[tokenIdx] += c; break;
			}
			++idx;
		}
		tokenStream.size = idx;
	}

	void ParseVertexShader(char* buffer, uint32_t idx, uint32_t size, ShaderInfo& shaderInfo) {
		TokenStream tokenStream;
		GetTokenStream(buffer, idx, size, tokenStream);
		shaderInfo.curStage = kVertexShader;
		if (tokenStream.AcceptToken(TOKEN_GL_VERSION)) {
			tokenStream.AcceptInt16(shaderInfo.version);

			bool flag = true;
			//Accept all layout info
			while (flag) {
				flag = false;
				if (tokenStream.AcceptTokenForward(TOKEN_LAYOUT)) {
					flag = true;
					tokenStream.AcceptToken(TOKEN_LP);
					if (!TryParseAttribute(tokenStream, shaderInfo)) {
						//uniform
						if (!TryParseBindingUniform(tokenStream, shaderInfo)) {
							TryParsePushConstant(tokenStream, shaderInfo);
						}
					}
				} else if (TryParseUniform(tokenStream, shaderInfo)) {
					flag = true;
				}
			}
		} else {
			LOGERROR("[SHADERPARSER] NO GL VERSION FOUND");
		}
	}

	void ParseFragmentShader(char* buffer, uint32_t idx, uint32_t size, ShaderInfo& shaderInfo) {
		TokenStream tokenStream;
		GetTokenStream(buffer, idx, size, tokenStream);

		shaderInfo.curStage = kFragmentShader;
		if (tokenStream.AcceptToken(TOKEN_GL_VERSION)) {
			tokenStream.AcceptInt16(shaderInfo.version);

			bool flag = true;
			//Accept all layout info
			while (flag) {
				flag = false;
				if (tokenStream.AcceptTokenForward(TOKEN_LAYOUT)) {
					flag = true;
					tokenStream.AcceptToken(TOKEN_LP);
					//if (!TryParseAttribute(tokenStream, shaderInfo)) {
						//uniform
					//}
					if (!TryParseBindingUniform(tokenStream, shaderInfo) && !TryParsePushConstant(tokenStream, shaderInfo)) {
						tokenStream.AcceptTokenForward(TOKEN_RP);
						tokenStream.AcceptTokenForward(TOKEN_SEMICOLON);
					}
				}
				else if (TryParseUniform(tokenStream, shaderInfo)) {
					flag = true;
				}
			}
		}
		else {
			LOGERROR("[SHADERPARSER] NO GL VERSION FOUND");
		}
	}


	bool TryParseBindingBuffer(TokenStream& tokenStream, ShaderInfo& shaderInfo) {
		uint16_t binding;
		if (tokenStream.AcceptToken(TOKEN_BINDING)) {
			tokenStream.AcceptToken(TOKEN_EQ);
			tokenStream.AcceptInt16(binding);
		} else {
			return false;
		}

		if (tokenStream.AcceptToken(TOKEN_COMMA)) {
			tokenStream.AcceptTokenForward(TOKEN_RP);
		} else {
			tokenStream.AcceptToken(TOKEN_RP);
		}

		return TryParseBuffer(tokenStream, shaderInfo, binding);
	}
	void ParseComputeShader(char* buffer, uint32_t idx, uint32_t size, ShaderInfo& shaderInfo) {
		TokenStream tokenStream;
		GetTokenStream(buffer, idx, size, tokenStream);

		shaderInfo.curStage = kComputeShader;
		if (tokenStream.AcceptToken(TOKEN_GL_VERSION)) {
			tokenStream.AcceptInt16(shaderInfo.version);

			bool flag = true;
			//Accept all layout info
			while (flag) {
				flag = false;
				if (tokenStream.AcceptTokenForward(TOKEN_LAYOUT)) {
					flag = true;
					tokenStream.AcceptToken(TOKEN_LP);
					//if (!TryParseAttribute(tokenStream, shaderInfo)) {
						//uniform
					//}
					if (!TryParseBindingUniform(tokenStream, shaderInfo) && !TryParsePushConstant(tokenStream, shaderInfo)) {
						tokenStream.AcceptTokenForward(TOKEN_RP);
						tokenStream.AcceptTokenForward(TOKEN_SEMICOLON);
					}
				}
				else if (TryParseUniform(tokenStream, shaderInfo)) {
					flag = true;
				}
			}
		}
		else {
			LOGERROR("[SHADERPARSER] NO GL VERSION FOUND");
		}
	}

	void ShaderParser::ParseShader(std::string& name, ShaderInfo& info, U32 stage) {
		FileSystem* fs = GetSubsystem<FileSystem>();
		const char* dir = fs->GetShaderDir();

		if (stage & kVertexShader) {
			std::string vertPath = dir + name + ".vert";
			File* vertFile = fs->ReadFile(vertPath.c_str());
			ParseVertexShader((char*)vertFile->GetBuffer(), 0, vertFile->Size(), info);
		}

		if (stage & kFragmentShader) {
			std::string fragPath = dir + name + ".frag";
			File* fragFile = fs->ReadFile(fragPath.c_str());
			ParseFragmentShader((char*)fragFile->GetBuffer(), 0, fragFile->Size(), info);
		}

		if (stage & kComputeShader) {
			std::string compPath = dir + name + ".comp";
			File* compFile = fs->ReadFile(compPath.c_str());
			ParseComputeShader((char*)compFile->GetBuffer(), 0, compFile->Size(), info);
		}
	}
}
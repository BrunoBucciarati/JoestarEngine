//#include "ShaderParser.h"
//#include "../../IO/FileSystem.h"
//#include "../../IO/Log.h"
//#include "../../Container/Vector.h"
//#include "../../Container/Str.h"
//#define TOKEN_LAYOUT "layout"
//#define TOKEN_BINDING "binding"
//#define TOKEN_LOCATION "location"
//#define TOKEN_EQ "="
//#define TOKEN_LP "("
//#define TOKEN_RP ")"
//#define TOKEN_TYPE_VOID "void"
//#define TOKEN_GL_VERSION "#version"
//#define TOKEN_IN "in"
//#define TOKEN_UNIFORM "uniform"
//#define TOKEN_FLOAT "float"
//#define TOKEN_VEC2 "vec2"
//#define TOKEN_VEC3 "vec3"
//#define TOKEN_VEC4 "vec4"
//#define TOKEN_MAT4 "mat4"
//#define TOKEN_SAMPLER2D "sampler2D"
//#define TOKEN_SAMPLERCUBE "samplerCube"
//#define TOKEN_SAMPLER3D "sampler3D"
//#define TOKEN_IMAGE2D "image2D"
//#define TOKEN_IMAGECUBE "imageCube"
//#define TOKEN_LB "{"
//#define TOKEN_RB "}"
//#define TOKEN_SEMICOLON ";"
//#define TOKEN_IN_POSITION "inPosition"
//#define TOKEN_INSTANCE_POSITION "instancePos"
//#define TOKEN_IN_COLOR "inColor"
//#define TOKEN_IN_TEXCOORD "inTexCoord"
//#define TOKEN_IN_NORMAL "inNormal"
//#define TOKEN_PUSH_CONSTANT "push_constant"
//#define TOKEN_BUFFER "buffer"
//#define TOKEN_COMMA ","
//#define TOKEN_READONLY "readonly"
//#define TOKEN_WRITEONLY "writeonly"
//#define TOKEN_INCLUDE "#include"
//#define TOKEN_QM "\""
//
//#define SET_DATATYPE_BY_TOKEN(val, token) \
//	if (token == TOKEN_FLOAT) val = ShaderDataTypeFloat; \
//	if (token == TOKEN_VEC2) val = ShaderDataTypeVec2; \
//	if (token == TOKEN_VEC3) val = ShaderDataTypeVec3; \
//	if (token == TOKEN_VEC4) val = ShaderDataTypeVec4; \
//	if (token == TOKEN_MAT4) val = ShaderDataTypeMat4;\
//	if (token == TOKEN_SAMPLER2D) val = SamplerType2D; \
//	if (token == TOKEN_SAMPLERCUBE) val = SamplerTypeCube; \
//	if (token == TOKEN_SAMPLER3D) val = SamplerType3D; \
//	if (token == TOKEN_IMAGE2D) val = ShaderDataTypeImage2D; \
//	if (token == TOKEN_IMAGECUBE) val = ShaderDataTypeImageCube; 
//
//#define SET_SAMPLERTYPE_BY_TOKEN(val, token) \
//	if (token == TOKEN_SAMPLER2D) val = SamplerType2D; \
//	if (token == TOKEN_SAMPLERCUBE) val = SamplerTypeCube; \
//	if (token == TOKEN_SAMPLER3D) val = SamplerType3D;
//
//namespace Joestar {
//	ShaderParser::~ShaderParser() {
//
//	}
//
//	struct TokenStream {
//		Vector<String> tokens;
//		U32 curIdx;
//		U32 size;
//
//		void Resize(uint32_t size) {
//			tokens.Resize(size);
//			curIdx = 0;
//		}
//
//		String& operator[](int idx) {
//			return tokens[idx];
//		}
//
//		bool AcceptToken(const char* token) {
//			if (tokens[curIdx] == token) {
//				++curIdx;
//				return true;
//			}
//			return false;
//		}
//
//		bool AcceptTokenForward(const char* token) {
//			while (curIdx < size) {
//				if (tokens[curIdx] == token) {
//					++curIdx;
//					return true;
//				} else {
//					++curIdx;
//				}
//			}
//			return false;
//		}
//
//		bool AcceptInt16(uint16_t& i) {
//			int n = atoi(tokens[curIdx].CString());
//			i = n;
//			++curIdx;
//			return true;
//		}
//
//		bool AcceptInt(uint32_t& i) {
//			int n = atoi(tokens[curIdx].CString());
//			i = n;
//			++curIdx;
//			return true;
//		}
//
//		bool AcceptString(String& str) {
//			str = tokens[curIdx];
//			++curIdx;
//			return true;
//		}
//
//#define TRY_ACCEPT_DATATYPE_TOKEN(t) \
//	if (AcceptToken(t)) { \
//		str = t; \
//		return true; \
//	}
//
//		bool AcceptDataTypeToken(String& str) {
//			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_FLOAT);
//			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_VEC2);
//			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_VEC3);
//			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_VEC4);
//			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_MAT4);
//			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_SAMPLER2D);
//			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_SAMPLERCUBE);
//			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_SAMPLER3D);
//			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_IMAGE2D);
//			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_IMAGECUBE);
//			return false;
//		}
//
//		bool AcceptSamplerTypeToken(String& str) {
//			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_SAMPLER2D);
//			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_SAMPLERCUBE);
//			TRY_ACCEPT_DATATYPE_TOKEN(TOKEN_SAMPLER3D);
//			return false;
//		}
//	};
//
//	bool TryParseBuffer(TokenStream& tokenStream, ShaderInfo& shaderInfo, uint16_t binding = 0) {
//		if (tokenStream.AcceptToken(TOKEN_BUFFER)) {
//			String tok;
//			//must be a UBO
//			tokenStream.AcceptString(tok);
//			shaderInfo.uniforms.push_back(UniformDef{});
//			UniformDef& def = shaderInfo.uniforms.back();
//			def.name = tok;
//			def.binding = binding;
//			def.stageFlag |= shaderInfo.curStage;
//			def.dataType = ShaderDataTypeBuffer;
//			tokenStream.AcceptToken(TOKEN_LB);
//			tokenStream.AcceptTokenForward(TOKEN_RB);
//			tokenStream.AcceptTokenForward(TOKEN_SEMICOLON);
//		}
//		else {
//			return false;
//		}
//		return true;
//	}
//
//	bool TryParseUniform(TokenStream& tokenStream, ShaderInfo& shaderInfo, uint16_t binding = 0) {
//		if (tokenStream.AcceptToken(TOKEN_UNIFORM)) {
//			bool readFlag = true, writeFlag = true;
//			if (tokenStream.AcceptToken(TOKEN_READONLY)) {
//				writeFlag = false;
//			}
//			if (tokenStream.AcceptToken(TOKEN_WRITEONLY)) {
//				readFlag = false;
//			}
//			String tok;
//			if (tokenStream.AcceptDataTypeToken(tok)) {
//				shaderInfo.uniforms.push_back(UniformDef{});
//				UniformDef& def = shaderInfo.uniforms.back();
//
//				SET_DATATYPE_BY_TOKEN(def.dataType, tok)
//				tokenStream.AcceptString(def.name);
//				def.binding = binding;
//				def.stageFlag |= shaderInfo.curStage;
//				def.readFlag = readFlag;
//				def.writeFlag = writeFlag;
//				tokenStream.AcceptTokenForward(TOKEN_SEMICOLON);
//			} else {
//				//must be a UBO
//				tokenStream.AcceptString(tok);
//				int existIdx = -1;
//				for (int i = 0; i < shaderInfo.uniforms.size(); ++i) {
//					if (shaderInfo.uniforms[i].name == tok) {
//						existIdx = i;
//						break;
//					}
//				}
//				if (existIdx > -1) {
//					UniformDef& def = shaderInfo.uniforms[existIdx];
//					def.stageFlag |= shaderInfo.curStage;
//				} else {
//					shaderInfo.uniforms.push_back(UniformDef{});
//					UniformDef& def = shaderInfo.uniforms.back();
//					def.name = tok;
//					def.binding = binding;
//					def.stageFlag |= shaderInfo.curStage;
//					//special handle
//					def.dataType = binding == 99 ? ShaderDataTypePushConst : ShaderDataTypeUBO;
//					def.readFlag = readFlag;
//					def.writeFlag = writeFlag;
//				}
//				tokenStream.AcceptToken(TOKEN_LB);
//				tokenStream.AcceptTokenForward(TOKEN_RB);
//				tokenStream.AcceptTokenForward(TOKEN_SEMICOLON);
//			}
//		} else {
//			return false;
//		}
//		return true;
//	}
//
//	bool TryParseBindingUniform(TokenStream& tokenStream, ShaderInfo& shaderInfo) {
//		uint16_t binding;
//		if (tokenStream.AcceptToken(TOKEN_BINDING)) {
//			tokenStream.AcceptToken(TOKEN_EQ);
//			tokenStream.AcceptInt16(binding);
//		} else {
//			return false;
//		}
//		tokenStream.AcceptTokenForward(TOKEN_RP);
//		if (shaderInfo.curStage == kComputeShader) {
//			int c = 0;
//		}
//		if (!TryParseUniform(tokenStream, shaderInfo, binding)) {
//			TryParseBuffer(tokenStream, shaderInfo, binding);
//		}
//	}
//
//	bool TryParsePushConstant(TokenStream& tokenStream, ShaderInfo& shaderInfo) {
//		if (!tokenStream.AcceptToken(TOKEN_PUSH_CONSTANT)) {
//			return false;
//		}
//		tokenStream.AcceptToken(TOKEN_RP);
//		return TryParseUniform(tokenStream, shaderInfo, 99);
//	}
//
//	bool TryParseAttribute(TokenStream& tokenStream, ShaderInfo& shaderInfo) {
//		uint16_t location;
//		if (tokenStream.AcceptToken(TOKEN_LOCATION)) {
//			tokenStream.AcceptToken(TOKEN_EQ);
//			tokenStream.AcceptInt16(location);
//		} else {
//			return false;
//		}
//		tokenStream.AcceptToken(TOKEN_RP);
//
//		//attribute
//		if (tokenStream.AcceptToken(TOKEN_IN)) {
//			shaderInfo.attrs.resize(shaderInfo.attrs.size() + 1);
//			VertexDef& vd = shaderInfo.attrs[shaderInfo.attrs.size() - 1];
//			vd.location = location;
//			//dataType
//			{
//				String tok;
//				bool flag = tokenStream.AcceptDataTypeToken(tok);
//				SET_DATATYPE_BY_TOKEN(vd.dataType, tok)
//			}
//
//			//attrib
//			{
//				if (tokenStream.AcceptToken(TOKEN_IN_POSITION)) {
//					vd.attr = VERTEX_POS;
//				}
//				else if (tokenStream.AcceptToken(TOKEN_IN_COLOR)) {
//					vd.attr = VERTEX_COLOR;
//				}
//				else if (tokenStream.AcceptToken(TOKEN_IN_TEXCOORD)) {
//					vd.attr = VERTEX_TEXCOORD;
//				}
//				else if (tokenStream.AcceptToken(TOKEN_IN_NORMAL)) {
//					vd.attr = VERTEX_NORMAL;
//				}
//				else if (tokenStream.AcceptToken(TOKEN_INSTANCE_POSITION)) {
//					vd.attr = VERTEX_POS;
//					vd.instancing = true;
//				}
//			}
//			tokenStream.AcceptToken(TOKEN_SEMICOLON);
//			return true;
//		}
//
//		return true;
//	}
//
//	void GetTokenStream(char* buffer, uint32_t idx, uint32_t size, TokenStream& tokenStream) {
//		char c;
//		int tokenIdx = 0;
//		tokenStream.Resize(10000);
//		while (idx < size) {
//			c = buffer[idx];
//			switch (c) {
//			case '\n':
//			case ' ':
//			case '\r':
//			case '\0': if (!tokenStream[tokenIdx].Empty()) ++tokenIdx; break;
//			case '(':
//			case '=':
//			case ';':
//			case '\"':
//			case ')': if (!tokenStream[tokenIdx].Empty()) ++tokenIdx; tokenStream[tokenIdx] = c; ++tokenIdx; break;
//			default: tokenStream[tokenIdx] += c; break;
//			}
//			++idx;
//		}
//		tokenStream.size = idx;
//	}
//
//	void ShaderParser::ParseVertexShader(char* buffer, U32 idx, U32 size, ShaderInfo& shaderInfo, String& newFile) {
//		TokenStream tokenStream;
//		GetTokenStream(buffer, idx, size, tokenStream);
//		shaderInfo.curStage = kVertexShader;
//		newFile = "";
//		String oldStr = buffer;
//		if (tokenStream.AcceptToken(TOKEN_GL_VERSION)) {
//			tokenStream.AcceptInt16(shaderInfo.version);
//			
//			bool flag = true;
//			ParseInclude(tokenStream, oldStr, newFile);
//			//Accept all layout info
//			while (flag) {
//				flag = false;
//				if (tokenStream.AcceptTokenForward(TOKEN_LAYOUT)) {
//					flag = true;
//					tokenStream.AcceptToken(TOKEN_LP);
//					if (!TryParseAttribute(tokenStream, shaderInfo)) {
//						//uniform
//						if (!TryParseBindingUniform(tokenStream, shaderInfo)) {
//							TryParsePushConstant(tokenStream, shaderInfo);
//						}
//					}
//				} else if (TryParseUniform(tokenStream, shaderInfo)) {
//					flag = true;
//				}
//			}
//		} else {
//			LOGERROR("[SHADERPARSER] NO GL VERSION FOUND");
//		}
//	}
//
//	bool ShaderParser::ParseInclude(TokenStream& tokenStream, String& oldStr, String& newFile) {
//		bool flag = true;
//		U32 includeStart = 0, includeEnd = 0;
//		//Accept all include info
//		while (flag) {
//			flag = false;
//			if (tokenStream.AcceptToken(TOKEN_INCLUDE)) {
//				flag = true;
//				tokenStream.AcceptToken(TOKEN_QM);
//				String inc;
//				tokenStream.AcceptString(inc);
//				tokenStream.AcceptToken(TOKEN_QM);
//
//				String incPath = shaderDir + inc;
//				File* file = fs->ReadFile(incPath.CString());
//				if (includeStart == 0) {
//					includeStart = oldStr.Find(TOKEN_INCLUDE);
//					newFile += oldStr.SubString(0, includeStart);
//				}
//				newFile += file->GetBuffer();
//				includeEnd = oldStr.Find(inc + TOKEN_QM) + inc.Length() + 1;
//			}
//		}
//		if (includeEnd > 0) {
//			newFile += oldStr.SubString(includeEnd, oldStr.Length() - includeEnd);
//		} else {
//			newFile = oldStr;
//		}
//
//		return flag;
//	}
//
//	void ShaderParser::ParseFragmentShader(char* buffer, U32 idx, U32 size, ShaderInfo& shaderInfo, String& newFile) {
//		TokenStream tokenStream;
//		String oldStr = buffer;
//		GetTokenStream(buffer, idx, size, tokenStream);
//		newFile = "";
//		shaderInfo.curStage = kFragmentShader;
//		if (tokenStream.AcceptToken(TOKEN_GL_VERSION)) {
//			tokenStream.AcceptInt16(shaderInfo.version);
//
//			bool flag = true;
//			ParseInclude(tokenStream, oldStr, newFile);
//			//Accept all layout info
//			while (flag) {
//				flag = false;
//				if (tokenStream.AcceptTokenForward(TOKEN_LAYOUT)) {
//					flag = true;
//					tokenStream.AcceptToken(TOKEN_LP);
//					//if (!TryParseAttribute(tokenStream, shaderInfo)) {
//						//uniform
//					//}
//					if (!TryParseBindingUniform(tokenStream, shaderInfo) && !TryParsePushConstant(tokenStream, shaderInfo)) {
//						tokenStream.AcceptTokenForward(TOKEN_RP);
//						tokenStream.AcceptTokenForward(TOKEN_SEMICOLON);
//					}
//				}
//				else if (TryParseUniform(tokenStream, shaderInfo)) {
//					flag = true;
//				}
//			}
//		}
//		else {
//			LOGERROR("[SHADERPARSER] NO GL VERSION FOUND");
//		}
//	}
//
//
//	bool TryParseBindingBuffer(TokenStream& tokenStream, ShaderInfo& shaderInfo) {
//		uint16_t binding;
//		if (tokenStream.AcceptToken(TOKEN_BINDING)) {
//			tokenStream.AcceptToken(TOKEN_EQ);
//			tokenStream.AcceptInt16(binding);
//		} else {
//			return false;
//		}
//
//		if (tokenStream.AcceptToken(TOKEN_COMMA)) {
//			tokenStream.AcceptTokenForward(TOKEN_RP);
//		} else {
//			tokenStream.AcceptToken(TOKEN_RP);
//		}
//
//		return TryParseBuffer(tokenStream, shaderInfo, binding);
//	}
//	void ShaderParser::ParseComputeShader(char* buffer, uint32_t idx, uint32_t size, ShaderInfo& shaderInfo, String& newFile) {
//		TokenStream tokenStream;
//		GetTokenStream(buffer, idx, size, tokenStream);
//
//		String oldStr = buffer;
//		shaderInfo.curStage = kComputeShader;
//		if (tokenStream.AcceptToken(TOKEN_GL_VERSION)) {
//			tokenStream.AcceptInt16(shaderInfo.version);
//
//			bool flag = true;
//			ParseInclude(tokenStream, oldStr, newFile);
//			//Accept all layout info
//			while (flag) {
//				flag = false;
//				if (tokenStream.AcceptTokenForward(TOKEN_LAYOUT)) {
//					flag = true;
//					tokenStream.AcceptToken(TOKEN_LP);
//					//if (!TryParseAttribute(tokenStream, shaderInfo)) {
//						//uniform
//					//}
//					if (!TryParseBindingUniform(tokenStream, shaderInfo) && !TryParsePushConstant(tokenStream, shaderInfo)) {
//						tokenStream.AcceptTokenForward(TOKEN_RP);
//						tokenStream.AcceptTokenForward(TOKEN_SEMICOLON);
//					}
//				}
//				else if (TryParseUniform(tokenStream, shaderInfo)) {
//					flag = true;
//				}
//			}
//		}
//		else {
//			LOGERROR("[SHADERPARSER] NO GL VERSION FOUND");
//		}
//	}
//
//	ShaderParser::ShaderParser(EngineContext* ctx) : Super(ctx) {
//		fs = GetSubsystem<FileSystem>();
//		shaderDir = fs->GetShaderDirAbsolute();
//		outputDir = shaderDir + "spirv/";
//	}
//
//	void ShaderParser::ParseShader(String& name, ShaderInfo& info, U32 stage) {
//		if (stage & kVertexShader) {
//			String newFile;
//			String vertPath = shaderDir + name + ".vert";
//			File* vertFile = fs->ReadFile(vertPath.CString());
//			ParseVertexShader((char*)vertFile->GetBuffer(), 0, vertFile->Size(), info, newFile);
//			String newPath = outputDir + name + ".vert";
//			fs->WriteFile(newPath.CString(), newFile);
//		}
//
//		if (stage & kFragmentShader) {
//			String newFile;
//			String fragPath = shaderDir + name + ".frag";
//			File* fragFile = fs->ReadFile(fragPath.CString());
//			ParseFragmentShader((char*)fragFile->GetBuffer(), 0, fragFile->Size(), info, newFile);
//			String newPath = outputDir + name + ".frag";
//			fs->WriteFile(newPath.CString(), newFile);
//		}
//
//		if (stage & kComputeShader) {
//			String newFile;
//			String compPath = shaderDir + name + ".comp";
//			File* compFile = fs->ReadFile(compPath.CString());
//			ParseComputeShader((char*)compFile->GetBuffer(), 0, compFile->Size(), info, newFile);
//			String newPath = outputDir + name + ".comp";
//			fs->WriteFile(newPath.CString(), newFile);
//		}
//	}
//}
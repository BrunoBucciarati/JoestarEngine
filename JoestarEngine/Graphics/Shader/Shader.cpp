#include "Shader.h"
#include "ShaderParser.h"

namespace Joestar {
	Shader::~Shader() {

	}
	void Shader::SetShader(String n, U32 f) {
		name = n;
		id = hashString(n.CString());
		GetSubsystem<ShaderParser>()->ParseShader(n, info, f);
		flag = f;
	}
	U32 Shader::GetVertexAttributeFlag() {
		U32 flag = 0;
		for (int i = 0; i < info.attrs.size(); ++i) {
			if (!info.attrs[i].instancing)
				flag = flag | 1 << info.attrs[i].attr;
		}
		return flag;
	}

	U32 Shader::GetInstanceAttributeFlag() {
		U32 flag = 0;
		for (int i = 0; i < info.attrs.size(); ++i) {
			if (info.attrs[i].instancing)
				flag = flag | 1 << info.attrs[i].attr;
		}
		return flag;
	}

	U16 Shader::GetUniformBindingByName(String& name) {
		//for (int i = 0; i < info.uniforms.size(); i++) {
		//	if (info.uniforms[i].name == name) {
		//		return info.uniforms[i].binding;
		//	}
		//}
		return 0;
	}

	U16 Shader::GetUniformBindingByHash(U32 hash) {
		for (int i = 0; i < info.uniforms.size(); i++) {
			if (hashString(info.uniforms[i].name.CString()) == hash) {
				return info.uniforms[i].binding;
			}
		}
	}

	UniformDef& Shader::GetUniformDefByHash(U32 hash) {
		for (int i = 0; i < info.uniforms.size(); i++) {
			if (hashString(info.uniforms[i].name.CString()) == hash) {
				return info.uniforms[i];
			}
		}
	}
	UniformDef& Shader::GetUniformDef(U8 b) {
		for (int i = 0; i < info.uniforms.size(); i++) {
			if (info.uniforms[i].binding == b) {
				return info.uniforms[i];
			}
		}
	}

	U16 Shader::GetSamplerBinding(int count) {
		for (int i = 0; i < info.uniforms.size(); i++) {
			if (info.uniforms[i].dataType > ShaderDataTypeSampler) {
				if (count == 0)
					return info.uniforms[i].binding;
				--count;
			}
		}
		return 0;
	}

	UniformDef& Shader::GetPushConsts() {
		for (int i = 0; i < info.uniforms.size(); i++) {
			if (info.uniforms[i].dataType == ShaderDataTypePushConst) {
				return info.uniforms[i];
			}
		}
		return info.uniforms[0];
	}
}
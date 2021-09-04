#include "Shader.h"
#include "ShaderParser.h"

namespace Joestar {
	void Shader::SetShader(std::string n, U32 flag) {
		name = n;
		id = hashString(n.c_str());
		GetSubsystem<ShaderParser>()->ParseShader(n, info, flag);
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

	U16 Shader::GetUniformBindingByName(std::string& name) {
		for (int i = 0; i < info.uniforms.size(); i++) {
			if (info.uniforms[i].name == name) {
				return info.uniforms[i].binding;
			}
		}
	}

	U16 Shader::GetUniformBindingByHash(U32 hash) {
		for (int i = 0; i < info.uniforms.size(); i++) {
			if (hashString(info.uniforms[i].name.c_str()) == hash) {
				return info.uniforms[i].binding;
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

	std::string Shader::GetPushConsts() {
		for (int i = 0; i < info.uniforms.size(); i++) {
			if (info.uniforms[i].dataType == ShaderDataTypePushConst) {
				return info.uniforms[i].name;
			}
		}
		return "";
	}
}
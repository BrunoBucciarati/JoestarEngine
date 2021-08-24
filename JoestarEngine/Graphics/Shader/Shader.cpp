#include "Shader.h"
#include "ShaderParser.h"

namespace Joestar {
	void Shader::SetName(std::string n) {
		name = n;
		id = hashString(n.c_str()); 
		GetSubsystem<ShaderParser>()->ParseShader(n, info);
	}

	uint32_t Shader::GetVertexAttributeFlag() {
		uint32_t flag = 0;
		for (int i = 0; i < info.attrs.size(); ++i) {
			flag = flag | 1 << info.attrs[i].attr;
		}
		return flag;
	}
}
#include "Shader.h"
#include "ShaderParser.h"

namespace Joestar {
	void Shader::SetName(std::string n) {
		name = n;
		id = hashString(n.c_str()); 
		GetSubsystem<ShaderParser>()->ParseShader(n, info);
	}
}
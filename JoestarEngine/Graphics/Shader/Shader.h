#pragma once
#include <string>
#include <vector>
#include "../GraphicDefines.h"
#include "../../Base/Object.h"
#include "../../Base/StringHash.h"
#include "ShaderParser.h"
namespace Joestar {
	class Shader : public Object {
		REGISTER_OBJECT(Shader, Object)
	public:
		explicit Shader(EngineContext* ctx) : Super(ctx) {}
		inline std::string& GetName() { return name; };
		void SetName(std::string n);
		std::vector<VERTEX_ATTRIBUTE> attrs;
		std::vector<std::string> uniforms;
		std::vector<std::string> samplers;
		std::vector<UniformBufferObject> ubos;
		uint32_t id;
	private:
		std::string name;
		ShaderInfo info;
	};
}
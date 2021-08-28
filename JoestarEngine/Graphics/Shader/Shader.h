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
		uint32_t GetVertexAttributeFlag();
		uint16_t GetUniformBindingByName(std::string& name);
		uint16_t GetSamplerBinding(int count);
		uint32_t id;
		ShaderInfo info;
	private:
		std::string name;
	};
}
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
		uint32_t id;
	private:
		std::string name;
		ShaderInfo info;
	};
}
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
		void SetShader(std::string n, U32 flag = kVertexShader | kFragmentShader);
		U32 GetVertexAttributeFlag();
		U32 GetInstanceAttributeFlag();
		U16 GetUniformBindingByName(std::string& name);
		U16 GetUniformBindingByHash(U32 hash);
		U16 GetSamplerBinding(int count);
		UniformDef& GetPushConsts();
		U32 id;
		U32 flag;
		ShaderInfo info;
	private:
		std::string name;
	};
}
#pragma once
#include <string>
#include <vector>
#include "../GraphicDefines.h"
#include "../../Core/Object.h"
#include "../../Base/StringHash.h"
#include "ShaderParser.h"
namespace Joestar {
	class Shader : public Object {
		REGISTER_OBJECT(Shader, Object)
	public:
		explicit Shader(EngineContext* ctx) : Super(ctx) {}
		inline String& GetName() { return name; };
		void SetShader(String n, U32 flag = kVertexShader | kFragmentShader);
		U32 GetVertexAttributeFlag();
		U32 GetInstanceAttributeFlag();
		U16 GetUniformBindingByName(String& name);
		U16 GetUniformBindingByHash(U32 hash);
		U16 GetSamplerBinding(int count);
		UniformDef& GetUniformDefByHash(U32 hash);
		UniformDef& GetPushConsts();
		UniformDef& GetUniformDef(U8 b);
		U32 id;
		U32 flag;
		ShaderInfo info;
	private:
		String name;
	};
}
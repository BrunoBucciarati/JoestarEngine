#pragma once
#include "GraphicDefines.h"
#include "VertexData.h"
#include "../Core/Object.h"
#include "Shader/Shader.h"
namespace Joestar {
	class ProgramCPU : public Object {
		REGISTER_OBJECT(ProgramCPU, Object)
	public:
		explicit ProgramCPU(EngineContext* ctx);
		//VertexBuffer* vb;
		//IndexBuffer* ib;
		//Shader* shader;
	};
}
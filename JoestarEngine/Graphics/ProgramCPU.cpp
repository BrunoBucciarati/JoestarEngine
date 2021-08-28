#include "ProgramCPU.h"

namespace Joestar {
	ProgramCPU::ProgramCPU(EngineContext*ctx) : Super(ctx) {
		vb = new VertexBuffer;
		ib = new IndexBuffer;
	}

	ProgramCPU::~ProgramCPU() {
		delete vb;
		delete ib;
	}
}
#pragma once
#include <stdint.h>
enum RenderCommandType {
	RenderCMD_Clear = 0,
	RenderCMD_Draw,
	RenderCMD_DrawIndexed,
	RenderCMD_UpdateUniformBuffer,
	RenderCMD_UpdateVertexBuffer,
	RenderCMD_UpdateIndexBuffer,
	RenderCMD_UseShader
};

struct RenderCommand {
	RenderCommandType typ;
	uint32_t flag;
	uint16_t size;
	void* data;
};
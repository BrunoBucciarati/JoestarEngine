#pragma once
#include <stdint.h>
enum RenderCommandType {
	RenderCMD_Clear,
	RenderCMD_Draw
};

struct RenderCommand {
	RenderCommandType typ;
	uint16_t size;
	void* data;
};
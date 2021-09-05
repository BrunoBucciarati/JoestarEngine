#pragma once
#include <stdint.h>
enum RenderCommandType {
	RenderCMD_Clear = 0,
	RenderCMD_Draw,
	RenderCMD_DrawIndexed,
	RenderCMD_UpdateUniformBuffer,
	RenderCMD_UpdateUniformBufferObject,
	RenderCMD_UpdateVertexBuffer,
	RenderCMD_UpdateIndexBuffer,
	RenderCMD_UpdateInstanceBuffer,
	RenderCMD_UseShader,
	RenderCMD_UpdateTexture,
	RenderCMD_UpdateProgram,
	RenderCMD_BeginRenderPass,
	RenderCMD_EndRenderPass,
	RenderCMD_BeginDrawCall,
	RenderCMD_EndDrawCall,
	RenderCMD_SetDepthCompare,
	RenderCMD_SetDepthWrite,
	RenderCMD_SetPolygonMode,



	RenderCMD_Invalid
};

enum ComputeCommandType {
	ComputeCMD_DispatchCompute = 0,
	ComputeCMD_BeginCompute,
	ComputeCMD_UpdateComputeBuffer,
	ComputeCMD_WriteBackComputeBuffer,
	ComputeCMD_UpdateUniformBuffer,
	ComputeCMD_EndCompute
};

struct RenderCommand {
	RenderCommandType typ;
	uint32_t flag;
	uint16_t size;
	void* data;
};

struct ComputeCommand {
	ComputeCommandType typ;
	uint32_t flag;
	uint16_t size;
	void* data;
};
#pragma once
#ifndef _JOESTAR_GRAPHICS_DEF_H_
#define _JOESTAR_GRAPHICS_DEF_H_
#include "../Math/Matrix4x4.h"
#include "../Platform/Platform.h"

namespace Joestar {
	enum GFX_API {
		GFX_API_OPENGL = 0,
		GFX_API_VULKAN,
		GFX_API_D3D11,
		GFX_API_D3D12,
		GFX_API_INVALID
	};

	enum class ImageLayout {
		UNDEFINED = 0,
		GENERAL = 1,
		COLOR_ATTACHMENT_OPTIMAL = 2,
		DEPTH_STENCIL_ATTACHMENT_OPTIMAL = 3,
		DEPTH_STENCIL_READ_ONLY_OPTIMAL = 4,
		SHADER_READ_ONLY_OPTIMAL = 5,
		TRANSFER_SRC_OPTIMAL = 6,
		TRANSFER_DST_OPTIMAL = 7,
		PREINITIALIZED = 8
	};

	enum class PolygonMode {
		FILL = 0,
		LINE,
		POINT,
	};

	enum class CullMode {
		NONE = 0,
		FRONT = 1,
		BACK = 2,
		FRONT_AND_BACK = 3
	};

	enum FrontFaceMode {
		COUNTER_CLOCKWISE = 0,
		CLOCKWISE = 1,
	};

	enum class CompareOp {
		NEVER = 0,
		LESS = 1,
		EQUAL = 2,
		LESS_EQUAL = 3,
		GREATER = 4,
		NOT_EQUAL = 5,
		GREATER_EQUAL = 6,
		ALWAYS = 7,
	};

	enum class StencilOp {
		KEEP = 0,
		ZERO = 1,
		REPLACE = 2,
		INCREMENT_AND_CLAMP = 3,
		DECREMENT_AND_CLAMP = 4,
		INVERT = 5,
		INCREMENT_AND_WRAP = 6,
		DECREMENT_AND_WRAP = 7
	};

	struct StencilOpState {
		StencilOp    failOp;
		StencilOp    passOp;
		StencilOp    depthFailOp;
		CompareOp    compareOp;
		U32       compareMask;
		U32		  writeMask;
		U32		  reference;
		U32 Hash()
		{
			return (U32)failOp + (U32)passOp + (U32)depthFailOp + (U32)compareOp + compareMask + writeMask + reference;
		}
	};

	enum class BlendFactor {
		ZERO = 0,
		ONE = 1,
		SRC_COLOR = 2,
		ONE_MINUS_SRC_COLOR = 3,
		DST_COLOR = 4,
		ONE_MINUS_DST_COLOR = 5,
		SRC_ALPHA = 6,
		ONE_MINUS_SRC_ALPHA = 7,
		DST_ALPHA = 8,
		ONE_MINUS_DST_ALPHA = 9,
		CONSTANT_COLOR = 10,
		ONE_MINUS_CONSTANT_COLOR = 11,
		CONSTANT_ALPHA = 12,
		ONE_MINUS_CONSTANT_ALPHA = 13,
		SRC_ALPHA_SATURATE = 14,
		SRC1_COLOR = 15,
		ONE_MINUS_SRC1_COLOR = 16,
		SRC1_ALPHA = 17,
		ONE_MINUS_SRC1_ALPHA = 18
	};

	enum class BlendOp {
		ADD = 0,
		SUBTRACT = 1,
		REVERSE_SUBTRACT = 2,
		MIN = 3,
		MAX = 4,
	};

	enum class ColorWriteMask {
		R = 0x00000001,
		G = 0x00000002,
		B = 0x00000004,
		A = 0x00000008,
		ALL = R | G | B | A,
		Color = R | G | B
	};

	const int TEXTURE_FORMAT_SIZE[] = {
		4
	};

	const U32 MAX_CMDLISTS_IN_FLIGHT = 3;

	enum class UniformDataType {
		Vec4 = 0,
		Mat3,
		Mat4,
		Sampler
	};

	const U32 UniformDataTypeSize[] = {
		4 * sizeof(float),
		12 * sizeof(float),
		16 * sizeof(float),
		4
	};

	enum class UniformFrequency {
		PASS = 0,
		BATCH, //Material
		OBJECT
	};

	enum class PerPassUniforms {
		VIEW_MATRIX = 0,
		PROJECTION_MATRIX,
		CAMERA_POS,
		COUNT
	};

	const UniformDataType PerPassUniformTypes[] = {
		UniformDataType::Mat4,
		UniformDataType::Mat4
	};

	enum class PerBatchUniforms {
		TO_ADD = (int)PerPassUniforms::COUNT,
		COUNT
	};

	enum class PerObjectUniforms {
		MODEL_MATRIX = (int)PerBatchUniforms::COUNT,
		FRUSTUM_PLANES,
		COUNT
	};

	UniformDataType GetPerObjectUniformDataType(PerObjectUniforms);
	UniformDataType GetPerBatchUniformDataType(PerBatchUniforms uniform);

	enum class AttachmentLoadOp
	{
		LOAD = 0,
		CLEAR = 1,
		DONT_CARE = 2
	};
	enum class AttachmentStoreOp
	{
		STORE = 0,
		DONT_CARE = 1,
		NONE = 2
	};
	enum class GPUQueue
	{
		GRAPHICS = 0,
		COMPUTE = 1,
		TRANSFER = 2,
		PRESENT = 3	
	};

	enum class VertexSemantic
	{
		INVALID = 0,
		POSITION = 1,
		NORMAL,
		TANGENT,
		COLOR,
		TEXCOORD0,
		TEXCOORD1,
		TEXCOORD2,
		TEXCOORD3,
		TEXCOORD4,
		TEXCOORD5,
		TEXCOORD6,
		TEXCOORD7
	};

	enum class VertexType
	{
		VEC4 = 0,
		VEC3,
		VEC2,
		FLOAT,
		UINT32,
		SINT32,
		UVEC2,
		UVEC3,
		UVEC4,
		SVEC2,
		SVEC3,
		SVEC4
	};

	const U32 VertexTypeSize[] = {
		4 * sizeof(F32),
		3 * sizeof(F32),
		2 * sizeof(F32),
		sizeof(F32),
		sizeof(U8)
	};

	struct VertexElement {
		VertexSemantic semantic;
		VertexType type;
		U32 GetSize()
		{
			return VertexTypeSize[(U32)type];
		}
	};

	enum class ShaderStage {
		VS = 1 << 0,
		PS = 1 << 1,
		HS = 1 << 2,
		DS = 1 << 3,
		GS = 1 << 4,
		CS = 1 << 5,
		VS_PS = VS | PS,
		VS_GS_PS = VS_PS | GS,
		VS_HS_DS_PS = VS_PS | HS | DS
	};

	enum class ShaderLanguage {
		GLSL,
		HLSL,
		SPIRV,
		METAL
	};

	enum class Filter {
		NEAREST = 0,
		LINEAR,
	};
	enum class SamplerAddressMode {
		REPEAT = 0,
		MIRRORED_REPEAT = 1,
		CLAMP_TO_EDGE = 2,
		CLAMP_TO_BORDER = 3,
		MIRROR_CLAMP_TO_EDGE = 4,
	};
	enum class PrimitiveTopology {
		POINT_LIST = 0,
		LINE_LIST = 1,
		LINE_STRIP = 2,
		TRIANGLE_LIST = 3,
		TRIANGLE_STRIP = 4,
		TRIANGLE_FAN = 5,
	};

	const char* GetShaderSuffix(ShaderLanguage lang, ShaderStage);

	const U32 MAX_DESCRIPTOR_SETS = 4;

	VertexSemantic GetMatchingSemantic(const char* name);

	U32 GetUniformID(const char* name);

	
}
#endif
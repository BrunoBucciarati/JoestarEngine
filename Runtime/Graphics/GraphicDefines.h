#pragma once
#ifndef _JOESTAR_GRAPHICS_DEF_H_
#define _JOESTAR_GRAPHICS_DEF_H_
#include "../Math/Matrix4x4.h"
#include <string>
#include "../Platform/Platform.h"

namespace Joestar {
	enum GFX_API {
		GFX_API_OPENGL = 0,
		GFX_API_VULKAN,
		GFX_API_D3D11,
		GFX_API_D3D12,
		GFX_API_INVALID
	};

	enum VERTEX_ATTRIBUTE {
		VERTEX_POS = 0,
		VERTEX_NORMAL,
		VERTEX_COLOR,
		VERTEX_TEXCOORD,
		VERTEX_ATTRIB_COUNT
	};
	const int VERTEX_ATTRIBUTE_SIZE[] = { 3, 3, 3, 2 };

	enum BUILTIN_VALUE {
		BUILTIN_MATRIX_MODEL = 0,
		BUILTIN_MATRIX_VIEW,
		BUILTIN_MATRIX_PROJECTION,
		BUILTIN_MATRIX_COUNT,
		BUILTIN_VEC3_CAMERAPOS,
		BUILTIN_VEC3_SUNDIRECTION,
		BUILTIN_VEC3_SUNCOLOR,
		BUILTIN_VEC3_COUNT,
		BUILTIN_STRUCT_LIGHTBLOCK,
	};

	//enum BUILTIN_MATRIX {
	//};

	//enum BUILTIN_VEC3 {
	//	BUILTIN_VEC3_CAMERAPOS = 0,
	//};

	enum TEXTURE_FORMAT {
		TEXTURE_FORMAT_RGBA8 = 0,
		TEXTURE_FORMAT_FORMATCOUNT
	};

	enum TEXTURE_TYPE {
		TEXTURE_2D = 0,
		TEXTURE_CUBEMAP
	};

	enum DepthCompareFunc {
		DEPTH_COMPARE_NEVER = 0,
		DEPTH_COMPARE_ALWAYS,
		DEPTH_COMPARE_LESS,
		DEPTH_COMPARE_LESSEQUAL,
		DEPTH_COMPARE_GREATER,
		DEPTH_COMPARE_GREATEREQUAL,
		DEPTH_COMPARE_EQUAL,
		DEPTH_COMPARE_NOTEQUAL,
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
		FRAME = 0,
		PASS,
		OBJECT
	};

	enum class BulitinUniforms {
		MODEL_MATRIX = 0,
		VIEW_MATRIX,
		PROJECTION_MATRIX,
		COUNT
	};

	struct UniformType
	{
		UniformDataType dataType;
		UniformFrequency frequency{ UniformFrequency::OBJECT };
	};

	const UniformType BuiltinUniformTypes[] = {
		{ UniformDataType::Mat4, UniformFrequency::OBJECT },
		{ UniformDataType::Mat4, UniformFrequency::PASS },
		{ UniformDataType::Mat4, UniformFrequency::PASS }
	};

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
		PRESENT = 1,
		COMPUTE = 2
	};

	enum class VertexSemantic
	{
		POSITION = 0,
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
		UINT32
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
}
#endif
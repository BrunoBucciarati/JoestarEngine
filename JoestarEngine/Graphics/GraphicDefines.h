#pragma once
#ifndef _JOESTAR_GRAPHICS_DEF_H_
#define _JOESTAR_GRAPHICS_DEF_H_
#include "../Math/Matrix4x4.h"
#include <string>

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

	enum BUILTIN_MATRIX {
		BUILTIN_MATRIX_MODEL = 0,
		BUILTIN_MATRIX_VIEW,
		BUILTIN_MATRIX_PROJECTION
	};

	struct BuiltinMatrixCmdData {
		BUILTIN_MATRIX matrixType;
		void* data;
	};

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

	enum PolygonMode {
		POLYGON_MODE_FILL = 0,
		POLYGON_MODE_LINE
	};

	const int TEXTURE_FORMAT_SIZE[] = {
		4
	};

	struct PushConsts {
		Matrix4x4f model;
	};

	struct UniformBufferObject {
		//Matrix4x4f model;
		Matrix4x4f view;
		Matrix4x4f proj;
		bool operator== (UniformBufferObject& ub) {
			return view == ub.view && proj == ub.proj;//&& model == ub.model;
		}
	};
}
#endif
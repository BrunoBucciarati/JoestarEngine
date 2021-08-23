#pragma once
#ifndef _JOESTAR_GRAPHICS_DEF_H_
#define _JOESTAR_GRAPHICS_DEF_H_
#include "../Math/Matrix4x4.h"

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
	const int TEXTURE_FORMAT_SIZE[] = {
		4
	};


	struct UniformBufferObject {
		Matrix4x4f model;
		Matrix4x4f view;
		Matrix4x4f proj;
		bool operator== (UniformBufferObject& ub) {
			return model == ub.model && view == ub.view && proj == ub.proj;
		}
	};
}
#endif
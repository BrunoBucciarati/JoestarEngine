#pragma once
#include "../Base/ObjectDefines.h"

namespace Joestar {
	struct FrameBufferDef {
		U32 width;
		U32 height;
		bool colorEnabled = true;
		bool depthEnabled = true;
		U32 id;
	};
}
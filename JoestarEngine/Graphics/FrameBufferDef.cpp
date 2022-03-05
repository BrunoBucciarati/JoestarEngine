#include "FrameBufferDef.h"

namespace Joestar {
	static U32 gFBID = 0;
	FrameBufferDef::FrameBufferDef() {
		id = ++gFBID;
	}
}
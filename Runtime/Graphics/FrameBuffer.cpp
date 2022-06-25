#include "FrameBuffer.h"

namespace Joestar {

	U32 FrameBuffer::GetNumColorAttachments()
	{
		for (I32 i = 3; i >= 0; --i)
		{
			if (mColorAttachments[i])
				return i + 1;
		}
		return 0;
	}


	Texture2D* FrameBuffer::GetColorAttachment(U32 idx)
	{
		return mColorAttachments[idx];
	}
}
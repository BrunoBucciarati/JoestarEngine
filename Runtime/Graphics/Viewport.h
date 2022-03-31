#pragma once
#include "../Math/Rect.h"
namespace Joestar
{
	struct Viewport
	{
		Rect rect;
		Rect scissor;
		F32 minDepth{ 0.0f };
		F32 maxDepth{ 1.0f };

		U32 Hash()
		{
			return rect.x + rect.y + rect.width + rect.height +
				scissor.x + scissor.y + scissor.width + scissor.height + minDepth + maxDepth;
		}

		void SetSize(U32 w, U32 h)
		{
			rect.width = w;
			rect.height = h;
			scissor.width = w;
			scissor.height = h;
		}

		void SetScissor(Rect& sc)
		{
			scissor = sc;
		}
	};
}
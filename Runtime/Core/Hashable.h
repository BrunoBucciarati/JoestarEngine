#pragma once
#include "../Platform/Platform.h"
namespace Joestar
{
	class Hashable
	{
	public:
		virtual U32 Hash() const
		{
			return mHash;
		}
	protected:
		U32 mHash;
	};
}
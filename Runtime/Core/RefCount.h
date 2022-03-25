#pragma once
#include "../Platform/Platform.h"
namespace Joestar
{
	class RefCount {
	public:
		RefCount() = default;
		void AddRef()
		{
			++mReference;
		}
		void Release()
		{
			--mReference;
			if (mReference <= 0)
				delete this;
		}
		U32 GetRef() {
			return mReference;
		}
	protected:
		U32 mReference{0};
	};
}
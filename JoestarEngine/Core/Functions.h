#pragma once
namespace Joestar {
	template<typename T> inline void Swap(T& a, T& b)
	{
		T c = a;
		a = b;
		b = c;
	}
}
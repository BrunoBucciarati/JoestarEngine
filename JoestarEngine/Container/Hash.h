#pragma once
#include "../Base/Platform.h"
namespace Joestar {
	template<class T> inline U32 MakeHash(const T& data) {
		return data.Hash();
	}
	template<> inline U32 MakeHash(const int& data) {
		return data;
	}
}
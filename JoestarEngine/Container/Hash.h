#pragma once
#include "../Core/Platform.h"
namespace Joestar {
	template<class T> inline U32 MakeHash(const T& data) {
		return data.Hash();
	}
	template<> inline U32 MakeHash(const int& data) {
		return data;
	}
	template<> inline U32 MakeHash(const U32& data) {
		return data;
	}
	template<> inline U32 MakeHash(const float& data) {
		return data;
	}
	template<> inline U32 MakeHash(const double& data) {
		return data;
	}
	template<> inline U32 MakeHash(const unsigned long long& data) {
		return data;
	}
	template<> inline U32 MakeHash(const long long& data) {
		return data;
	}
}
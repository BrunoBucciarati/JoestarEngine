#pragma once
#include "Hash.h"

namespace Joestar {
	template <class T, class U>
	class Pair {
	public:
		Pair(T f, U s) : key(f), value(s) {}
		Pair() = default;
		U32 Hash() const {
			return MakeHash(key);
		}
		bool operator == (const Pair<T, U>& rhs) {
			return key == rhs.key;
		}
		bool operator != (const Pair<T, U>& rhs) {
			return key != rhs.key;
		}
		T key;
	
		U value;
	};
}
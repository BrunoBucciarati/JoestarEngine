#pragma once
#include "HashSet.h"
namespace Joestar {
	template <class T, class U>
	class KeyValuePair {
	public:
		KeyValuePair(T f, U s) : key(f), value(s) {}
		KeyValuePair() = default;
		U32 Hash() const {
			return MakeHash(key);
		}
		T key;
		U value;
	};

	template<class T, class U>
	class HashMap {
	public:
		bool Insert(T key, U value) {
			KeyValuePair<T, U> pair(key, value);
			return Insert(pair);
		}
		bool Insert(KeyValuePair<T, U>& pair) {
			set.Insert(pair);
			return true;
		}
	private:
		typedef KeyValuePair<T, U> dataType;
		HashSet<dataType> set;
	};
}
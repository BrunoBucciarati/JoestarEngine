#pragma once
namespace Joestar {
	template <class T, class U>
	class KeyValuePair {
	public:
		KeyValuePair(T f, U s) : mKey(f), mValue(s) {}
		T key;
		U value;
	};
	template<class T>
	class HashMap {

	};
}
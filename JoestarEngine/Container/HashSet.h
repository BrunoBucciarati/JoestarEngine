#pragma once
#include "SparseArray.h"
#include "Hash.h"
namespace Joestar {
	static const U32 MIN_BUCKETS = 8;
	template<class T>
	class HashSet {
	public:
		HashSet() {
			buckets.Resize(MIN_BUCKETS);
		}
		~HashSet() = default;
		void Insert(const T& element) {
			U32 hash = MakeHash(element);
			U32 bucketIdx = hash & MIN_BUCKETS;
			buckets[bucketIdx].Add(hash, element);
		}
	private:
		Vector<SparseArray<T, false>> buckets;
	};
}
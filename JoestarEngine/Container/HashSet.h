#pragma once
#include "SparseArray.h"
#include "Hash.h"
namespace Joestar {
	static const U32 MIN_BUCKETS = 8;
	template<class SetElementType>
	class HashSet {
	public:
		//迭代器，如果等于MIN_BUCKETS意味着是End()
		struct Iterator {
			Iterator& operator ++()
			{
				if (bucketIdx < MIN_BUCKETS) {
					Next();
				}
				return *this;
			}
			Iterator& operator --()
			{
				if (bucketIdx < MIN_BUCKETS) {
					Prev();
				}
				return *this;
			}
			void Next() {
				if (arrayIndex == (*buffer)[bucketIdx].Size() - 1) {
					++bucketIdx;
					arrayIndex = 0;
				}
			}
			void Prev() {
				if (arrayIndex == 0) {
					if (bucketIdx == 0)
						bucketIdx = MIN_BUCKETS;
					else
					{
						--bucketIdx;
						arrayIndex = (*buffer)[bucketIdx].Size() - 1;
					}
				}
			}
			SetElementType* Get() {
				return &((*buffer)[bucketIdx][arrayIndex]);
			}

			SetElementType* operator ->() {
				return Get();
			}

			SetElementType& operator *() {
				return *buffer[bucketIdx][arrayIndex];
			}

			bool operator==(const Iterator& rhs) {
				return bucketIdx == rhs.bucketIdx && arrayIndex == rhs.arrayIndex;
			}
			bool operator!=(const Iterator& rhs) {
				return !(*this == rhs);
			}
			U32 bucketIdx{ 0 };
			U32 arrayIndex{ 0 };
			Vector<SparseArray<SetElementType, false>>* buffer{nullptr};
		};

		Iterator End() {
			return tail;
		}

		Iterator Begin() {
			return Iterator{ 0, 0, &buckets };
		}

		Iterator FindByHash(U32 hash) {
			U32 bucketIdx = hash & (MIN_BUCKETS - 1);
			U32 idx = buckets[bucketIdx].FindIndex(hash);
			if (idx != INDEX_NONE) {
				return Iterator{ bucketIdx, idx, &buckets };
			}
			return End();
		}

		Iterator Find(const SetElementType& key) {
			U32 hash = MakeHash(key);
			for (int i = 0; i < buckets[hash].Size(); ++i) {
				if (buckets[hash][i] == key) {
					return Iterator{ hash, i, &buckets };
				}
			}
			return End();
		}

		U32 Size() {
			U32 sz = 0;
			for (int i = 0; i < buckets.Size(); ++i) {
				sz += buckets[i].Size();
			}
			return sz;
		}

		HashSet() {
			buckets.Resize(MIN_BUCKETS);
			tail = {MIN_BUCKETS, 0, &buckets };
		}
		~HashSet() = default;

		Iterator Insert(const SetElementType& element) {
			U32 hash = MakeHash(element);
			U32 bucketIdx = hash & (MIN_BUCKETS - 1);
			buckets[bucketIdx].Add(hash, element);
			return { bucketIdx, buckets[bucketIdx].Size() - 1, &buckets };
		}
	private:
		Vector<SparseArray<SetElementType, false>> buckets;
		Iterator tail;
	};
}
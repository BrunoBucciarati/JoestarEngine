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
				if (bucketIdx < (*buffer).Size()) {
					Next();
				}
				return *this;
			}
			Iterator& operator --()
			{
				if (bucketIdx < numBuckets) {
					Prev();
				}
				return *this;
			}
			void Next()
			{
				if (arrayIndex == (*buffer)[bucketIdx].Size() - 1)
				{
					++bucketIdx;
					while(bucketIdx < (*buffer).Size() && (*buffer)[bucketIdx].Empty())
						++bucketIdx;
					arrayIndex = 0;
				}
				else
					++arrayIndex;
			}
			void Prev()
			{
				if (arrayIndex == 0) {
					if (bucketIdx == 0)
						bucketIdx = (*buffer).Size();
					else
					{
						--bucketIdx;
						arrayIndex = 0;
						while (bucketIdx > 0 && (*buffer)[bucketIdx].Empty())
							--bucketIdx;
						if (!(*buffer)[bucketIdx].Empty())
							arrayIndex = (*buffer)[bucketIdx].Size() - 1;
						else
							bucketIdx = (*buffer).Size();
					}
				}
			}
			SetElementType* Get()
			{
				return &((*buffer)[bucketIdx][arrayIndex]);
			}

			SetElementType* operator ->()
			{
				return Get();
			}

			SetElementType& operator *()
			{
				return (*buffer)[bucketIdx][arrayIndex];
			}

			bool operator==(const Iterator& rhs) {
				return bucketIdx == rhs.bucketIdx && arrayIndex == rhs.arrayIndex;
			}
			bool operator!=(const Iterator& rhs) {
				return !(*this == rhs);
			}

			bool Erase()
			{
				for (U32 i = arrayIndex; i < (*buffer)[bucketIdx].Size() - 1; ++i)
				{
					(*buffer)[bucketIdx][i] = (*buffer)[bucketIdx][i + 1];
				}
				(*buffer)[bucketIdx].Resize((*buffer)[bucketIdx].Size() - 1);
				return true;
			}

			U32 bucketIdx{ 0 };
			U32 arrayIndex{ 0 };
			Vector<SparseArray<SetElementType, false>>* buffer{nullptr};
		};

		Iterator End()
		{
			return tail;
		}

		Iterator Begin()
		{
			return Iterator{ 0, 0, &buckets };
		}

		Iterator FindByHash(U32 hash)
		{
			U32 bucketIdx = hash & (numBuckets - 1);
			U32 idx = buckets[bucketIdx].FindIndex(hash);
			if (idx != INDEX_NONE) {
				return Iterator{ bucketIdx, idx, &buckets };
			}
			return End();
		}

		Iterator Find(const SetElementType& key)
		{
			U32 hash = MakeHash(key);
			return FindByHash(hash);
		}

		U32 Size() const
		{
			U32 sz = 0;
			for (int i = 0; i < buckets.Size(); ++i)
			{
				sz += buckets[i].Size();
			}
			return sz;
		}

		HashSet() {
			buckets.Resize(numBuckets);
			tail = { numBuckets, 0, &buckets };
		}

		/// 初始化列表
		HashSet(const std::initializer_list<SetElementType>& list) : HashSet()
		{
			for (auto it = list.begin(); it != list.end(); it++)
			{
				Insert(*it);
			}
		}
		~HashSet() = default;

		Iterator Insert(const SetElementType& element) {
			U32 hash = MakeHash(element);
			U32 bucketIdx = hash & (MIN_BUCKETS - 1);
			buckets[bucketIdx].Add(hash, element);
			++numElements;
			return { bucketIdx, buckets[bucketIdx].Size() - 1, &buckets };
		}

		bool Erase(const SetElementType& value)
		{
			Iterator it = Find(value);
			if (it == End())
				return false;
			--numElements;
			return it.Erase();
		}

		bool Empty() const
		{
			return 0 == numElements;
		}
	private:
		Vector<SparseArray<SetElementType, false>> buckets;
		Iterator tail;
		U32 numBuckets{ MIN_BUCKETS };
		U32 numElements;
	};

	template <class T> typename HashSet<T>::ConstIterator begin(const HashSet<T>& v) { return v.Begin(); }

	template <class T> typename HashSet<T>::ConstIterator end(const HashSet<T>& v) { return v.End(); }

	template <class T> typename HashSet<T>::Iterator begin(HashSet<T>& v) { return v.Begin(); }

	template <class T> typename HashSet<T>::Iterator end(HashSet<T>& v) { return v.End(); }
}
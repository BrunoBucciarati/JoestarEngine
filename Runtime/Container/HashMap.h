#pragma once
#include "Pair.h"
#include "HashSet.h"
namespace Joestar {
	template<class T, class U>
	class HashMap {
	public:
		typedef Pair<T, U> DataType;
		typedef typename HashSet<DataType>::Iterator SetIterator;
		HashMap() {
			setTail = set.End();
			tail = { setTail };
		};
		~HashMap() = default;

		struct Iterator {
			Iterator& operator ++() {
				++setIter;
				return *this;
			}
			Iterator& operator --() {
				--setIter;
				return *this;
			}
			bool operator==(const Iterator& rhs) {
				return setIter == rhs.setIter;
			}
			bool operator!=(const Iterator& rhs) {
				return setIter != rhs.setIter;
			}
			DataType* operator->() {
				return setIter.Get();
			}
			SetIterator setIter;
		};
		Iterator End() {
			return tail;
		}
		Iterator Find(const T& key) {
			SetIterator it = set.FindByHash(MakeHash(key));
			if (it != set.End()) {
				return Iterator{ it };
			}
			return End();
		}
		bool Contains(const T& key)
		{
			return Find(key) != End();
		}
		U& operator[](const T& key) {
			Iterator it = Find(key);
			if (it != End()) {
				return it->value;
			}
			it = Insert(key, NULL);
			return it->value;
		}
		const U& operator[](const T& key) const {
			Iterator it = Find(key);
			if (it != End()) {
				return it->value;
			}
			it = Insert(key, NULL);
			return it->value;
		}

		Iterator Insert(T key, U value) {
			Pair<T, U> pair(key, value);
			return Insert(pair);
		}
		Iterator Insert(DataType& pair) {
			return { set.Insert(pair) };
		}
	private:
		HashSet<DataType> set;
		SetIterator setTail;
		Iterator tail;
	};
}
#pragma once
#include "Vector.h"
namespace Joestar {
	template <class T, bool Ordered>
	class SparseArray {
	public:
		SparseArray() = default;
		~SparseArray() = default;
		U32 Add(U32 index, const T & element);

	private:
		Vector<T> data;
		Vector<U32> indices;
		U32 numElements{0};
		U32 maxIndex{0};
	};

	template <class T, bool Ordered>
	U32 SparseArray<T, Ordered>::Add(U32 index, const T& element) {
		if (!Ordered) {
			data.Push(element);
			indices.Push(index);
			numElements = data.Size();
			return 0;
		}

		int st = 0, ed = numElements;
		//二分一个插入点
		while (st < ed) {
			int idx = (st + ed) / 2;
			if (indices[idx] == index) {
				break;
			}
			else if (indices[idx] < index) {
				st = idx + 1;
			}
			else if (indices[idx] > index) {
				ed = idx - 1;
			}
		}
		indices.Resize(numElements + 1);
		data.Resize(numElements + 1);
		for (int i = numElements; i > st; --i) {
			indices[i] = indices[i - 1];
			data[i] = data[i - 1];
		}
		indices[st] = index;
		data[st] = element;
		numElements = data.Size();
		return 0;
	}
}
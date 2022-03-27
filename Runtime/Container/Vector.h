#pragma once
#include "../Platform/Platform.h"
#include "../Core/Functions.h"
#include "../IO/MemoryManager.h"
namespace Joestar
{
    template <class T> struct VectorIterator
    {
        VectorIterator() :
            ptr(nullptr)
        {
        }

        /// Construct with an object pointer.
        explicit VectorIterator(T* ptr1) :
            ptr(ptr1)
        {
        }

        /// Point to the object.
        T* operator ->() const { return ptr; }

        /// Dereference the object.
        T& operator *() const { return *ptr; }

        /// Preincrement the pointer.
        VectorIterator<T>& operator ++()
        {
            ++ptr;
            return *this;
        }

        /// Postincrement the pointer.
        VectorIterator<T> operator ++(int)
        {
            VectorIterator<T> it = *this;
            ++ptr;
            return it;
        }

        /// Predecrement the pointer.
        VectorIterator<T>& operator --()
        {
            --ptr;
            return *this;
        }

        /// Postdecrement the pointer.
        VectorIterator<T> operator --(int)
        {
            VectorIterator<T> it = *this;
            --ptr;
            return it;
        }

        /// Add an offset to the pointer.
        VectorIterator<T>& operator +=(int value)
        {
            ptr += value;
            return *this;
        }

        /// Subtract an offset from the pointer.
        VectorIterator<T>& operator -=(int value)
        {
            ptr -= value;
            return *this;
        }

        /// Add an offset to the pointer.
        VectorIterator<T> operator +(int value) const { return VectorIterator<T>(ptr + value); }

        /// Subtract an offset from the pointer.
        VectorIterator<T> operator -(int value) const { return VectorIterator<T>(ptr - value); }

        /// Calculate offset to another iterator.
        int operator -(const VectorIterator& rhs) const { return (int)(ptr - rhs.ptr); }

        /// Test for equality with another iterator.
        bool operator ==(const VectorIterator& rhs) const { return ptr == rhs.ptr; }

        /// Test for inequality with another iterator.
        bool operator !=(const VectorIterator& rhs) const { return ptr != rhs.ptr; }

        /// Test for less than with another iterator.
        bool operator <(const VectorIterator& rhs) const { return ptr < rhs.ptr; }

        /// Test for greater than with another iterator.
        bool operator >(const VectorIterator& rhs) const { return ptr > rhs.ptr; }

        /// Test for less than or equal with another iterator.
        bool operator <=(const VectorIterator& rhs) const { return ptr <= rhs.ptr; }

        /// Test for greater than or equal with another iterator.
        bool operator >=(const VectorIterator& rhs) const { return ptr >= rhs.ptr; }

        /// Pointer.
        T* ptr;
    };

    /// Random access const iterator.
    template <class T> struct VectorConstIterator
    {
        /// Construct.
        VectorConstIterator() :
            ptr(0)
        {
        }

        /// Construct with an object pointer.
        explicit VectorConstIterator(const T* ptr) :
            ptr(ptr)
        {
        }

        /// Construct from a non-const iterator.
        VectorConstIterator(const VectorIterator<T>& rhs) :     // NOLINT(google-explicit-constructor)
            ptr(rhs.ptr)
        {
        }

        /// Assign from a non-const iterator.
        VectorConstIterator<T>& operator =(const VectorIterator<T>& rhs)
        {
            ptr = rhs.ptr;
            return *this;
        }

        /// Point to the object.
        const T* operator ->() const { return ptr; }

        /// Dereference the object.
        const T& operator *() const { return *ptr; }

        /// Preincrement the pointer.
        VectorConstIterator<T>& operator ++()
        {
            ++ptr;
            return *this;
        }

        /// Postincrement the pointer.
        VectorConstIterator<T> operator ++(int)
        {
            VectorConstIterator<T> it = *this;
            ++ptr;
            return it;
        }

        /// Predecrement the pointer.
        VectorConstIterator<T>& operator --()
        {
            --ptr;
            return *this;
        }

        /// Postdecrement the pointer.
        VectorConstIterator<T> operator --(int)
        {
            VectorConstIterator<T> it = *this;
            --ptr;
            return it;
        }

        /// Add an offset to the pointer.
        VectorConstIterator<T>& operator +=(int value)
        {
            ptr += value;
            return *this;
        }

        /// Subtract an offset from the pointer.
        VectorConstIterator<T>& operator -=(int value)
        {
            ptr -= value;
            return *this;
        }

        /// Add an offset to the pointer.
        VectorConstIterator<T> operator +(int value) const { return VectorConstIterator<T>(ptr + value); }

        /// Subtract an offset from the pointer.
        VectorConstIterator<T> operator -(int value) const { return VectorConstIterator<T>(ptr - value); }

        /// Calculate offset to another iterator.
        int operator -(const VectorConstIterator& rhs) const { return (int)(ptr - rhs.ptr); }

        /// Test for equality with another iterator.
        bool operator ==(const VectorConstIterator& rhs) const { return ptr == rhs.ptr; }

        /// Test for inequality with another iterator.
        bool operator !=(const VectorConstIterator& rhs) const { return ptr != rhs.ptr; }

        /// Test for less than with another iterator.
        bool operator <(const VectorConstIterator& rhs) const { return ptr < rhs.ptr; }

        /// Test for greater than with another iterator.
        bool operator >(const VectorConstIterator& rhs) const { return ptr > rhs.ptr; }

        /// Test for less than or equal with another iterator.
        bool operator <=(const VectorConstIterator& rhs) const { return ptr <= rhs.ptr; }

        /// Test for greater than or equal with another iterator.
        bool operator >=(const VectorConstIterator& rhs) const { return ptr >= rhs.ptr; }

        /// Pointer.
        const T* ptr;
    };

	template<class T>
	class Vector {
	public:
        using Iterator = VectorIterator<T>;
        using ConstIterator = VectorConstIterator<T>;
		Vector() = default;
        
        Vector(U32 sz)
        {
            Resize(sz);
        }
        Vector(const Vector<T>& rhs)
        {
            *this = rhs;
        }
        /// 初始化列表
        Vector(const std::initializer_list<T>&list)
        {
            for (auto it = list.begin(); it != list.end(); it++)
            {
                Push(*it);
            }
        }
        /// 初始化列表
        Vector(U32 sz, const T& value)
        {
            Resize(sz);
            for (int i = 0; i < sz; ++i)
            {
                mBuffer[i] = value;
            }
        }

		~Vector()
        {
            DestructItems(0, mSize);
			JOJO_DELETE_ARRAY((U8*)mBuffer, MEMORY_CONTAINER);
		}

		void Resize(U32 sz)
        {
			U32 oldSz = mSize;
			Reserve(sz);
            if (sz > oldSz)
                ConstructItems(oldSz, sz - oldSz);
			mSize = sz;
		}
		void Reserve(U32 cap) {
			if (!mCapacity && cap) {
				mCapacity = cap;
                U8* buf = JOJO_NEW_ARRAY(U8, mCapacity * sizeof(T), MEMORY_CONTAINER);
				mBuffer = reinterpret_cast<T*>(buf);
				return;
			}
			if (mCapacity && mCapacity < cap) {
				U32 oldCapacity = mCapacity;
				while (mCapacity < cap) {
					mCapacity = mCapacity << 1;
				}
                T* newBuffer = reinterpret_cast<T*>(JOJO_NEW_ARRAY(U8, mCapacity * sizeof(T), MEMORY_CONTAINER));
				if (mBuffer) {
                    for (U32 i = 0; i < oldCapacity; ++i)
                        JOJO_PLACEMENT_NEW(T(std::move(mBuffer[i])), newBuffer + i, MEMORY_CONTAINER);
                    DestructItems(0, oldCapacity);
					JOJO_DELETE_ARRAY((U8*)mBuffer, MEMORY_CONTAINER);
				}
                mBuffer = newBuffer;
			}
		}

		const U32 Size() const
        {
			return mSize;
		}

		T& operator[](U32 index)
        {
			return mBuffer[index];
		}

        const T& operator[](U32 index) const
        {
            return mBuffer[index];
        }

        Vector& operator=(const Vector& rhs)
        {
            Reserve(rhs.mCapacity);
            mCapacity = rhs.mCapacity;
            mSize = rhs.mSize;
            for (U32 i = 0; i < rhs.mSize; ++i)
            {
                JOJO_PLACEMENT_NEW(T(rhs.mBuffer[i]), mBuffer + i, MEMORY_CONTAINER);
            }
            return *this;
        }

		void Push(const T& value)
		{
			if (mSize >= mCapacity) {
				Reserve(mSize + 1);
			}
			JOJO_PLACEMENT_NEW(T(value), mBuffer + mSize, MEMORY_CONTAINER);
			++mSize;
		}


        /// Create an element at the end.
        template <class... Args> T& EmplaceBack(Args&&... args)
        {
            if (mSize < mCapacity)
            {
                // Optimize common case
                ++mSize;
                JOJO_PLACEMENT_NEW(T(std::forward<Args>(args)...), &Back(), MEMORY_CONTAINER);
            }
            else
            {
                T value(std::forward<Args>(args)...);
                Push(std::move(value));
            }
            return Back();
        }

		void Clear()
        {
			mSize = 0;
		}

		T* Buffer() const
        {
			return mBuffer;
		}
		T& Back() {
			return mBuffer[mSize - 1];
		}
		ConstIterator Begin() const {
			return ConstIterator(mBuffer);
		}
        Iterator Begin() {
            return Iterator(mBuffer);
        }
        ConstIterator End() const {
            return ConstIterator(mBuffer + mSize);
        }
        Iterator End() {
            return Iterator(mBuffer + mSize);
        }
        virtual void DestructItems(U32 st, U32 length)
        {
            for (U32 i = 0; i < length; ++i) {
                (mBuffer + st + i)->~T();
            }
        }
        virtual void ConstructItems(U32 st, U32 length)
        {
            for (U32 i = 0; i < length; ++i) {
                JOJO_PLACEMENT_NEW(T, mBuffer + st + i, MEMORY_CONTAINER);
            }
        }
        bool Empty()
        {
            return mSize == 0;
        }

        void Swap(Vector& rhs)
        {
            Joestar::Swap(mCapacity, rhs.mCapacity);
            Joestar::Swap(mSize, rhs.mSize);
            Joestar::Swap(mBuffer, rhs.mBuffer);
        }

        Iterator Find(const T& key)
        {
            for (U32 i = 0; i < mSize; ++i)
            {
                if (key == mBuffer[i])
                {
                    return Iterator(mBuffer + i);
                }
            }
            return End();
        }

	private:
        T* mBuffer{ nullptr };
		U32 mSize{ 0 };
		U32 mCapacity{ 0 };
	};

    template<class T>
    class PODVector : public Vector<T>
    {
    public:
        PODVector() = default;
        /// 初始化列表
        PODVector(const std::initializer_list<T>& list)
        {
            for (auto it = list.begin(); it != list.end(); it++)
            {
                Push(*it);
            }
        }
        void DestructItems(U32 st, U32 length) {}
        void ConstructItems(U32 st, U32 length) {}
    };

    template <class T> typename Vector<T>::ConstIterator begin(const Vector<T>& v) { return v.Begin(); }

    template <class T> typename Vector<T>::ConstIterator end(const Vector<T>& v) { return v.End(); }

    template <class T> typename Vector<T>::Iterator begin(Vector<T>& v) { return v.Begin(); }

    template <class T> typename Vector<T>::Iterator end(Vector<T>& v) { return v.End(); }
}
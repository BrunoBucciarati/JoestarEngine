#pragma once
namespace Joestar
{
	template <class T>
	class SharedPtr
	{
	public:
		SharedPtr() : mPtr(nullptr)
		{}

		SharedPtr(T* ptr) : mPtr(ptr)
		{
			AddRef();
		}

		SharedPtr(const SharedPtr<T>& rhs) : mPtr(rhs.mPtr)
		{
			AddRef();
		}
		~SharedPtr()
		{
			ReleaseRef();
		}
		T* operator ->() const
		{
			return mPtr;
		}
		T* operator *() const
		{
			return *mPtr;
		}
		T* operator [](int index)
		{
			return mPtr[index];
		}
		SharedPtr& operator=(T* ptr)
		{
			mPtr = ptr;
			AddRef();
			return *this;
		}
		void AddRef() {
			if (mPtr)
				mPtr->AddRef();
		}
		void ReleaseRef() {
			if (mPtr)
				mPtr->Release();
		}
		bool Null() {
			return !!mPtr;
		}
		T* Get() const {
			return mPtr;
		}
		operator bool()
		{
			return mPtr != nullptr;
		}
		operator T* ()
		{
			return mPtr;
		}
	private:
		T* mPtr;
	};

	template <class T>
	class WeakPtr
	{
	public:
		WeakPtr() : mPtr(nullptr)
		{}

		WeakPtr(T* ptr) : mPtr(ptr)
		{
			//AddRef();
		}

		WeakPtr(const WeakPtr<T>& rhs) : mPtr(rhs.mPtr)
		{
			//AddRef();
		}
		~WeakPtr()
		{
			ReleaseRef();
		}
		T* operator ->() const
		{
			return mPtr;
		}
		T* operator *() const
		{
			return *mPtr;
		}
		T* operator [](int index)
		{
			return mPtr[index];
		}
		WeakPtr& operator=(T* ptr)
		{
			mPtr = ptr;
			return *this;
		}
		void AddRef()
		{
		}
		void ReleaseRef()
		{
			mPtr = nullptr;
		}
		bool Null()
		{
			return !!mPtr;
		}
		T* Get() const
		{
			return mPtr;
		}
		operator bool()
		{
			return mPtr != nullptr;
		}
		operator T* ()
		{
			return mPtr;
		}
	private:
		T* mPtr;
	};

	template <class T>
	class UniquePtr
	{
	public:
		UniquePtr() : mPtr(nullptr)
		{}
		UniquePtr(T* ptr) : mPtr(ptr)
		{}
		UniquePtr(const UniquePtr<T>& rhs) : mPtr(rhs.mPtr)
		{}
		UniquePtr& operator=(T* ptr)
		{
			mPtr = ptr;
			return *this;
		}

		~UniquePtr()
		{
			if (mPtr)
				delete mPtr;
		}
		T* operator ->() const
		{
			return mPtr;
		}
		T* operator *() const
		{
			return *mPtr;
		}
		T* operator [](int index)
		{
			return mPtr[index];
		}
		operator bool()
		{
			return mPtr != nullptr;
		}
		operator T* ()
		{
			return mPtr;
		}
		bool Null()
		{
			return !!mPtr;
		}
		T* Get() const
		{
			return mPtr;
		}
	private:
		T* mPtr;
	};

}
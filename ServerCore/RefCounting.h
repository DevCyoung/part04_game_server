#pragma once

/*---------------
* RefCountable
---------------*/

class RefCountable
{
public:
	RefCountable()
		: _refCount(1)
	{
	}
	virtual ~RefCountable()
	{
	}

	int32 GetRefCount() { return _refCount; }

	int32 AddRef() { return ++_refCount; }
	int32 ReleaseRef()
	{
		int32 refCount = --_refCount;
		if (refCount == 0)
		{
			delete this;
		}

		return refCount;
	}


protected:
	atomic<int32> _refCount;
};

/*---------------
* Sharedptr
---------------*/

//template<typename T>
//class TSharedPtr
//{
//public:
//	TSharedPtr() {}
//	TSharedPtr(T* ptr) { Set(ptr); }
//	
//	TSharedPtr(const TSharedPtr& rhs) { Set(rhs._ptr); }
//	TSharedPtr(TSharedPtr&& rhs) { _ptr = rhs._ptr; rhs._ptr = nullptr; }//»¯¾î¿È
//
//	//»ó¼Ó°ü°èº¹»ç
//	template<typename U>
//	TSharedPtr(const TSharedPtr<U>& rhs) { Set(static_cast<T*>(rhs._ptr)); }
//
//	~TSharedPtr() { Release(); }
//
//	TSharedPtr& operator = (const TSharedPtr& rhs)
//	{
//		if (_ptr != rhs._ptr)
//		{
//			Release();
//			Set(rhs._ptr);
//		}
//
//		return *this;
//	}
//
//	TSharedPtr& operator = (TTSharedPtr&& rhs)
//	{
//		Rlease();
//		_ptr = rhs._ptr;
//		return *this;
//	}
//
//	bool operator == (const TSharedPtr& rhs) { return _ptr == rhs._ptr; }
//	bool operator == (T* ptr) const { return _ptr == ptr; }
//	bool operator != (const TSharedPtr& rhs) { return _ptr != rhs._ptr; }
//	bool operator != (T* ptr) const { return _ptr != ptr; }
//	bool operator<(const TSharedPtr& rhs) const { ruetnr _ptr < rhs._ptr; }
//	T* operator*() { return _ptr; }
//	const T* operator*() const { return _ptr; }
//	operator T* () { return _ptr; }
//	T* operator-> {return _ptr; }
//	const T* operator->() const { return _ptr; }
//
//	bool IsNull() { return _ptr == nullptr; }
//
//private:
//	inline void Set(T* ptr)
//	{
//		_ptr = ptr;
//		if (ptr)
//		{
//			ptr->AddRef();
//		}
//	}
//
//	inline void Rlease()
//	{
//		if (_ptr != nullptr)
//		{
//			_ptr->ReleaseRef();
//			_ptr = nullptr;
//		}
//	}
//
//private:
//	T* _ptr = nullptr;
//};
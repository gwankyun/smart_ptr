#pragma once
#include <cstdlib> // NULL
#include <iostream>
#include <cassert> // assert
#include <cstddef> // std::nullptr_t
#include <algorithm>
#include <utility>
#include <compile_features.h>

#ifndef NOEXCEPT
#ifdef CXX_NOEXCEPT
#define NOEXCEPT noexcept
#else
#define NOEXCEPT
#endif // CXX_NOEXCEPT
#endif // !NOEXCEPT

#ifndef CONSTEXPR
#ifdef __cpp_constexpr
#define CONSTEXPR constexpr
#else
#define CONSTEXPR
#endif // __cpp_constexpr
#endif // !CONSTEXPR

#ifndef OVERRIDE
#ifdef CXX_OVERRIDE
#define OVERRIDE override
#else
#define OVERRIDE
#endif // CXX_OVERRIDE
#endif // !OVERRIDE

template<typename T>
void safe_delete(T*& ptr)
{
	if (ptr != NULL)
	{
		delete ptr;
		ptr = NULL;
	}
}

template<typename T>
class IDeleter
{
public:
	IDeleter()
	{
	}

	virtual ~IDeleter()
	{
	}

	virtual void operator()(T* ptr) = 0;

private:

};

template<typename T, typename D>
class DeleterHelp : public IDeleter<T>
{
public:
	DeleterHelp(D deleter) : _deleter(deleter)
	{
	}

	~DeleterHelp()
	{
	}

	void operator()(T* ptr) OVERRIDE
	{
		_deleter(ptr);
	}

private:
	D _deleter;
};



template<typename T>
class ControlBlock
{
public:
	ControlBlock() : object_ptr(NULL), shared_count(0), weak_count(0), object(T()), deleter(NULL), max_id(0)
	{
	}

	~ControlBlock()
	{
		if (deleter != NULL) // 有刪除器就用刪除器
		{
			(*deleter)(object_ptr);
            safe_delete(deleter);
		}
		else
		{
            safe_delete(object_ptr);
		}
		printf("[%s] shared: %d\n", __func__, shared_count);
	}

	T object;
	T* object_ptr;
	int shared_count;
	int weak_count;
	// deleter

	IDeleter<T>* deleter; // 刪除器
	int max_id;

	// allocator

private:

};

template<typename T>
inline void shared_dec(ControlBlock<T>*& controlBlock)
{
    printf("[%s]\n", __func__);
    if (controlBlock == NULL)
    {
        return;
    }
	assert(controlBlock->shared_count > 0);
    controlBlock->shared_count--;
    if (controlBlock->shared_count == 0)
    {
        if (controlBlock->object_ptr != NULL)
        {
            safe_delete(controlBlock->object_ptr);
        }
        if (controlBlock->weak_count == 0)
        {
            safe_delete(controlBlock);
        }
    }
}

template<typename T>
inline void shared_inc(ControlBlock<T>*& controlBlock)
{
	if (controlBlock == NULL)
	{
		return;
	}
	printf("[%s]\n", __func__);
	controlBlock->shared_count++;
	controlBlock->max_id++;
}

template<typename T>
inline void weak_inc(ControlBlock<T>*& controlBlock)
{
	if (controlBlock == NULL)
	{
		return;
	}
	controlBlock->weak_count++;
	if (controlBlock->weak_count == 0 && controlBlock->shared_count == 0)
	{
		safe_delete(controlBlock);
	}
}

template<typename T>
inline void weak_dec(ControlBlock<T>*& controlBlock)
{
	if (controlBlock == NULL)
	{
		return;
	}
	controlBlock->weak_count--;
}

template<typename T> class weak_ptr;
template<typename T>
class shared_ptr
{
public:

	typedef T element_type;

	CONSTEXPR shared_ptr() NOEXCEPT : _ptr(NULL), _controlBlock(NULL)
	{
	}

#ifdef CXX_NULLPTR
	CONSTEXPR shared_ptr(std::nullptr_t ptr) NOEXCEPT : _ptr(ptr), _controlBlock(NULL)
	{
	}
#endif // CXX_NULLPTR

	template<typename Y>
	explicit shared_ptr(Y* ptr) NOEXCEPT : _ptr(ptr), _controlBlock(new ControlBlock<T>())
	{
		_controlBlock->object_ptr = _ptr;
		_id = _controlBlock->max_id;
		shared_inc(_controlBlock);
	}

	template<typename Y, typename Deleter>
	shared_ptr(Y* ptr, Deleter d) NOEXCEPT : _ptr(ptr), _controlBlock(new ControlBlock<T>())
	{
		_controlBlock->object_ptr = _ptr;
		_controlBlock->deleter = new DeleterHelp<T, Deleter>(d);
		_id = _controlBlock->max_id;
		shared_inc(_controlBlock);
	}

#ifdef CXX_NULLPTR
	template<typename Deleter>
	shared_ptr(std::nullptr_t* ptr, Deleter d) NOEXCEPT : _ptr(ptr), _controlBlock(new ControlBlock<T>())
	{
		_controlBlock->object_ptr = _ptr;
		_controlBlock->deleter = new DeleterHelp<T, Deleter>(d);
		_id = _controlBlock->max_id;
		shared_inc(_controlBlock);
	}
#endif // CXX_NULLPTR

	shared_ptr(T* ptr) : _ptr(ptr), _controlBlock(new _controlBlock<T>())
	{
		_controlBlock->object_ptr = _ptr;
		_id = _controlBlock->max_id;
		shared_inc(_controlBlock);
	}

	shared_ptr(shared_ptr& other) : _ptr(other._ptr), _controlBlock(other._controlBlock)
	{
		printf("[copy %s] shared: %d\n", __func__, _controlBlock->shared_count);
		_id = _controlBlock->max_id;
		shared_inc(_controlBlock);
	}

	template<typename Y>
	shared_ptr(const weak_ptr<Y>& r) : _ptr(r._ptr), _controlBlock(r._controlBlock)
	{
		_id = _controlBlock->max_id;
		shared_inc(_controlBlock);
    }

	~shared_ptr()
	{
		printf("[%s] shared: %d\n", __func__, _controlBlock->shared_count);
		shared_dec(_controlBlock);
        _ptr = NULL;
		_controlBlock = NULL;
	}

	shared_ptr& operator=(const shared_ptr& other) NOEXCEPT
	{
		if (this == &other)
		{
			return *this;
		}

		shared_dec(_controlBlock);

		_ptr = other->_ptr;
		_controlBlock = other->_controlBlock;
		_id = _controlBlock->max_id;
		shared_inc(_controlBlock);

		return *this;
	}

	// 修改器

	void reset() NOEXCEPT
	{
		shared_dec(_controlBlock);
		_ptr = NULL;
	}

    template<typename Y>
	void reset(Y* ptr)
	{
		if (_controlBlock == NULL)
		{
			return;
		}
		_controlBlock->object_ptr = ptr;
	}

	void swap(shared_ptr& r) NOEXCEPT
	{
		std::swap(_ptr, r->_ptr);
		std::swap(_controlBlock, r->_controlBlock);
	}

	// 觀察器

	T* get() const NOEXCEPT
	{
		return _ptr;
	}

	T& operator*() const NOEXCEPT
	{
		if (_ptr == NULL)
		{
			return (T*)0;
		}
		return *_ptr;
	}

	T* operator->() const NOEXCEPT
	{
		return _ptr;
	}

	long use_count() const NOEXCEPT
	{
		if (_controlBlock == NULL)
		{
			return 0;
		}
		return _controlBlock->shared_count;
	}

	bool unique() const NOEXCEPT
	{
		return use_count() == 1;
	}

	explicit operator bool() const NOEXCEPT
	{
		return get() != NULL;
	}

	template<typename Y>
	bool owner_before(const shared_ptr<Y>& other) const NOEXCEPT
	{
		if (_controlBlock == NULL && other._controlBlock == NULL)
		{
			return true;
		}
		else if (_controlBlock == other._controlBlock)
		{
			return _id < other._id;
		}
		else
		{
			assert(false);
		}
	}

    template<typename T>
	friend shared_ptr<T> make_shared();

private:
	T* _ptr;
	ControlBlock<T>* _controlBlock;
	int _id;
};

template<typename T>
class weak_ptr
{
public:

	typedef T element_type;

	CONSTEXPR weak_ptr() NOEXCEPT : _ptr(NULL), _controlBlock(NULL)
	{
	}

	weak_ptr(const weak_ptr& r) NOEXCEPT : _ptr(r._ptr), _controlBlock(r._controlBlock)
	{
		_id = _controlBlock->max_id;
		weak_inc(_controlBlock);
	}

	template<typename Y>
	weak_ptr(const weak_ptr<Y>& r) NOEXCEPT : _ptr(r._ptr), _controlBlock(r._controlBlock)
	{
		_id = _controlBlock->max_id;
		weak_inc(_controlBlock);
	}

	template<typename Y>
	weak_ptr(const shared_ptr<Y>& r) NOEXCEPT : _ptr(r._ptr), _controlBlock(r._controlBlock)
	{
		_id = _controlBlock->max_id;
		weak_inc(_controlBlock);
	}

#ifdef __cpp_rvalue_references
	weak_ptr(weak_ptr&& r) NOEXCEPT : _ptr(r._ptr), _controlBlock(r._controlBlock)
	{
		r._ptr = NULL;
		r._controlBlock = NULL;
	}

	template<typename Y>
	weak_ptr(weak_ptr<Y>&& r) NOEXCEPT : _ptr(r->_ptr), _controlBlock(r._controlBlock)
	{
		r._ptr = NULL;
		r._controlBlock = NULL;
	}
#endif // __cpp_rvalue_references

	~weak_ptr()
	{
		weak_dec(_controlBlock);
		_ptr = NULL;
		_controlBlock = NULL;
	}

	weak_ptr& operator=(const weak_ptr& r) NOEXCEPT
	{
		_ptr = r._ptr;
		_controlBlock = r._controlBlock;
		_id = _controlBlock->max_id;
		weak_inc(_controlBlock);
	}

	template<typename Y>
	weak_ptr& operator=(const weak_ptr<Y>& r) NOEXCEPT
	{
		_ptr = r._ptr;
		_controlBlock = r._controlBlock;
		_id = _controlBlock->max_id;
		weak_inc(_controlBlock);
	}

	template<typename Y>
	weak_ptr& operator=(const shared_ptr<Y>& r) NOEXCEPT
	{
		_ptr = r._ptr;
		_controlBlock = r._controlBlock;
		_id = _controlBlock->max_id;
		weak_inc(_controlBlock);
	}

	weak_ptr& operator=(weak_ptr&& r) NOEXCEPT
	{
		reset();
		swap(r);
	}

	template<typename Y>
	weak_ptr& operator=(weak_ptr<Y>&& r) NOEXCEPT
	{
		reset();
		swap(r);
	}

	void reset() NOEXCEPT
	{
		_ptr = NULL;
		weak_dec(_controlBlock);
		_controlBlock = NULL;
	}

	void swap(weak_ptr& r) NOEXCEPT
	{
		std::swap(_ptr, r._ptr);
		std::swap(_controlBlock, r._controlBlock);
	}

	long use_count() const NOEXCEPT
	{
		if (_controlBlock == NULL)
		{
			return 0;
		}
		return _controlBlock->shared_count;
	}

	bool expired() const NOEXCEPT
	{
		return use_count() == 0;
	}

	shared_ptr<T> lock() const NOEXCEPT
	{
		if (expired())
		{
			return shared_ptr<T>();
		}
		else
		{
			return shared_ptr<T>(*this);
		}
	}

private:
	T* _ptr;
	ControlBlock<T>* _controlBlock;
	int _id;
};

template<typename T>
shared_ptr<T> make_shared()
{
	ControlBlock<T>* controlBlock = new ControlBlock<T>();

	shared_ptr<T> ptr;
	ptr._id = controlBlock->max_id;
	ptr._ptr = &(controlBlock->object);
	ptr._controlBlock = controlBlock;

	shared_inc(controlBlock);
	return ptr;
}

#pragma once
#include <cstdlib> // NULL
#include <iostream>
#include <cassert> // assert
#include <cstddef> // std::nullptr_t
#include <algorithm> // std::swap
#include <utility> // std::swap
#include "ControlBlock.hpp"

template<typename T> class weak_ptr;
template<typename T>
class shared_ptr
{
public:

    typedef T element_type;

    typedef weak_ptr<T> weak_type;

    //constexpr shared_ptr() noexcept;
    CONSTEXPR shared_ptr() NOEXCEPT : _ptr(NULL), _controlBlock(NULL), _id(0)
    {
        LOG("NULL");
    }

    //constexpr shared_ptr(std::nullptr_t) noexcept;
#ifdef CXX_NULLPTR
    CONSTEXPR shared_ptr(std::nullptr_t ptr) NOEXCEPT : _ptr(ptr), _controlBlock(NULL), _id(0)
    {
        LOG();
    }
#endif // CXX_NULLPTR

    //template< class Y >
    //explicit shared_ptr(Y* ptr);
    template<typename Y>
    explicit shared_ptr(Y* ptr) NOEXCEPT : _ptr(ptr), _controlBlock(new ControlBlock<T>()), _id(0)
    {
        LOG();
        LOG("shared_ptr copy");
        _controlBlock->object_ptr = _ptr;
        //_id = _controlBlock->max_id;
        CHECK_SHARED_INC();
        shared_inc(_controlBlock);
    }

    //template< class Y, class Deleter >
    //shared_ptr(Y* ptr, Deleter d);
    template<typename Y, typename Deleter>
    shared_ptr(Y* ptr, Deleter d) NOEXCEPT : _ptr(ptr), _controlBlock(new ControlBlock<T>()), _id(0)
    {
        LOG();
        _controlBlock->object_ptr = _ptr;
        _controlBlock->deleter = new DeleterHelp<T, Deleter>(d);
        //_id = _controlBlock->max_id;
        CHECK_SHARED_INC();
        shared_inc(_controlBlock);
    }

    //template< class Deleter >
    //shared_ptr(std::nullptr_t ptr, Deleter d);
#ifdef CXX_NULLPTR
    template<typename Deleter>
    shared_ptr(std::nullptr_t* ptr, Deleter d) NOEXCEPT : _ptr(ptr), _controlBlock(new ControlBlock<T>()), _id(0)
    {
        LOG();
        _controlBlock->object_ptr = _ptr;
        _controlBlock->deleter = new DeleterHelp<T, Deleter>(d);
        //_id = _controlBlock->max_id;
        CHECK_SHARED_INC();
        shared_inc(_controlBlock);
    }
#endif // CXX_NULLPTR

    //template< class Y, class Deleter, class Alloc >
    //shared_ptr(Y* ptr, Deleter d, Alloc alloc);

    //template< class Deleter, class Alloc >
    //shared_ptr(std::nullptr_t ptr, Deleter d, Alloc alloc);

    //template< class Y >
    //shared_ptr(const shared_ptr<Y>& r, element_type* ptr) noexcept;
    template<typename Y>
    shared_ptr(const shared_ptr<Y>& r, element_type* ptr) NOEXCEPT
    {
        LOG();
        // 未做
        assert(false);
    }

    //template< class Y >
    //shared_ptr(shared_ptr<Y>&& r, element_type* ptr) noexcept;
    template<typename Y>
    shared_ptr(shared_ptr<Y>&& r, element_type* ptr) NOEXCEPT
    {
        LOG();
        // 未做
        assert(false);
    }

    //shared_ptr(const shared_ptr& r) noexcept;
    shared_ptr(const shared_ptr& r) NOEXCEPT
        : _ptr(r._ptr), _controlBlock(r._controlBlock), _id(r._controlBlock->max_id)
    {
        LOG();
        //_id = _controlBlock->max_id;
        //source_location sl(SOURCE_LOCATION_CURRENT());
        //printf("[%s:%d] shared: %d weak: %d\n", sl.function_name(), , _controlBlock->shared_count, _controlBlock->weak_count);
        CHECK_SHARED_INC();
        shared_inc(_controlBlock);
    }

    //template< class Y >
    //shared_ptr(const shared_ptr<Y>& r) noexcept;
    template<typename Y>
    shared_ptr(const shared_ptr<Y>& r) NOEXCEPT
        : _ptr(r._ptr), _controlBlock(r._controlBlock), _id(r._controlBlock->max_id)
    {
        LOG();
        //_id = _controlBlock->max_id;
        CHECK_SHARED_INC();
        shared_inc(_controlBlock);
    }

    //shared_ptr(shared_ptr&& r) noexcept;
    shared_ptr(shared_ptr&& r) NOEXCEPT
        : _ptr(r._ptr), _controlBlock(r._controlBlock), _id(r._controlBlock->max_id)
    {
        LOG("&&");
        r._controlBlock = NULL;
    }

    //template< class Y >
    //shared_ptr(shared_ptr<Y>&& r) noexcept;
    template<typename Y>
    shared_ptr(shared_ptr<Y>&& r) NOEXCEPT
        : _ptr(r._ptr), _controlBlock(r._controlBlock), _id(r._controlBlock->max_id)
    {
        LOG("&&");
        r._controlBlock = NULL;
    }

    //template< class Y >
    //explicit shared_ptr(const std::weak_ptr<Y>& r);
    template<typename Y>
    shared_ptr(const weak_ptr<Y>& r)
        : _ptr(r._ptr), _controlBlock(r._controlBlock), _id(r._controlBlock->max_id)
    {
        LOG();
        //_id = _controlBlock->max_id;
        CHECK_SHARED_INC();
        shared_inc(_controlBlock);
    }

    //template< class Y >
    //shared_ptr(std::auto_ptr<Y>&& r);

    //template< class Y, class Deleter >
    //shared_ptr(std::unique_ptr<Y, Deleter>&& r);

    ~shared_ptr()
    {
        //printf("[%s] shared: %d weak: %d\n", __func__, _controlBlock->shared_count, _controlBlock->weak_count);
        show(__func__, __LINE__);
        CHECK_SHARED_DEC();
        shared_dec(_controlBlock);
        _ptr = NULL;
        _controlBlock = NULL;
    }

    shared_ptr& operator=(const shared_ptr& other) NOEXCEPT
    {
        LOG();
        if (this == &other)
        {
            return *this;
        }

        CHECK_SHARED_DEC();
        shared_dec(_controlBlock);

        _ptr = other->_ptr;
        _controlBlock = other->_controlBlock;
        _id = _controlBlock->max_id;
        CHECK_SHARED_INC();
        shared_inc(_controlBlock);

        return *this;
    }

    // 修改器

    void reset() NOEXCEPT
    {
        LOG();
        CHECK_SHARED_DEC();
        shared_dec(_controlBlock);
        _ptr = NULL;
        show();
    }

    template<typename Y>
    void reset(Y* ptr)
    {
        LOG();
        if (_controlBlock == NULL)
        {
            return;
        }
        _controlBlock->object_ptr = ptr;
    }

    void swap(shared_ptr& r) NOEXCEPT
    {
        LOG();
        std::swap(_ptr, r->_ptr);
        std::swap(_controlBlock, r->_controlBlock);
    }

    // 觀察器

    T* get() const NOEXCEPT
    {
        LOG();
        return _ptr;
    }

    T& operator*() const NOEXCEPT
    {
        LOG();
        if (_ptr == NULL)
        {
            return (T*)0;
        }
        return *_ptr;
    }

    T* operator->() const NOEXCEPT
    {
        LOG();
        return _ptr;
    }

    long use_count() const NOEXCEPT
    {
        LOG();
        if (_controlBlock == NULL)
        {
            return 0;
        }
        return _controlBlock->shared_count;
    }

    bool unique() const NOEXCEPT
    {
        LOG();
        return use_count() == 1;
    }

    explicit operator bool() const NOEXCEPT
    {
        LOG();
        return get() != NULL;
    }

    template<typename Y>
    bool owner_before(const shared_ptr<Y>& other) const NOEXCEPT
    {
        LOG();
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

    inline void show()
    {
        if (_controlBlock == NULL)
        {
            LOG("null");
            return;
        }
        LOG("shard: %d weak: %d", _controlBlock->shared_count, _controlBlock->weak_count);
    }

    inline void show(int line)
    {
        if (_controlBlock == NULL)
        {
            return;
        }
        printf("[%s:%d] shard: %d weak: %d\n", __func__, line, _controlBlock->shared_count, _controlBlock->weak_count);
    }

    inline void show(const std::string& func, int line)
    {
        if (_controlBlock == NULL)
        {
            return;
        }
        printf("[%s:%d] shard: %d weak: %d\n", func.c_str(), line, _controlBlock->shared_count, _controlBlock->weak_count);
    }

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
    LOG("[enter make_shared]");
    struct OnExit
    {
        OnExit() = default;
        ~OnExit()
        {
            LOG("[exit make_shared] shared: %d weak: %d", controlBlock->shared_count, controlBlock->weak_count);
        }
        ControlBlock<T>* controlBlock;
    } onExit;

    ControlBlock<T>* controlBlock = new ControlBlock<T>();
    onExit.controlBlock = controlBlock;

    shared_ptr<T> ptr;
    ptr._id = controlBlock->max_id;
    ptr._ptr = &(controlBlock->object);
    ptr._controlBlock = controlBlock;

    CHECK_SHARED_INC();
    shared_inc(controlBlock);
    ptr.show(__LINE__);
    return ptr;
}

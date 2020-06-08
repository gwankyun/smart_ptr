#pragma once
#include "compile_features.h"
#include "log.hpp"

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

//#define CHECK_SHARED_INC() printf("CHECK_SHARED_INC: [%s: %d]\n", __func__, __LINE__)
//#define CHECK_SHARED_DEC() printf("CHECK_SHARED_DEC: [%s: %d]\n", __func__, __LINE__)

#define CHECK_SHARED_INC()
#define CHECK_SHARED_DEC()

template<typename T>
inline void safe_delete(T*& ptr)
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
        //assert(false);
        LOG("shared: %d weak: %d", shared_count, weak_count);
    }

    void show()
    {
        LOG("shared: %d weak: %d", shared_count, weak_count);
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
    if (controlBlock == NULL)
    {
        return;
    }
    LOG("shared: %d weak: %d", controlBlock->shared_count, controlBlock->weak_count);
    assert(controlBlock->shared_count > 0 && controlBlock->weak_count >= 0);
    controlBlock->shared_count--;
    if (controlBlock->shared_count == 0)
    {
        //assert(false);
        if (controlBlock->object_ptr != NULL)
        {
            safe_delete(controlBlock->object_ptr);
            //printf("[%s:%d] delete object_ptr\n", __func__, __LINE__);
            LOG("delete object_ptr");
        }
        if (controlBlock->weak_count == 0)
        {
            safe_delete(controlBlock);
            //printf("[%s:%d] delete controlBlock\n", __func__, __LINE__);
            LOG("delete controlBlock");
            assert(controlBlock == NULL);
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
    printf("[%s] shared: %d weak: %d\n", __func__, controlBlock->shared_count, controlBlock->weak_count);
    assert(controlBlock->shared_count >= 0 && controlBlock->weak_count >= 0);
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

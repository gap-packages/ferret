#ifndef FREE_ANY_DCD
#define FREE_ANY_DCD

namespace detail
{
    template<typename T>
    void freeMemFunction(void* ptr)
    { delete (T*)ptr; }

    typedef void(*free_mem_function)(void*);
}

struct FreeObj
{
    detail::free_mem_function fmf;
    void* ptr;

    void operator()() { fmf(ptr); }
};

template<typename T>
FreeObj makeFreeObj(T* t)
{
    FreeObj fo = {detail::freeMemFunction<T>, t};
    return fo;
}

#endif

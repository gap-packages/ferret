#ifndef MEM_BACKTRACK_CDJOICDSJ
#define MEM_BACKTRACK_CDJOICDSJ
#include "library/vec1.hpp"
#include "library/free_any_object.hpp"
#include "library/library.hpp"
#include <utility>
#include <set>

class MemoryBacktracker;

class BacktrackableType
{
    MemoryBacktracker* mb;
public:
    BacktrackableType(MemoryBacktracker* _mb);
    virtual ~BacktrackableType();

    virtual void event_pushWorld(){}
    virtual void event_popWorld() {}
};


template<typename T>
class Reverting
{
    MemoryBacktracker* mb;
    T* val;
public:
    Reverting(MemoryBacktracker* _mb, T* _t)
    : mb(_mb), val(_t)
    { }

    Reverting()
    : mb(NULL), val(NULL)
    { }

    Reverting(const Reverting& bt):
    mb(bt.mb), val(bt.val)
    { }

    void operator=(const Reverting& bt) {
        mb = bt.mb;
        val = bt.val;
    }

    T get() const
    { return *val; }

    void set(const T& val);
    void set(T&& val);
};

template<typename T>
class RevertingStack
{
    MemoryBacktracker* mb;
    vec1<T>* stack;
public:
    RevertingStack(MemoryBacktracker* _mb, vec1<T>* _t)
    : mb(_mb), stack(_t)
    { }

    RevertingStack()
    : mb(NULL), stack(NULL)
    { }

    RevertingStack(const RevertingStack& bt):
    mb(bt.mb), stack(bt.stack)
    { }

    void operator=(const RevertingStack& bt) {
        mb = bt.mb;
        stack = bt.stack;
    }

    void push_back(const T& t);
    void push_back(T&& t);

    const T& back()
    { return stack->back(); }

    const vec1<T>& get() const
    { return *stack; }

    int size() const
    { return stack->size(); }

    // Why put 'dangerous' in this method name?
    // Because only the size is backtracked, not the contents!
    vec1<T>& getMutable_dangerous()
    { return *stack; }

    // This can only be called before the first 'push'.
    // we will notice that, on the later backtrack.
    void clearStack_dangerous()
    { stack->clear(); }
};

template<typename T>
void resizeBacktrackStack(void* ptr, int val)
{
    T* stack_ptr = (T*)ptr;
    D_ASSERT(stack_ptr->size() >= val);
    stack_ptr->resize(val);
}

typedef void(*backtrack_function)(void*, int);

struct BacktrackObj
{
    backtrack_function fun;
    void* ptr;
    int data;

    void operator()()
    { fun(ptr, data); }
};




class MemoryBacktracker
{
    vec1<vec1<std::pair<int*, int> > > reversions;
    vec1<vec1<BacktrackObj> > function_reversions;

    // This just stores all the objects we allocated,
    // so we can clean up at the end.
    vec1<void*> raw_mem_store;
    vec1<FreeObj> stack_mem_store;

    std::set<BacktrackableType*> objects_to_notify;
public:
    void registerBacktrackableObject(BacktrackableType* bt)
    { objects_to_notify.insert(bt); }

    void unregisterBacktrackableObject(BacktrackableType* bt)
    { objects_to_notify.erase(bt); }
private:

// forbid copying
    MemoryBacktracker(const MemoryBacktracker&);

public:
    MemoryBacktracker() : reversions(1), function_reversions(1)
    { }

    ~MemoryBacktracker()
    {
        for(int i : range1(raw_mem_store.size()))
            free(raw_mem_store[i]);
        for(int i : range1(stack_mem_store.size()))
            stack_mem_store[i]();
    }

    void storeCurrentValue(int* ptr)
    {
        reversions.back().push_back(std::make_pair(ptr, *ptr));
    }

    template<typename Vec>
    void storeCurrentSize(Vec* ptr)
    {
        BacktrackObj obj;
        obj.fun = resizeBacktrackStack<Vec>;
        obj.ptr = ptr;
        obj.data = ptr->size();
        function_reversions.back().push_back(obj);
    }

    void pushWorld()
    {
        debug_out(1, "MemoryManager", "pushWorld");
        for(auto it : objects_to_notify)
            it->event_pushWorld();

        reversions.resize(reversions.size() + 1);
        function_reversions.resize(function_reversions.size() + 1);
    }

    void popWorld()
    {
        debug_out(1, "MemoryManager", "popWorld");

        // Need to go through last state, in reverse order.
        vec1<std::pair<int*, int> >& backref = reversions.back();
        for(int i = backref.size(); i >= 1; --i)
        {
            *(backref[i].first) = backref[i].second;
        }
        reversions.pop_back();

        vec1<BacktrackObj>& stackbackref = function_reversions.back();
        for(int i = stackbackref.size(); i >= 1; --i)
        {
            (stackbackref[i])();
        }
        function_reversions.pop_back();

        for(auto it = objects_to_notify.rbegin();
            it != objects_to_notify.rend(); ++it)
            (*it)->event_popWorld();
    }

    int getDepth()
    { return reversions.size(); }

    void popWorldToDepth(int i)
    {
        D_ASSERT(reversions.size() >= i);
        while(reversions.size() > i)
            popWorld();
    }

    template<typename T>
    Reverting<T> makeReverting()
    {
        void* ptr = calloc(1, sizeof(T));
        raw_mem_store.push_back(ptr);
        return Reverting<T>(this, (T*)ptr);
    }

    template<typename T>
    Reverting<T> makeReverting(const T& t)
    {
        Reverting<T> r = makeReverting<T>();
        r.set(t);
        return r;
    }

    template<typename T>
    RevertingStack<T> makeRevertingStack()
    {
        vec1<T>* ptr = new vec1<T>();
        stack_mem_store.push_back(makeFreeObj(ptr));
        return RevertingStack<T>(this, ptr);
    }
};

template<typename T>
void Reverting<T>::set(const T& t)
{
    mb->storeCurrentValue(val);
    *val = t;
}

template<typename T>
void Reverting<T>::set(T&& t)
{
    mb->storeCurrentValue(val);
    *val = std::move(t);
}

template<typename T>
void RevertingStack<T>::push_back(const T& t)
{
    mb->storeCurrentSize(stack);
    stack->push_back(t);
}

template<typename T>
void RevertingStack<T>::push_back(T&& t)
{
    mb->storeCurrentSize(stack);
    stack->push_back(std::move(t));
}

BacktrackableType::BacktrackableType(MemoryBacktracker* _mb)
: mb(_mb)
{ if(mb != NULL) mb->registerBacktrackableObject(this); }

BacktrackableType::~BacktrackableType()
{ if(mb != NULL) mb->unregisterBacktrackableObject(this); }

#endif

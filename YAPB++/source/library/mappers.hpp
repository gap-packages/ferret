#ifndef _MAPPERS_HPP
#define _MAPPERS_HPP

#include <set>
#include "vec1.hpp"
#include "perm.hpp"

// Some useful class for filtering

struct InSet
{
    const std::set<int>* vals;
    InSet(const std::set<int>* _vals) : vals(_vals)
    { }

    typedef bool result_type;

    template<typename T>
    bool operator()(const T& t)
    { return (*vals).count(t) > 0; }
};


template<typename Container>
struct ContainerToFunctionImpl
{
    typedef typename Container::value_type result_type;
    const Container* vec;
    ContainerToFunctionImpl(Container const* _vec) : vec(_vec)
    { }

    const result_type& operator()(int i)
    { return (*vec)[i]; }

    friend std::ostream& operator<<(std::ostream& o, const  ContainerToFunctionImpl& c)
    { return o << *(c.vec); }
};

template<typename T>
ContainerToFunctionImpl<T> ContainerToFunction(T const* t)
{ return ContainerToFunctionImpl<T>(t); }

// This exists just because for map we want 'result_type', not 'value_type'
template<typename Container>
struct MapToFunctionImpl
{
    typedef typename Container::mapped_type result_type;
    const Container* m;
    MapToFunctionImpl(Container const* _m) : m(_m)
    { }

    const result_type& operator()(int i)
    { 
        typename Container::const_iterator it = m->find(i); 
        D_ASSERT(it != m->end());
        return it->second;
    }
};

template<typename T>
MapToFunctionImpl<T> MapToFunction(T const* t)
{ return MapToFunctionImpl<T>(t); }

template<typename F>
struct FunctionByPermImpl
{
    F f;
    Permutation p;

    typedef typename F::result_type result_type;

    FunctionByPermImpl(const F& _f, const Permutation& _p)
    : f(_f), p(_p)
    { }

    result_type operator()(int i)
    { return f(p[i]); }

    friend std::ostream& operator<<(std::ostream& o, const FunctionByPermImpl& c)
    { return o << c.f << " by " << c.p; }
};

template<typename F>
FunctionByPermImpl<F> FunctionByPerm(F f, const Permutation& p)
{ return FunctionByPermImpl<F>(f, p); }

template<typename F1, typename F2>
struct IndirectFunctionImpl
{
    typedef typename F1::result_type result_type;
    F1 f1;
    F2 f2;

    IndirectFunctionImpl(const F1& _f1, const F2& _f2)
    : f1(_f1), f2(_f2)
    { }

    const result_type& operator()(int i)
    { return f1(f2(i)); }
};


template<typename F1, typename F2>
IndirectFunctionImpl<F1, F2> IndirectFunction(const F1& t, const F2& p)
{ return IndirectFunctionImpl<F1, F2>(t, p); }

template<typename F1, typename F2>
struct IndirectVecCollapseFunctionImpl
{
    typedef typename F2::result_type inner_type;
    typedef typename F1::result_type result_type;
    F1 f1;
    F2 f2;

    IndirectVecCollapseFunctionImpl(const F1& _f1, const F2& _f2)
    : f1(_f1), f2(_f2)
    { }

    result_type operator()(int i)
    {
        typedef typename inner_type::const_iterator it_type;
        const inner_type& c = f2(i);
        result_type r = 0;
        for(it_type it = c.begin(); it != c.end(); ++it)
            r += f1(*it);
        return r;
    }
};


template<typename F1, typename F2>
IndirectVecCollapseFunctionImpl<F1, F2> IndirectVecCollapseFunction(const F1& t, const F2& p)
{ return IndirectVecCollapseFunctionImpl<F1, F2>(t, p); }
#endif

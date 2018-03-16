#ifndef _MAPPERS_HPP
#define _MAPPERS_HPP

#include <set>
#include "library/vec1.hpp"
#include "perm.hpp"

// Some useful classes for filtering

// A function which returns if a value is in a set
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

// Transform a class with an operator[] to one with an operator()
template<typename T>
auto SquareBrackToFunction(T const* t)
{ return [t](int i){ return (*t)[i]; }; }

// This exists just because for map we want 'result_type', not 'value_type'
template<typename Container>
struct MapToFunctionImpl
{
    typedef typename Container::mapped_type result_type;
    const Container* m;
    MapToFunctionImpl(Container const* _m) : m(_m)
    { }

    const auto& operator()(int i)
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

    FunctionByPermImpl(const F& _f, const Permutation& _p)
    : f(_f), p(_p)
    { }

    auto operator()(int i)
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
    F1 f1;
    F2 f2;

    IndirectFunctionImpl(const F1& _f1, const F2& _f2)
    : f1(_f1), f2(_f2)
    { }

    const auto& operator()(int i)
    { return f1(f2(i)); }
};


template<typename F1, typename F2>
IndirectFunctionImpl<F1, F2> IndirectFunction(const F1& t, const F2& p)
{ return IndirectFunctionImpl<F1, F2>(t, p); }

template<typename F1, typename F2>
struct IndirectVecCollapseFunctionImpl
{
    F1 f1;
    F2 f2;

    IndirectVecCollapseFunctionImpl(const F1& _f1, const F2& _f2)
    : f1(_f1), f2(_f2)
    { }

    HashType operator()(int i)
    {
        const auto& c = f2(i);
        HashType r = 0;
        for(const auto& member : c)
            r += f1(member);
        return r;
    }
};


template<typename F1, typename F2>
IndirectVecCollapseFunctionImpl<F1, F2> IndirectVecCollapseFunction(const F1& t, const F2& p)
{ return IndirectVecCollapseFunctionImpl<F1, F2>(t, p); }
#endif

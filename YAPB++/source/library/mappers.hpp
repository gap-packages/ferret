#ifndef _MAPPERS_HPP
#define _MAPPERS_HPP

#include <set>
#include "library/vec1.hpp"
#include "perm.hpp"

// Some useful classes for filtering

// Transform a class with an operator[] to one with an operator()
template<typename T>
auto SquareBrackToFunction(T const* t)
{ return [t](int i){ return (*t)[i]; }; }

template<typename Container, typename T>
auto& CheckedMap(const Container& c, const T& t)
{
    auto it = c.find(t);
    D_ASSERT(it != c.end());
    return it->second;
}

template<typename F>
auto FunctionByPerm(F f, const Permutation& p)
{ return [f,p](auto i) { return f(p[i]); }; }

template<typename F1, typename F2>
auto IndirectFunction(const F1& t, const F2& p)
{ return [t,p](auto i) -> auto& { return t(p(i)); }; }


template<typename F1, typename F2, typename Val>
auto VecCollapseFuncInternal(F1& f1, F2& f2, Val i)
{
    const auto& c = f2(i);
    HashType r = 0;
    for(const auto& member : c)
        r += f1(member);
    return r;
}


template<typename F1, typename F2>
auto IndirectVecCollapseFunction(const F1& t, const F2& p)
{ return [t,p](auto i){ return VecCollapseFuncInternal(t,p,i);}; }
#endif

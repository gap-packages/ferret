#ifndef BASE1VEC_CDJXJIO
#define BASE1VEC_CDJXJIO

#include <vector>
#include <ostream>
#include "debug.hpp"

template<typename T>
class vec1
{
    std::vector<T> v;
public:
    typedef typename std::vector<T>::iterator iterator;
    typedef typename std::vector<T>::const_iterator const_iterator;
    typedef typename std::vector<T>::value_type value_type;
    vec1() { }

    vec1(unsigned size)
    : v(size)
    { }
#ifdef USE_CPP11
    vec1(std::initializer_list<T> l)
    : v(l)
    { }
#endif
    vec1(const vec1& vec) : v(vec.v) { }
#ifdef USE_CPP11
    vec1(vec1&& vec) : v(MOVE(vec.v)) { }
#endif
    void operator=(const vec1& vec)
    { v = vec.v; }
#ifdef USE_CPP11
    void operator=(vec1&& vec)
    { v = MOVE(vec.v); }
#endif
    template<typename It>
    vec1(It begin, It end)
    : v(begin, end)
    { }

    vec1(unsigned size, T val)
    : v(size, val)
    { }

    T& operator[](int i)
    {
        D_ASSERT(i <= this->size() && i != 0);
        return v[i-1];
    }

    const T& operator[](int i) const
    {
        D_ASSERT(i <= this->size() && i != 0);
        return v[i-1];
    }

    // Yes, this is 'int' on purpose. This is to avoid
    // signed/unsigned warnings all over the place.
    int size() const
    { return v.size(); }

    void reserve(int i)
    { v.reserve(i); }

    void resize(int i)
    { v.resize(i); }

    void resize(int i, const T& ele)
    { v.resize(i, ele); }

    bool empty() const
    { return v.empty(); }

    iterator begin()
    { return v.begin(); }

    const_iterator begin() const
    { return v.begin(); }

    iterator end()
    { return v.end(); }

    const_iterator end() const
    { return v.end(); }

    void push_back(const T& t)
    { v.push_back(t); }
#ifdef USE_CPP11
    void push_back(T&& t)
    { v.push_back(MOVE(t)); }
#endif
    void pop_back()
    { v.pop_back(); }

    void clear()
    { v.clear(); }

    T& back()
    { return v.back(); }

    const T& back() const
    { return v.back(); }

    T& front()
    { return v.front(); }

    const T& front() const
    { return v.front(); }


    template<typename It>
    void assign(It begin, It end)
    { v.assign(begin, end); }

    bool operator==(const vec1<T>& vec) const
    { return v == vec.v; }

    bool operator!=(const vec1<T>& vec) const
    { return v != vec.v; }

    bool operator<(const vec1<T>& vec) const
    { return v < vec.v; }

    friend std::ostream& operator<<(std::ostream& o, const vec1& vec)
    {
        o << "[";
        for(const_iterator it = vec.begin(); it != vec.end(); ++it)
        {
            o << *it << " ";
        }
        o << "]";
        return o;
    }
};

#endif

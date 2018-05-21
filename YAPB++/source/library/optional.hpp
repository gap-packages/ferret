#ifndef OPTIONAL_HPP
#define OPTIONAL_HPP

#include <assert.h>

template<typename T>
class optional
{
    T t;
    bool present;


    typedef void (optional::*bool_type)() const;
    void random_function_name() const {}
public:
    operator bool_type() const
    { 
        bool_type tr = &optional::random_function_name;
        bool_type f(0);
        return present ? tr : f;
    }

    void clear()
    { present = false; }

    T& operator*()
    {
        assert(present);
        return t;
    }

    const T& operator*() const
    {
        assert(present);
        return t;
    }

    optional() : t(), present(false)
    { }

    optional(const T& _t) : t(_t), present(true)
    { }

    optional(const optional& o) : t(o.t), present(o.present)
    { }

};

// This function is just to provide nicer looking notation
template<typename T>
optional<T> empty_optional()
{ return optional<T>(); }

#endif

#ifndef CPP11_CAJ_HEADER
#define CPP11_CAJ_HEADER


#if __cplusplus >= 201103L

#define MOVE(x) std::move(x)
#define DEFAULT_MOVE_COPY_CONST_ASSIGN(C) \
  C(const C&) = default; \
  C(C&&) = default; \
  C& operator=(const C&) = default; \
  C& operator=(C&&) = default;

#else

#define MOVE(x) x
#define DEFAULT_MOVE_COPY_CONST_ASSIGN(C) 

#endif


template<typename Iterator>
class Range
{
  Iterator b, e;
public:
  Range(Iterator _b, Iterator _e) : b(_b), e(_e)
  { }

  typedef Iterator iterator;
  //typedef const Iterator const_iterator;

  Iterator begin() const { return b; }
  Iterator end() const { return e; }

  friend std::ostream& operator<<(std::ostream& os, const Range& r)
  { return os << "[" << &*(r.b) << "," << &*(r.e) << ")"; }
};

template<typename Iterator>
Range<Iterator> rangeWrap(Iterator b, Iterator e)
{ return Range<Iterator>(b,e); }

namespace intrangeimpl {

template<typename T>
class IntlikeRangeIterator
{
    T value;
public:
    IntlikeRangeIterator(T value_)
        : value(value_){}

    bool operator!=(const IntlikeRangeIterator& other) const
    { return value != other.value; }

    T operator*() const
    { return value; }

    IntlikeRangeIterator& operator++()
    {
        ++value;
        return *this;
    }
};

template<typename T>
class IntlikeRange
{
    T const from;
    T const to;

public:
    IntlikeRange(T from_, T to_)
        : from(from_), to(to_){}

    IntlikeRange(T to_)
        : from(0), to(to_){}

    IntlikeRangeIterator<T> begin() const
    { return IntlikeRangeIterator<T>(from); }

    IntlikeRangeIterator<T> end() const
    { return IntlikeRangeIterator<T>(to); }
};

}

template<typename T>
intrangeimpl::IntlikeRange<T> xrange(T end) { return intrangeimpl::IntlikeRange<T>(end); }

template<typename T>
intrangeimpl::IntlikeRange<T> range1(T end) { return intrangeimpl::IntlikeRange<T>(1, end + 1); }


template<typename T>
intrangeimpl::IntlikeRange<T> xrange(T begin, T end) { return intrangeimpl::IntlikeRange<T>(begin, end); }


#endif

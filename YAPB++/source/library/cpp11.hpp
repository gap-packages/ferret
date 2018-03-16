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

namespace rangeimpl {

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

    int size() const
    { return to - from; }
};

template<typename Iterator, typename Func>
class FuncMapRangeIterator
{
    Iterator it;
    Func* f;
public:
    FuncMapRangeIterator(Iterator _it, Func* _f)
        : it(_it), f(_f) {}

    bool operator!=(const FuncMapRangeIterator& other) const
    { return it != other.it; }

    auto operator*() const
    { return (*f)(*it); }

    FuncMapRangeIterator& operator++()
    {
        ++it;
        return *this;
    }
};

template<typename Range, typename Func>
class FuncMapRange
{
    Range range;
    Func f;

    typedef decltype(range.begin()) Iterator;
public:
    FuncMapRange(Range _r, const Func& _f)
        : range(_r), f(_f) {}

    FuncMapRangeIterator<Iterator, const Func> begin() const
    { return FuncMapRangeIterator<Iterator, const Func>(range.begin(), &f); }

    FuncMapRangeIterator<Iterator, const Func> end() const
    { return FuncMapRangeIterator<Iterator, const Func>(range.end(), &f); }

    int size() const
    { return range.size(); }
};

}

template<typename T>
rangeimpl::IntlikeRange<T> xrange(T end) { return rangeimpl::IntlikeRange<T>(end); }

template<typename T>
rangeimpl::IntlikeRange<T> range1(T end) { return rangeimpl::IntlikeRange<T>(1, end + 1); }


template<typename T>
rangeimpl::IntlikeRange<T> xrange(T begin, T end) { return rangeimpl::IntlikeRange<T>(begin, end); }

template<typename Range, typename Func>
rangeimpl::FuncMapRange<Range, Func> maprange(Range&& range, Func&& f)
{ return rangeimpl::FuncMapRange<Range, Func>(range, f); }

#endif

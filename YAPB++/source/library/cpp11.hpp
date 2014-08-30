#ifndef CPP11_CAJ_HEADER
#define CPP11_CAJ_HEADER

#ifdef USE_CPP11

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

  Iterator begin() const { return b; }
  Iterator end() const { return e; }

  friend std::ostream& operator<<(std::ostream& os, const Range& r)
  { return os << "[" << &*(r.b) << "," << &*(r.e) << ")"; }
};

template<typename Iterator>
Range<Iterator> make_range(Iterator b, Iterator e)
{ return Range<Iterator>(b,e); }
#endif

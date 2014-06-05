#ifndef CPP11_CAJ_HEADER
#define CPP11_CAJ_HEADER

// there are better ways of checking for this which will catch more compilers
#if __cplusplus >= 201103L

#define CAJ_CPP11
#define MOVE(x) std::move(x)

#define DEFAULT_MOVE_COPY_CONST_ASSIGN(C) \
  C(const C&) = default; \
  C(C&&) = default; \
  C& operator=(const C&) & = default; \
  C& operator=(C&&) & = default;

#else

#define MOVE(x) x

#endif


#endif

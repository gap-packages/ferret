#ifndef CPP11_CAJ_HEADER
#define CPP11_CAJ_HEADER

// there are better ways of checking for this which will catch more compilers
#if __cplusplus >= 201103L

#define CAJ_CPP11
#define MOVE(x) std::move(x)

#else

#define MOVE(x) x

#endif


#endif

#ifndef CPP11_CAJ_HEADER
#define CPP11_CAJ_HEADER

#define MOVE(x) std::move(x)

#define DEFAULT_MOVE_COPY_CONST_ASSIGN(C) \
  C(const C&) = default; \
  C(C&&) = default; \
  C& operator=(const C&) & = default; \
  C& operator=(C&&) & = default;

#endif

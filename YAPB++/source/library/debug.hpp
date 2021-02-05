#ifndef DEBUG_HPP_CVCV
#define DEBUG_HPP_CVCV

#include <sstream>
#include "info.hpp"
#include <assert.h>

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 1
#endif

#if DEBUG_LEVEL == 0
  #define D_ASSERT(x)
  #define D_SLOW_ASSERT(x)
#else
  #if DEBUG_LEVEL == 1
  #define D_ASSERT(x) assert(x)
  #define D_SLOW_ASSERT(x)
  #else
    #if DEBUG_LEVEL == 2
    #define D_ASSERT(x) assert(x)
    #define D_SLOW_ASSERT(x) assert(x)
    #else
      #error Invalid error level
    #endif
  #endif
#endif

#ifdef DEBUG_PRINT_LEVEL

#ifdef  GAP_COMPILED_H
#define debug_out(level, type, message) \
{ \
  if(level < DebugInfoLevel()) \
  { \
    std::ostringstream oss(""); \
    oss << "#I " << type << ":" << message << "\n"; \
    GAP_print(oss.str()); \
  } \
}
#else
#define debug_out(level, type, message) \
{ \
  if(level < DebugInfoLevel()) \
  { \
    std::cerr << "#I " << type << ":" << message << "\n"; \
  } \
}
#endif

#else
#define debug_out(...)
#endif

static const int blank_value = -999999;

#endif

// Make sure C++14 has been checked
#include "cpp11.hpp"

#ifndef _VERSION_HPP
#define _VERSION_HPP

const static char* version =
"YAPB v0.1 :"

#ifdef NO_DEBUG
"Release : "
#else
"Debug : "
#endif

#ifdef ENABLE_TIMING
"Timing : "
#endif

#ifdef DEBUG_PRINT_LEVEL
"Debug output : "
#endif

;

#endif

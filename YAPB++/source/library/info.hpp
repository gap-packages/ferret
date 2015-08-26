#ifndef INFO_HPP_AOPQ
#define INFO_HPP_AOPQ

inline int& InfoLevel()
{
  static int InfoLevel = 0;
  return InfoLevel;
}

inline int& DebugInfoLevel()
{
  static int DebugInfoLevel = 0;
  return DebugInfoLevel;
}

#ifdef  GAP_COMPILED_H

#define info_out(level, output) \
  { if(level <= InfoLevel()) { \
      std::ostringstream oss; \
      oss << "#I " << output << "\n"; \
      GAP_print(oss.str()); \
    } }
#else
#define info_out(level, output) \
  { if(level <= InfoLevel()) { \
    std::cerr << "#I " << output << "\n"; \
    } }
#endif

#endif

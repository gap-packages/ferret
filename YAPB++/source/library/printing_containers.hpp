/** \weakgroup MinLib
 * @{
 */

#ifndef TOSTRING_H
#define TOSTRING_H


#include <ostream>
#include <vector>
#include <set>
#include <list>
#include <map>
#include "optional.hpp"

template<typename T>
void output_container(std::ostream& o, const T& t);

namespace std {
template<typename T, typename U>
std::ostream& operator<<(std::ostream& o, const std::pair<T,U>& p)
{ return o << "(" << p.first << "," << p.second << ")"; }

template<typename T>
std::ostream& operator<<(std::ostream& o, const std::vector<T>& t)
{
  output_container(o, t);
  return o;
}

template<typename T>
std::ostream& operator<<(std::ostream& o, const std::list<T>& t)
{
  output_container(o, t);
  return o;
}

template<typename T>
std::ostream& operator<<(std::ostream& o, const std::set<T>& t)
{
  output_container(o, t);
  return o;
}

template<typename T>
std::ostream& operator<<(std::ostream& o, const std::multiset<T>& t)
{
  output_container(o, t);
  return o;
}

template<typename T, typename U>
std::ostream& operator<<(std::ostream& o, const std::map<T,U>& t)
{
	output_container(o, t);
	return o;
}

}


template<typename T>
void output_container(std::ostream& o, const T& t)
{
  o << "[";
  if(!t.empty())
  {
    typename T::const_iterator it(t.begin());
    o << *it;
    ++it;
    for(; it != t.end(); ++it)
    {
      o << ",";
      o << *it;
    }
  }
  o << "]";
}

template<typename T>
std::string toString(const T& t)
{
  std::ostringstream oss("");
  oss << t;
  return oss.str();
}


#endif
/** @}
 */

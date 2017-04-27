#ifndef _TIMING_HPP
#define _TIMING_HPP

#include <string>
#include <vector>
#include <utility>
#include <map>
#include <time.h>
#include <assert.h>

#ifdef ENABLE_TIMING

double get_clock()
{
    timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return t.tv_sec + (t.tv_nsec / (double)1000000000);
}

std::map<std::string, double> func_calls;
std::map<std::string, double> func_calls_start_time;
std::map<std::string, long> timing_counters;
// A simple, fast, global timing mechanism

void timing_event(const std::string& name)
{
    timing_counters[name]++;
}

void timing_start(const std::string& name)
{
    timing_event(name);
    assert(func_calls_start_time[name] == 0);
    func_calls_start_time[name] = get_clock();
}
void timing_end(const std::string& name)
{
    auto& ref = func_calls_start_time[name];
    assert(ref != 0);
    func_calls[name] += get_clock() - ref;
    ref = 0;
}

void timing_reset()
{
    func_calls_start_time.clear();
    func_calls.clear();
    timing_counters.clear();
}


#else

#define timing_start(X)
#define timing_end(X)
#define timing_event(X)
#define timing_reset()
#endif

#endif

#ifndef _TIMING_HPP
#define _TIMING_HPP

#include <string>
#include <vector>
#include <utility>
#include <map>
#include <time.h>

#ifdef FERRET_TIMING

double get_clock()
{
    timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return t.tv_sec + (t.tv_nsec / (double)1000000000);
}

std::map<std::string, double> func_calls;
std::map<std::string, double> func_calls_start_time;
// A simple, fast, global timing mechanism

std::vector<std::pair<std::string, double> > events_store;

double time_start;

void timing_start()
{ time_start = get_clock(); }

void timing_finish()
{
    std::cout << "Total time: " << get_clock() - time_start << "\n";
}

double GAP_call_start;
void timing_start_GAP_call(const std::string& name)
{
    assert(func_calls_start_time[name] == 0);
    func_calls_start_time[name] = get_clock();
}
void timing_end_GAP_call(const std::string& name)
{ 
    auto& ref = func_calls_start_time[name];
    assert(ref != 0);
    func_calls[name] += get_clock() - ref;
    ref = 0;
}

void timing_event(const std::string& name)
{
    events_store.push_back(std::make_pair(name, get_clock() - time_start));
    std::cout << "event '" << events_store.back().first << "' : " << std::fixed << events_store.back().second << "\n";
}


#else

#define timing_start()
#define timing_finish()
#define timing_start_GAP_call(X)
#define timing_end_GAP_call()
#define timing_event(X)
#endif

#endif

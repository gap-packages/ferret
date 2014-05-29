#ifndef _TIMING_HPP
#define _TIMING_HPP

#include <string>
#include <vector>
#include <utility>
#include <time.h>

#ifdef ENABLE_TIMING

double get_clock()
{
    timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return t.tv_sec + (t.tv_nsec / (double)1000000000);
}

std::vector<std::pair<std::string, double> > GAP_calls;
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
    GAP_calls.push_back(std::make_pair(name, 0.0));
    GAP_call_start = get_clock();
}
void timing_end_GAP_call()
{ 
    GAP_calls.back().second = get_clock() - GAP_call_start; 
    std::cout << "GAP call '" << GAP_calls.back().first << "' : " << std::fixed << GAP_calls.back().second << "\n";
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

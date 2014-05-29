#include "memory_backtrack.hpp"
#include <assert.h>

int main(void)
{
    MemoryBacktracker mb;
    RevertingStack<std::pair<int, int> > i = mb.makeRevertingStack<std::pair<int,int> >();

    assert(i.get().size() == 0);

    i.push_back(std::make_pair(1,2));
    assert(i.get().size() == 1);
    mb.pushWorld();
    assert(i.get().size() == 1);
    i.push_back(std::make_pair(2,3));
    assert(i.get().size() == 2);
    assert(i.get()[1] == std::make_pair(1,2));
    assert(i.get()[2] == std::make_pair(2,3));
    mb.popWorld();
    assert(i.get().size() == 1);
    assert(i.get()[1] == std::make_pair(1,2));
}

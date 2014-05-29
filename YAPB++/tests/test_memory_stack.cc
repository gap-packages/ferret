#include "memory_backtrack.hpp"
#include <assert.h>

int main(void)
{
    MemoryBacktracker mb;
    RevertingStack<int> i = mb.makeRevertingStack<int>();

    assert(i.get().size() == 0);

    i.push_back(1);
    assert(i.get().size() == 1);
    mb.pushWorld();
    assert(i.get().size() == 1);
    i.push_back(2);
    assert(i.get().size() == 2);
    assert(i.get()[1] == 1);
    assert(i.get()[2] == 2);
    mb.popWorld();
    assert(i.get().size() == 1);
    assert(i.get()[1] == 1);
}

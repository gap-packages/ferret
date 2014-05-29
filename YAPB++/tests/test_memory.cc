#include "memory_backtrack.hpp"
#include <assert.h>

int main(void)
{
    MemoryBacktracker mb;
    Reverting<int> i = mb.makeReverting<int>();

    i.set(1);
    assert(i.get() == 1);
    mb.pushWorld();
    assert(i.get() == 1);
    i.set(2);
    assert(i.get() == 2);
    i.set(-1);
    assert(i.get() == -1);
    mb.pushWorld();
    i.set(3);
    assert(i.get() == 3);
    i.set(4);
    assert(i.get() == 4);
    mb.popWorld();
    assert(i.get() == -1);
    mb.popWorld();
    assert(i.get() == 1);
}

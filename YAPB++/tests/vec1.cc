#include "library/vec1.hpp"
#include "assert.h"

int main(void)
{
    vec1<int> v;
    assert(v.size() == 0);
    v.push_back(1);
    assert(v.size() == 1);
    assert(v[1] == 1);
}
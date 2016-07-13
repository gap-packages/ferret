#ifndef _FAILURE_CHECK_AKLE
#define _FAILURE_CHECK_AKLE

class SplitState
{
    bool success;

public:
    explicit SplitState(bool b)
    : success(b)
    { }

    bool hasFailed() const
    { return !success; }

    friend std::ostream& operator<<(std::ostream& o, SplitState s)
    { return o << (s.hasFailed() ? "fail" : "pass"); }
};

#endif

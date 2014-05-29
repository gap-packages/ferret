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

};

#endif

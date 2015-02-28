#ifndef GAP_FUNCTION_HPPQR
#define GAP_FUNCTION_HPPQR

#include <string>

class GAPFunction
{
    Obj obj;
    std::string name;

public:
    GAPFunction() : obj(0), name()
    { }
    
    GAPFunction(std::string s) : obj(0), name(s)
    { }

    void setName(std::string s)
    { name = s; }
    
    Obj getObj()
    {
        if(obj == 0)
        {
            UInt varname = GVarName(name.c_str());
            obj = VAL_GVAR(varname);
        }
        return obj;
    }
};

#endif

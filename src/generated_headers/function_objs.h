
#ifndef FUNOBJ
#define FUNOBJ(x,y) extern GAPFunction FunObj_ ## x;

typedef struct
{
    Obj obj;
    const char* name;
}  GAPFunction;

#endif


FUNOBJ(addRef, "_YAPB_addRef");
FUNOBJ(checkRef, "_YAPB_checkRef");
FUNOBJ(clearRefs,"_YAPB_clearRefs");
FUNOBJ(YAPB_FixedOrbits, "_YAPB_FixedOrbits");
FUNOBJ(YAPB_RepresentElement, "_YAPB_RepresentElement");
FUNOBJ(YAPB_getPermTo, "_YAPB_getPermTo");
FUNOBJ(StabChainMutable, "StabChainMutable");
FUNOBJ(ChangeStabChain, "ChangeStabChain");
FUNOBJ(getOrbitPart, "_YAPB_getOrbitPart");
FUNOBJ(inGroup, "_YAPB_inGroup");
FUNOBJ(isGroupConj, "_YAPB_isGroupConj");
FUNOBJ(getBlockList, "_YAPB_getBlockList");
#undef FUNOBJ
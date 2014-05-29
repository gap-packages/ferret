/****************************************************************************

This is a prototype of a HELLO_WORLD example implemented as a kernel module.

An example of its compilation:

dyn-194-155:src alexk$ ~/gap4r4/bin/i686-apple-darwin10.4.0-gcc/gac -d hellod.c
gcc -fPIC -o /var/folders/NB/NBxYeZaRHxGpf4haob7SqE+++TQ/-Tmp-//gac36146/36146_hellod.o -I/Users/alexk/gap4r4/bin/i686-apple-darwin10.4.0-gcc/../.. -I/Users/alexk/gap4r4/bin/i686-apple-darwin10.4.0-gcc -DCONFIG_H -c hellod.c
ld -bundle -bundle_loader /Users/alexk/gap4r4/bin/i686-apple-darwin10.4.0-gcc/gap /usr/lib/bundle1.o -lc -lm -o hellod.so /var/folders/NB/NBxYeZaRHxGpf4haob7SqE+++TQ/-Tmp-//gac36146/36146_hellod.o
rm -f /var/folders/NB/NBxYeZaRHxGpf4haob7SqE+++TQ/-Tmp-//gac36146/36146_hellod.o

It works as follows:

gap> LoadDynamicModule("hellod.so");
gap> HELLO_WORLD();
Hello World!
gap>

It may be included in one of the future versions of the Example package.
However, this will require documenting more rules of kernel programming.

****************************************************************************/

#include <stdio.h>
#include "include_gap_headers.h"

/*
Obj FuncHELLO_WORLD( Obj self ) {
  Pr("Hello World!\n",0L, 0L);
  return (Obj) 0;
  }

Obj FuncADD_ONE_SMALL_INT( Obj self, Obj arg ) {

  if(!IS_INTOBJ(arg))
  {
    SyClearErrorNo();
    return Fail;
  }

  int val = INT_INTOBJ(arg);

  return INTOBJ_INT(val + 1);
}

Obj FuncSIMPLE_FUNCTION_CALL_WRAPPER(Obj self, Obj func, Obj arg1)
{
  // This just calls a function with a single argument:
  if(!IS_FUNC(func))
  {
    SyClearErrorNo();
    return Fail;
  }
  Obj ret = CALL_1ARGS(func, arg1);
  return ret;
}

Obj FuncMAKE_EXAMPLE_LIST( Obj self ) {
  // This only allocates space for the list
  Obj list = NEW_PLIST(T_PLIST_DENSE, 5);
  // This sets the actual length of the list
  SET_LEN_PLIST(list, 5);
  int i;
  for(i = 1; i <= 5; ++i)
    SET_ELM_PLIST(list, i, INTOBJ_INT(i));
  return list;
}

Obj FuncMAKE_EXAMPLE_LIST_LIST( Obj self ) {
  // This only allocates space for the list
  Obj list = NEW_PLIST(T_PLIST_DENSE, 5);
  // This sets the actual length of the list
  SET_LEN_PLIST(list, 5);
  int i;
  for(i = 1; i <= 5; ++i)
  {
    Obj newlist = NEW_PLIST(T_PLIST_DENSE, 2);
    SET_LEN_PLIST(newlist, 2);
    SET_ELM_PLIST(newlist, 1, INTOBJ_INT(i*10));
    SET_ELM_PLIST(newlist, 2, INTOBJ_INT(i*20));
    SET_ELM_PLIST(list, i, newlist);
    // (TOCHECK)
    // We have to call CHANGED_BAG before we exit
    // this loop, or a garbage collection could lose
    // newlist!
    CHANGED_BAG(list);
  }
  return list;
}

Obj FuncRECORD_FUN(Obj self, Obj arg) {
  // Check arg is a record
  if(!IS_PREC_REP(arg))
  {
    SyClearErrorNo();
    return Fail;
  }

  // make a new record
  Obj rec = NEW_PREC(0);

  // set a member
  AssPRec(rec, RNamName("one"), INTOBJ_INT(1));

  // check if record has memory called 'in'
  if(IsbPRec(arg, RNamName("inval")))
  {
    // Read the member out
    Obj recval = ElmPRec(arg, RNamName("inval"));
    // and assign it in a different record
    AssPRec(rec, RNamName("outval"), recval);
  }

  CHANGED_BAG(rec);
  return rec;
}
*/

extern "C" {

Obj FuncSOLVE(Obj self, Obj conlist, Obj options) {
  (void)self; // remove warning

  if(!IS_REC(options))
  {
    fprintf(stderr, "Not a record!");
  }
  return solver(conlist, options);
}

static StructGVarFunc GVarFuncs [] = {
/*
{ "HELLO_WORLD", 0, "", FuncHELLO_WORLD, "src/string.c:FuncHELLO_WORLD" },
{ "ADD_ONE_SMALL_INT", 1, "list", FuncADD_ONE_SMALL_INT, "src/string.c:FuncADD_ONE_SMALL_INT" },
{ "MAKE_EXAMPLE_LIST", 0, "", FuncMAKE_EXAMPLE_LIST, "src/string.c:FuncMAKE_EXAMPLE_LIST" },
{ "MAKE_EXAMPLE_LIST_LIST", 0, "", FuncMAKE_EXAMPLE_LIST_LIST, "src/string.c:FuncMAKE_EXAMPLE_LIST_LIST" },
{ "RECORD_FUN", 1, "record", FuncRECORD_FUN, "src/string.c:FuncRECORD_FUN" },
{ "SIMPLE_FUNCTION_CALL_WRAPPER", 2, "function, object", FuncSIMPLE_FUNCTION_CALL_WRAPPER, "src/string.c:FuncSIMPLE_FUNCTION_CALL_WRAPPER" },
*/
  { "YAPB_SOLVE", 2, "object, object", (UInt**(*)())FuncSOLVE, "src/string.c:FuncSOLVE" },
{ 0 }

};


/**************************************************************************

*F  InitKernel( <module> )  . . . . . . . . initialise kernel data structures
*/
static Int InitKernel (
    StructInitInfo *    module )
{
    (void)module; // remove warning

    /* init filters and functions                                          */
    InitHdlrFuncsFromTable( GVarFuncs );

    /* return success                                                      */
    return 0;
}


/****************************************************************************
**
*F  InitLibrary( <module> ) . . . . . . .  initialise library data structures
*/

#include "generated_headers/RNamNames_list.h"
#include "generated_headers/function_objs.h"

#define RNAM(x) UInt RName_ ## x;
#include "generated_headers/RNamNames_list.h"


#define FUNOBJ(x,y) GAPFunction FunObj_##x;
#include "generated_headers/function_objs.h"


static Int InitLibrary (
    StructInitInfo *    module )
{
    (void)module; // remove warning

    InitGVarFuncsFromTable( GVarFuncs );

#define FUNOBJ(x,y) FunObj_##x .obj = VAL_GVAR(GVarName(y)); FunObj_##x .name = y;
#include "generated_headers/function_objs.h"


#define RNAM(x) RName_ ## x = RNamName(#x);
#include "generated_headers/RNamNames_list.h"


    /* return success                                                      */
    return 0;
}


/****************************************************************************
**
*F  InitInfopl()  . . . . . . . . . . . . . . . . . table of init functions
*/
/* <name> returns the description of this module */
static StructInitInfo module = {
#ifdef EDIVSTATIC
 /* type        = */ MODULE_STATIC,
#else
 /* type        = */ MODULE_DYNAMIC,
#endif
 /* name        = */ "hello",
 /* revision_c  = */ 0,
 /* revision_h  = */ 0,
 /* version     = */ 0,
 /* crc         = */ 0,
 /* initKernel  = */ InitKernel,
 /* initLibrary = */ InitLibrary,
 /* checkInit   = */ 0,
 /* preSave     = */ 0,
 /* postSave    = */ 0,
 /* postRestore = */ 0,
 /* filename    = */ 0,
 /* isGapRootRelative = */ 0
};

#ifndef EDIVSTATIC
StructInitInfo * Init__Dynamic ( void )
{
 return &module;
}
#endif

StructInitInfo * Init__ediv ( void )
{
  return &module;
}
}

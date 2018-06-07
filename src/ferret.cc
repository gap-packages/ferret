/****************************************************************************

****************************************************************************/

#include <stdio.h>
#include "include_gap_headers.hpp"
#include "c_to_cpp.h"
#include "generated_headers/function_objs.h"

#define FUNOBJ(x,y) GAPFunction FunObj_##x;
#include "generated_headers/function_objs.h"

extern "C" {

Obj FuncSOLVE(Obj self, Obj conlist, Obj options) {
  (void)self; // remove warning

  if(!IS_REC(options))
  {
    fprintf(stderr, "Not a record!");
  }
  return solver(conlist, options);
}

Obj FuncSOLVE_COSET(Obj self, Obj conlistC, Obj conlistL, Obj conlistR, Obj options) {
  (void)self; // remove warning

  if(!IS_REC(options))
  {
    fprintf(stderr, "Not a record!");
  }
  return cosetSolver(conlistC, conlistL, conlistR, options);
}

static StructGVarFunc GVarFuncs [] = {
  { "YAPB_SOLVE", 2, "object, object", (UInt**(*)())FuncSOLVE, "src/string.c:FuncSOLVE" },
  { "YAPB_SOLVE_COSET", 4, "object, object, object, object", (UInt**(*)())FuncSOLVE_COSET, "src/string.c:FuncSOLVE_COSET" },

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

#define RNAM(x) UInt RName_ ## x;
#include "generated_headers/RNamNames_list.h"


static Int InitLibrary (
    StructInitInfo *    module )
{
    (void)module; // remove warning

    InitGVarFuncsFromTable( GVarFuncs );

#define FUNOBJ(x,y) FunObj_##x .setName(y);
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
 /* type        = */ MODULE_DYNAMIC,
 /* name        = */ "ferret",
 /* revision_c  = */ 0,
 /* revision_h  = */ 0,
 /* version     = */ 0,
 /* crc         = */ 0,
 /* initKernel  = */ InitKernel,
 /* initLibrary = */ InitLibrary,
 /* checkInit   = */ 0,
 /* preSave     = */ 0,
 /* postSave    = */ 0,
 /* postRestore = */ 0
};

StructInitInfo * Init__Dynamic ( void )
{
 return &module;
}

}

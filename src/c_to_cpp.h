#ifndef C_TO_CPP_HEADER
#define C_TO_CPP_HEADER

#ifdef __cplusplus
extern "C" {
#endif

Obj solver(Obj conlist, Obj options);
Obj cosetSolver(Obj conlist1, Obj conlist2, Obj options);

#ifdef __cplusplus
}
#endif

#endif

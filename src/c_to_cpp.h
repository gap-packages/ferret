#ifndef C_TO_CPP_HEADER
#define C_TO_CPP_HEADER

#ifdef __cplusplus
extern "C" {
#endif

Obj solver(Obj conlist, Obj options);
Obj cosetSolver(Obj conlistC, Obj conlistL, Obj conlistR, Obj options);

#ifdef __cplusplus
}
#endif

#endif

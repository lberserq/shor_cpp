#ifndef PARALLEL_SUBSYSTEM_TEMPLATES_IMPL_H
#define PARALLEL_SUBSYSTEM_TEMPLATES_IMPL_H
#include <config.h>

#define GDB_ENABLE
#ifndef GDB_ENABLE
#define pragma_omp_parallel_for _Pragma(STRINGIFY(omp parallel for))
#else
#define pragma_omp_parallel_for
#endif

#define DeclareThreadsVectorOp(_funcName, _op) template<class T> void ParallelSubSystemHelper::thread::_funcName(T *first, T *second, size_t size)\
{\
    pragma_omp_parallel_for\
    for (size_t i = 0; i < size; ++i) {\
        first[i] _op second[i];\
    }\
}\


DeclareThreadsVectorOp(parallelAddAssign, +=)
DeclareThreadsVectorOp(parallelSubAssign, -=)
DeclareThreadsVectorOp(parallelMulAssign, *=)
DeclareThreadsVectorOp(parallelDivAssign, /=)


#endif // PARALLEL_SUBSYSTEM_TEMPLATES_IMPL_H


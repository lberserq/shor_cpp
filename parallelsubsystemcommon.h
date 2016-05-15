#ifndef PARALLEL_SUBSYSTEM_TEMPLATES_IMPL_H
#define PARALLEL_SUBSYSTEM_TEMPLATES_IMPL_H
#include <config.h>


#define DeclareThreadsVectorOp(_funcName, _op) template<class T> void ParallelSubSystemHelper::thread::_funcName(T *first, T *second, size_t size)\
{\
_Pragma(STRINGIFY(omp parallel for))\
    for (size_t i = 0; i < size; ++i) {\
        first[i] _op second[i];\
    }\
}\


DeclareThreadsVectorOp(parallelAddAssign, +=)
DeclareThreadsVectorOp(parallelSubAssign, -=)
DeclareThreadsVectorOp(parallelMulAssign, *=)
DeclareThreadsVectorOp(parallelDivAssign, /=)


#endif // PARALLEL_SUBSYSTEM_TEMPLATES_IMPL_H


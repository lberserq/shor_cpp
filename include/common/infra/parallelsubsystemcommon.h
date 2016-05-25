#ifndef PARALLEL_SUBSYSTEM_TEMPLATES_IMPL_H
#define PARALLEL_SUBSYSTEM_TEMPLATES_IMPL_H
#include <common/config.h>

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

#define X_SAFE_COMMON_CALL_IMPL(_expr, _allResult, _RESULT_MPI_TYPE, _SAFE_OP)\
{\
    result_t _exprResult = (_expr);\
    result_t _globalResult = _exprResult;\
    if (ParallelSubSystemHelper::isInited()) {\
        MPI_SAFE_CALL(MPI_Allreduce(&_exprResult, &_globalResult, 1, _RESULT_MPI_TYPE, _SAFE_OP, MPI_COMM_WORLD));\
    }\
    _allResult = _globalResult;\
}\

#define X_SAFE_COMMON_ONECALL_IMPL(_expr, _allResult, _RESULT_MPI_TYPE, _rank)\
{\
    result_t _globResult;\
    if (!ParallelSubSystemHelper::isInited() || ParallelSubSystemHelper::getConfig().rank == _rank) {\
        result_t _exprResult = (_expr);\
        _globResult = _exprResult;\
    }\
    MPI_SAFE_CALL(MPI_Bcast(&_globResult, 1, _RESULT_MPI_TYPE, _rank, MPI_COMM_WORLD));\
}\



#define X_SAFE_COMMON_CALL_ALL(_expr, _allResult) X_SAFE_COMMON_CALL_IMPL(_expr, _allResult, MPI_RESULT_T, MPI_BOR)
#define X_SAFE_COMMON_CALL_ANYONE(_expr, _allResult) X_SAFE_COMMON_CALL_IMPL(_expr, _allResult, MPI_RESULT_T, MPI_BAND)
#define X_SAFE_COMMON_CALL_ONE(_expr, _allResult, _rank) X_SAFE_COMMON_ONECALL_IMPL(_expr, _allResult, MPI_RESULT_T, _rank)
#define X_SAFE_COMMON_CALL_ROOT(_expr, _allResult) X_SAFE_COMMON_CALL_ONE(_expr, _allResult, 0)

#endif // PARALLEL_SUBSYSTEM_TEMPLATES_IMPL_H


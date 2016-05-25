#include "simpleperfcounter.h"
#include <common/infra/parallelsubsystemcommon.h>
#include <time.h>
SimplePerfCounter::SimplePerfCounter()
{
    memset(&m_startTime, 0, sizeof(m_startTime));
}

result_t SimplePerfCounter::StartCounter()
{
    result_t res;
    X_SAFE_COMMON_CALL_ALL(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &m_startTime), res);
    return res;
}

result_t SimplePerfCounter::StopCounter(real_t *outValue)
{
    X_ASSERT(outValue);

    struct timespec spec;
    memset(&spec, 0, sizeof(spec));

    result_t res = X_SOK;
    X_SAFE_COMMON_CALL_ALL(clock_getres(CLOCK_PROCESS_CPUTIME_ID, &spec), res);

    if (X_FAILED(res))
        return res;
    struct timespec end;
    X_SAFE_COMMON_CALL_ALL(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end), res);
    real_t val = (end.tv_sec - m_startTime.tv_sec) + (end.tv_nsec - m_startTime.tv_nsec) * 1e-9;
    if (ParallelSubSystemHelper::isInited()) {
        real_t gVal = val;
        MPI_SAFE_CALL(MPI_Allreduce(&val, &gVal, 1, MPI_REAL_T, MPI_MAX, MPI_COMM_WORLD));
        val = gVal;
    }
    *outValue = val;
    return X_SOK;
}


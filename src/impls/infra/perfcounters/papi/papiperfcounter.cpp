#include "papiperfcounter.h"
#include <papi.h>
#include <common/infra/parallelsubsystemcommon.h>

const int maxEvents = 2;
PapiPerfCounter::PapiPerfCounter()
    :m_events(maxEvents, 0),
      m_eventSet(0)
{
    X_ASSERT(PAPI_library_init(PAPI_VER_CURRENT) == PAPI_VER_CURRENT);
    if(PAPI_query_event(PAPI_FP_OPS) == PAPI_OK) {
        m_events[0] = PAPI_FP_OPS;
    } else  {
        X_ASSERT(PAPI_query_event(PAPI_TOT_INS) == PAPI_OK);
        m_events[0] = PAPI_TOT_INS;
    }

    if(PAPI_query_event(PAPI_TOT_CYC) == PAPI_OK) {
        m_events[1] = PAPI_TOT_CYC;
    } else  {
        X_ASSERT(PAPI_query_event(PAPI_TOT_INS) == PAPI_OK);
        m_events[1] = PAPI_TOT_INS;
    }
}

result_t PapiPerfCounter::StartCounter()
{
    result_t res;
    X_SAFE_COMMON_CALL_ALL(PAPI_start_counters(&m_events[0], maxEvents), res);
    return res;
}


result_t PapiPerfCounter::StopCounter(real_t *outValue)
{
    X_ASSERT(outValue);
    long_long vals[maxEvents];

    result_t pres = 0;
    X_SAFE_COMMON_CALL_ALL(PAPI_stop_counters(vals, maxEvents), pres);
    if (pres != PAPI_OK)
        return X_FAIL;

    if (ParallelSubSystemHelper::isInited())
    {
        long_long gval = 0;
        MPI_SAFE_CALL(MPI_Allreduce(&vals[0], &gval, 1, MPI_LONG_LONG, MPI_MAX, MPI_COMM_WORLD));
        vals[0] = gval;
    }

    *outValue = static_cast<real_t>(vals[0]);
    return X_SOK;
}

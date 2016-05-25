#ifndef SIMPLEPERFCOUNTER_H
#define SIMPLEPERFCOUNTER_H
#include <iface/infra/iperfcounter.h>
#include <time.h>
class SimplePerfCounter : public IPerfCounter
{
    struct timespec m_startTime;
public:
    SimplePerfCounter();
    result_t StartCounter();
    result_t StopCounter(real_t *outValue);
};

#endif // SIMPLEPERFCOUNTER_H

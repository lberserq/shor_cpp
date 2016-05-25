#ifndef PAPIPERFCOUNTER_H
#define PAPIPERFCOUNTER_H
#include <iface/infra/iperfcounter.h>

class PapiPerfCounter : public IPerfCounter
{
    std::vector<int> m_events;
    int m_eventSet;
public:
    PapiPerfCounter();
    result_t StartCounter();
    result_t StopCounter(real_t *outValue);
};

#endif // PAPIPERFCOUNTER_H

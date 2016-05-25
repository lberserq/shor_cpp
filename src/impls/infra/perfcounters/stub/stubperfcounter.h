#ifndef STUBPERFCOUNTER_H
#define STUBPERFCOUNTER_H

#include <iface/infra/iperfcounter.h>
class StubPerfCounter : public IPerfCounter
{
public:
    result_t StartCounter(){return X_SOK;}
    result_t StopCounter(real_t *){return X_SOK;}
};

#endif // STUBPERFCOUNTER_H


#ifndef IPERFCOUNTER_H
#define IPERFCOUNTER_H
#include <common/config.h>
#include <common/utils.h>
struct IPerfCounter : private utils::NonCopyable
{
    virtual result_t StartCounter() = 0;
    virtual result_t StopCounter(real_t *outValue) = 0;
    virtual ~IPerfCounter(){}
};

#endif // IPERFCOUNTER_H


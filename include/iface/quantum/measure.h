#ifndef MEASURE_H
#define MEASURE_H
#include <iface/quantum/iqregister.h>
/*!
 *\class
 *\brief static class with method Measure(Measured register with the "natural" basis
 *
 */


class Measurer
{
public:
    static state_t Measure(IQRegister &reg);
};

#endif // MEASURE_H

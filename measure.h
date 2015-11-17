#ifndef MEASURE_H
#define MEASURE_H
#include "qregister.h"
/*!
 *\class
 *\brief static class with method Measure(Measured register with the "natural" basis
 *
 */
class Measurer
{
public:
    static state Measure(IQRegister &reg);
};

#endif // MEASURE_H

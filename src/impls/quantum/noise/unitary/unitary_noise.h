#ifndef UNITARY_NOISE_H
#define UNITARY_NOISE_H
#include <iface/quantum/inoise.h>
#include <vector>

class UnitaryNoiseImpl: public IOperatorNoise
{
public:
    QMatrix GenNoisyMatrix(const QMatrix &m);
};

#endif // UNITARY_NOISE_H

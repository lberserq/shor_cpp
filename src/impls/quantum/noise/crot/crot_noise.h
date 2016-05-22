#ifndef CROT_NOISE
#define CROT_NOISE
#include <iface/quantum/inoise.h>
#include <vector>

class CRotNoiseImpl: public IOperatorNoise
{
public:
    QMatrix GenNoisyMatrix(const QMatrix &m);
};

#endif // CROT_NOISE

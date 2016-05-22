#ifndef STUBS_NOISE_H
#define STUBS_NOISE_H

#include <iface/quantum/inoise.h>
class NoiseDensityStub: public IDensityMatrixNoise
{
public:
    NoiseDensityStub(const std::string &, int) {}
    void ApplyNoiseForDensityMatrix(IQRegister &) {}
};

class NoNoiseImpl: public IOperatorNoise
{
public:
    //explicit NoNoiseImpl(const std::string &path = std::string()) {}
    QMatrix GenNoisyMatrix(const QMatrix &m) {
        return m;
    }
};
#endif // STUBS_NOISE_H


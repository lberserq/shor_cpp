#ifndef NOISE_H
#define NOISE_H
#include "inoise.h"
#include <vector>

class UnitaryNoiseImpl: public IOperatorNoise
{
public:
    QMatrix GenNoisyMatrix(const QMatrix &m);
};

enum {
    CRAUSS_MAX_DIM_V1 = 3
    };
class CraussNoiseImpl: public IOperatorNoise {
    std::vector<QMatrix> m_CraussOps[CRAUSS_MAX_DIM_V1];


public:
    CraussNoiseImpl(const std::string &filename);
    QMatrix GenNoisyMatrix(const QMatrix &m);
};

class NoNoiseImpl: public IOperatorNoise
{
public:
    QMatrix GenNoisyMatrix(const QMatrix &m) {
        return m;
    }
};

class CraussNoiseDensityImpl: public IDensityMatrixNoise
{
    std::vector<QMatrix> m_CraussOps;
    int m_dim;
public:
    CraussNoiseDensityImpl(const std::string &filename, int dim);
    void ApplyNoiseForDensityMatrix(IQRegister &reg);
};

class SimpleNoiseProvider : public INoiseProvider {
    IDensityMatrixNoise *m_densNoise;
    IOperatorNoise *m_opNoise;
public:
    SimpleNoiseProvider(IDensityMatrixNoise * densNoise,
                       IOperatorNoise * opNoise) : m_densNoise(densNoise), m_opNoise(opNoise)
    {}
    IOperatorNoise * GetOperatorNoise() {
        return m_opNoise;
    }

    IDensityMatrixNoise * GetDensityMatrixNoise() {
        return m_densNoise;
    }
    ~SimpleNoiseProvider() {
        if (m_opNoise) {
            delete m_opNoise;
        }
        if (m_densNoise) {
            delete m_densNoise;
        }
    }

};

class NoiseDensityStub: public IDensityMatrixNoise
{
public:
    NoiseDensityStub(const std::string &, int) {}
    void ApplyNoiseForDensityMatrix(IQRegister &) {}
};


template<class NoiseOp, class NoiseDens> INoiseProvider * CreateNoiseProviderStrImpl(const std::string &filename) {
    return new SimpleNoiseProvider(new NoiseDens(filename), new NoiseOp());
}


template<class NoiseOp, class NoiseDens> INoiseProvider * CreateNoiseProviderStrDimImpl(const std::string &filename, int dim) {
    return new SimpleNoiseProvider(new NoiseDens(filename, dim), new NoiseOp());
}


#endif // NOISE_H

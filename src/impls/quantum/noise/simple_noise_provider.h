#ifndef SIMPLENOISEPROVIDER_H
#define SIMPLENOISEPROVIDER_H
#include <iface/quantum/inoise.h>
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


template<class NoiseOp, class NoiseDens> INoiseProvider * CreateNoiseProviderStrImpl(const std::string &filename) {
    return new SimpleNoiseProvider(new NoiseDens(filename), new NoiseOp());
}


template<class NoiseOp, class NoiseDens> INoiseProvider * CreateNoiseProviderStrDimImpl(const std::string &filename, int dim) {
    return new SimpleNoiseProvider(new NoiseDens(filename, dim), new NoiseOp());
}


#endif // SIMPLENOISEPROVIDER_H


#ifndef CRAUSS_NOISE_H
#define CRAUSS_NOISE_H
#include <iface/quantum/inoise.h>
#include <vector>


enum {
    CRAUSS_MAX_DIM_V1 = 3
    };
class CraussNoiseImpl: public IOperatorNoise {
    std::vector<QMatrix> m_CraussOps[CRAUSS_MAX_DIM_V1];


public:
    CraussNoiseImpl(const std::string &filename);
    QMatrix GenNoisyMatrix(const QMatrix &m);
};


class CraussNoiseDensityImpl: public IDensityMatrixNoise
{
    std::vector<std::pair<QMatrix, std::vector<uint_type> > > m_CraussOps;

    int m_dim;
public:
    CraussNoiseDensityImpl(const std::string &filename, int dim);
    void ApplyNoiseForDensityMatrix(IQRegister &reg);
};




#endif // CRAUSS_NOISE_H

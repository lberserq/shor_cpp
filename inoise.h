#ifndef INOISE_H
#define INOISE_H
#include "qmatrix.h"
#include "qregister.h"

struct IOperatorNoise {
    virtual QMatrix GenNoisyMatrix(const QMatrix &m) = 0;
    virtual ~IOperatorNoise(){}
};

struct IDensityMatrixNoise {
    virtual  void ApplyNoiseForDensityMatrix(IQRegister &reg) = 0;
    virtual ~IDensityMatrixNoise(){}
};

struct INoiseProvider {
    virtual IOperatorNoise * GetOperatorNoise() = 0;
    virtual IDensityMatrixNoise * GetDensityMatrixNoise() = 0;
    virtual ~INoiseProvider() {}
};


extern INoiseProvider * CreateNoiseProvider(const std::string &filename);


#endif // INOISE_H


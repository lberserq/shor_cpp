#ifndef QREGISTERFACTORY_H
#define QREGISTERFACTORY_H
#include <iface/quantum/iqregister.h>

template <class RegImpl>
class StandardQRegisterFactory: public IQRegisterFactory
{
public:
    result_t CreateRegister(size_t width, state_t startState, mcomplex ampl, representation_t representation, IQRegister **out)
    {
        if (!out)
            return X_CONTRACT_VIOLATON;
        *out = new RegImpl(width, startState, ampl, representation);
        return X_SOK;
    }
};

#endif // QREGISTERFACTORY_H

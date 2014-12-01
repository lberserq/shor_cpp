#include "measure.h"
#include <cstdlib>

state Measurer::Measure(Qregister &reg)
{
    double rnd = static_cast<double>(rand()) / RAND_MAX;
    for (int i = 0; i < reg.getSize(); i++) {
        mcomplex ampl = reg.getAmpls()[i];
        long double probability = std::abs(ampl) * std::abs(ampl);
        rnd -= probability;
        if (rnd < 0 || std::abs(rnd - probability) < g_eps) {
            return reg.getStates()[i];
        }

    }
    return -1;
}

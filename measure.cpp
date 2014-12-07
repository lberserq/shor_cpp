#include "measure.h"
#include <cstdlib>

state Measurer::Measure(Qregister &reg)
{
    double rnd = static_cast<double>(rand()) / RAND_MAX;
    int  i = 0;
    register int sz = reg.getSize();
    while (i < sz && !(rnd < 0)) {
        mcomplex ampl = reg.getAmpls()[i];
        long double probability = std::abs(ampl) * std::abs(ampl);
        rnd -= probability;
        if (rnd < 0 || std::abs(rnd - probability) < g_eps) {
            return reg.getStates()[i];
        }
        i++;
    }
    return -1;
}

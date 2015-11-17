#include "measure.h"
#include "common.h"
#include <cstdlib>
#include <unistd.h>


state Measurer::Measure(IQRegister &reg)
{
    //double rnd = static_cast<double>(rand()) / RAND_MAX;

    double rnd = drand48();
    for (state i = 0; i < reg.getStatesSize(); i++) {
        if (std::abs(reg.getStates()[i]) > g_eps) {
            mcomplex ampl = reg.getStates()[i];
            long double probability = std::abs(ampl) * std::abs(ampl);
            rnd -= probability;
            if (rnd < 0 || std::abs(rnd - probability) < g_eps) {
                return i;
            }
        }

    }
    return -1;
}

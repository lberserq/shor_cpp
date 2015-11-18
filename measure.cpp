#include "measure.h"
#include "common.h"
#include <cstdlib>
#include <unistd.h>


state Measurer::Measure(Qregister &reg)
{
    //double rnd = static_cast<double>(rand()) / RAND_MAX;

    double rnd = drand48();
    for (state i = 0; i < reg.getSize(); i++) {
        if (std::abs(reg.getAmpls()[i]) > g_eps) {
            mcomplex ampl = reg.getAmpls()[i];
            long double probability = std::abs(ampl) * std::abs(ampl);
            rnd -= probability;
            if (rnd < 0 || std::abs(rnd - probability) < g_eps) {
                return i;
            }
        }

    }
    return -1;
}
